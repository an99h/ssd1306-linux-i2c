#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>

#include <asm/pgtable.h>
#include <linux/mm.h>

#include "ioctrl_args.h"
#include "ssd1306.h"

#define DRV_COUNT 1
#define OLED_NAME "oled"

struct _chr_env
{
    int major;
    int number;
    struct class *class;
};

typedef struct _oled_ctx
{
    struct cdev cdev;
    dev_t devid;
    char* kernel_buf;
} oled_ctx;

static struct _chr_env chr_env;

static int oled_open(struct inode *node, struct file *file);
static int oled_close(struct inode *node, struct file *file);
static ssize_t oled_write(struct file *file, const char __user *buf, size_t size, loff_t *offset);
static long oled_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int oled_mmap(struct file* file, struct vm_area_struct* vma);

static int oled_remove(struct i2c_client *client);
static int oled_probe(struct i2c_client* client,
    const struct i2c_device_id* id);

// 字符驱动描述结构体
static struct file_operations oled_ops = {
    .owner   = THIS_MODULE,
    .open    = oled_open,
    .write   = oled_write,
    .release = oled_close,
    .unlocked_ioctl = oled_ioctl,
    .mmap = oled_mmap,
};


static const struct i2c_device_id oled_id[] = {
    { "ssd1306", 0 },
    { }
};

MODULE_DEVICE_TABLE(i2c, oled_id);

static const struct of_device_id oled_of_match[] = {
    {
        .compatible = "solomon,ssd1306fb-i2c",
    },
    {}
};

static struct i2c_driver oled_driver = {
    .probe = oled_probe,
    .remove = oled_remove,
    .id_table = oled_id,
    .driver = {
        .name = OLED_NAME,
        .of_match_table = oled_of_match,
    },
};

static int oled_open(struct inode *node, struct file *file)
{
    oled_ctx *ctx = container_of(node->i_cdev, oled_ctx, cdev);
    
    file->private_data = ctx;
    
    return 0;
}

static int oled_close(struct inode *node, struct file *file)
{
    oled_ctx *ctx = (oled_ctx *)file->private_data;
    ctx = ctx;
    
    return 0;
}

static ssize_t oled_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    int ret = 0;
    struct write_msg msg;
    oled_ctx *ctx = (oled_ctx *)file->private_data;
    ctx = ctx;
    
    ret = copy_from_user((void*)&msg, buf, size);
    if (ret < 0)
    {
        printk("copy from user error\r\n");
		return -EFAULT;
    }
    
    //printk("msg x = %d, y = %d, font = %d, str = %s\n", msg.x, msg.y, msg.font, msg.str);
    
    ssd1306_display_string(msg.x, msg.y, msg.str, msg.font, 1);
    ssd1306_refresh_gram();
    
    return 0;
}

static long oled_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct io_ctrl ioctrl;
    oled_ctx* ctx = (oled_ctx*)file->private_data;

    switch (cmd)
    {
    case OPEN_OLED_DISPLAY:
        ssd1306_display_on();
        break;    
    case CLOSE_OLED_DISPLAY:
        ssd1306_display_off();
        break;
    case CLEAR_SCREEN:
        ssd1306_clear_screen(0x00);
        break;
    case REFRESH_SCREEN:
        ssd1306_draw_image(ctx->kernel_buf);
        break;
	case DRAW_RECT:
	{
		int ret = 0;
		
		ret = copy_from_user((void*)&ioctrl, (void*)arg, sizeof(struct io_ctrl));
		if (ret < 0)
		{
			return -EFAULT;
		}
        //printk("x = %u, y = %u, width = %u, height = %u\r\n", ioctrl.rect.x, ioctrl.rect.y, ioctrl.rect.xLen, ioctrl.rect.yLen);

        if (ioctrl.rect.fill)
        {
            ssd1306_fill_screen(ioctrl.rect.x, ioctrl.rect.y, ioctrl.rect.xLen, ioctrl.rect.yLen, 1);
        }
        else
        {
            ssd1306_draw_rectangle(ioctrl.rect.x, ioctrl.rect.y, ioctrl.rect.xLen, ioctrl.rect.yLen, 1);
        }
		break;
	}
    case DRAW_HLINE:
    {
        int ret = 0;
		ret = copy_from_user((void*)&ioctrl, (void*)arg, sizeof(struct io_ctrl));
		if (ret < 0)
		{
			return -EFAULT;
		}
        ssd1306_draw_rectangle(ioctrl.hLine.x, ioctrl.hLine.y, ioctrl.hLine.xLen,0, 1);
        break;
    }
    case DRAW_VLINE:
    {
        int ret = 0;
		ret = copy_from_user((void*)&ioctrl, (void*)arg, sizeof(struct io_ctrl));
		if (ret < 0)
		{
			return -EFAULT;
		}
        ssd1306_draw_rectangle(ioctrl.vLine.x, ioctrl.vLine.y, 0, ioctrl.vLine.yLen, 1);
        break;
    }
    case DRAW_IMAGE:
    {
        int ret = 0;
        static unsigned char image[1024] = { 0 };
		ret = copy_from_user((void*)&ioctrl, (void*)arg, sizeof(struct io_ctrl));
        if (ret < 0)
		{
			return -EFAULT;
		}
        ret = copy_from_user((void*)image, (void*)ioctrl.imageView.image, 1024);
        ssd1306_draw_image(image);
        break;
    }
    default:
		break;
	}
    
    return 0;
}

static int oled_mmap(struct file* file, struct vm_area_struct* vma)
{
    oled_ctx* ctx = (oled_ctx*)file->private_data;

    /* 获得物理地址 */
    unsigned long phy = virt_to_phys(ctx->kernel_buf);

    /* 设置属性: cache, buffer */
    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    /* map */
    if (remap_pfn_range(vma, vma->vm_start, phy >> PAGE_SHIFT,
        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        printk("mmap remap_pfn_range failed\n");
        return -ENOBUFS;
    }

    return 0;
}

static int oled_probe(struct i2c_client* client,
    const struct i2c_device_id* id)
{
    int ret = 0;
    struct device *device = NULL;
    oled_ctx *ctx = kmalloc(sizeof(oled_ctx), GFP_KERNEL);
    ctx->kernel_buf = kmalloc(1024*8, GFP_KERNEL);
    i2c_set_clientdata(client, ctx);
    
    ctx->devid = MKDEV(chr_env.major, chr_env.number++);
    ctx->cdev.owner = THIS_MODULE;
    cdev_init(&ctx->cdev, &oled_ops);
    ret = cdev_add(&ctx->cdev, ctx->devid, 1);
    if (ret < 0)
    {
        printk("cdev_add error\n");
        return -1;
    }
    
    device = device_create(chr_env.class, NULL, ctx->devid, NULL, "%s", OLED_NAME);
    if (IS_ERR(device))
    {
        printk("device_create error\n");
        ret = PTR_ERR(device);
        return -2;
    }
    
    printk("probe %s\n", __func__);
    printk("%s, addr = %x, line = %d\n", __func__, client->addr, client->adapter->nr);
    printk("%s, name = %s, adapter nr = %d\n", __func__, client->adapter->name, client->adapter->nr);
    
    set_ssd1306_i2c_client(client);

    ssd1306_init();
    ssd1306_display_on();
    ssd1306_clear_screen(0xff);
    ssd1306_clear_screen(0x00);
    
    return 0;
};


static int oled_remove(struct i2c_client *client)
{
    oled_ctx *ctx = i2c_get_clientdata(client);

    ssd1306_clear_screen(0x00);
    ssd1306_display_off();

    device_destroy(chr_env.class, ctx->cdev.dev);
    cdev_del(&ctx->cdev);
    
    if (--chr_env.number == 0)
    {
        class_destroy(chr_env.class);
        unregister_chrdev_region(ctx->devid, DRV_COUNT);
    }
    
    kfree(ctx->kernel_buf);
    kfree(ctx);
    return 0;
}

static int __init oled_dev_init(void)
{
    int ret = 0;
    dev_t devid = 0;
    
    ret = alloc_chrdev_region(&devid, 0, DRV_COUNT, OLED_NAME);
    if (ret < 0)
    {
        return -1;
    }
    
    chr_env.major = MAJOR(devid);
    chr_env.class = class_create(THIS_MODULE, OLED_NAME);
    if (IS_ERR(chr_env.class))
    {
        ret = PTR_ERR(chr_env.class);
        unregister_chrdev_region(devid, DRV_COUNT);
        return ret;
    }
    
    return i2c_add_driver(&oled_driver);
}


static void __exit oled_dev_exit(void)
{
    i2c_del_driver(&oled_driver);
}

module_init(oled_dev_init);
module_exit(oled_dev_exit);
MODULE_LICENSE("GPL");

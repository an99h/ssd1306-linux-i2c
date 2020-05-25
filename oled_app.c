#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "ioctrl_args.h"
#include "images.h"


int main()
{
	int i = 0;
    int ret = 0;
    int font = 0;
    int fd_oled = 0;
    int fd_ap3216c = 0;
    unsigned short databuf[3];
	char *sen_name[] = {"ir","als", "ps"};
    
    struct write_msg msg;
    memset(&msg, 0x0, sizeof(struct write_msg));
    msg.x = 0;
    msg.y = 0;
    msg.font = 16;

    fd_oled = open("/dev/oled", O_RDWR);
    if(fd_oled < 0) {
        printf("can't open file ssd1306\r\n");
        return -1;
    }
#if 0
    fd_ap3216c = open("/dev/ap3216c", O_RDWR);
    if (fd_ap3216c < 0) {
        printf("can't open file ap3216c\r\n");
        return -1;
    }
#endif
    ioctl(fd_oled, CLEAR_SCREEN);

    struct io_ctrl ioctrl;
#if 1
    ioctrl.rect.x = 0;
    ioctrl.rect.y = 0;
    ioctrl.rect.xLen = 125;
    ioctrl.rect.yLen = 63;
    ioctrl.rect.fill = 0;
    for (i = 0; i < 1; i++)
    {
        if (i == 5)
        {
            ioctrl.rect.fill = 1;
        }
        
        ioctl(fd_oled, DRAW_RECT, &ioctrl);
        ioctrl.rect.x += 5;
        ioctrl.rect.y += 5;
        ioctrl.rect.xLen -= 10;
        ioctrl.rect.yLen -= 10;
    }
    
    ioctrl.hLine.x = 0;
    ioctrl.hLine.y = 20;
    ioctrl.hLine.xLen = 125;
    ioctl(fd_oled, DRAW_HLINE, &ioctrl);
    ioctrl.hLine.y = 40;
    ioctl(fd_oled, DRAW_HLINE, &ioctrl);

    ioctrl.vLine.x = 28;
    ioctrl.vLine.y = 0;
    ioctrl.vLine.yLen = 63;
    ioctl(fd_oled, DRAW_VLINE, &ioctrl);
    ioctrl.vLine.x = 100;
    ioctl(fd_oled, DRAW_VLINE, &ioctrl);
#endif
#if 0
    while (1)
    {
        ret = read(fd_ap3216c, databuf, sizeof(databuf));
        if (ret != 0)
        {
			usleep(20000);
            continue;
        }
		
		for(i = 0; i < 3; i++)
		{
			msg.y = i * 20 + 2;
            msg.x = 30;
			sprintf(msg.str, "%s:%04d",sen_name[i], databuf[i]);
			write(fd_oled, (void*)&msg, sizeof(struct write_msg));
			memset(msg.str, 0, 128);
		}

        usleep(100000);
    }
#endif
#if 1
    ioctrl.imageView.image = (unsigned char *)malloc(1024);
    while (1)
    {
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_apple, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_mi, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_wifi, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_ig, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_cat, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_lufei, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_ox, 1024);
        ioctl(fd_oled, DRAW_IMAGE, &ioctrl);
    }
    
    free(ioctrl.imageView.image);
#endif 
    close(fd_oled);
    close(fd_ap3216c);
    
    return 0;
}

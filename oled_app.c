#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "ioctrl_args.h"
#include "images.h"

void case1(int fd);
void case2(int fd);
void case3(int fd);
void case4(int fd);
void display_str(int x, int y, char *str, int fd);

int main()
{
    int fd_oled = 0;

    fd_oled = open("/dev/oled", O_RDWR);
    if(fd_oled < 0) {
        printf("can't open file ssd1306\r\n");
        return -1;
    }

    ioctl(fd_oled, CLEAR_SCREEN);

    case1(fd_oled);

    sleep(1);

    ioctl(fd_oled, CLEAR_SCREEN);

    case2(fd_oled);

    sleep(1);

    ioctl(fd_oled, CLEAR_SCREEN);

    case3(fd_oled);

    ioctl(fd_oled, CLEAR_SCREEN);

    case4(fd_oled);

    close(fd_oled);
    
    return 0;
}


void display_str(int x, int y, char *str, int fd)
{
    struct write_msg args;
    memset(&args, 0x0, sizeof(struct write_msg));
    args.x = x;
    args.y = y;
    args.font = 16;

    strcpy(args.str,str);

    write(fd, (void*)&args, sizeof(struct write_msg));
}

void case1(int fd)
{
    display_str(40, 20, "case1", fd);
    display_str(0, 0, "123456789", fd);
    display_str(10, 10, "asdfghjk", fd);
    display_str(0, 40, "dsafjisdfso", fd);
}

void case2(int fd)
{
    int i = 0;
    struct io_ctrl ioctrl;

    display_str(40, 20, "case2", fd);

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
        
        ioctl(fd, DRAW_RECT, &ioctrl);
        ioctrl.rect.x += 5;
        ioctrl.rect.y += 5;
        ioctrl.rect.xLen -= 10;
        ioctrl.rect.yLen -= 10;
    }
    
    ioctrl.hLine.x = 0;
    ioctrl.hLine.y = 20;
    ioctrl.hLine.xLen = 125;
    ioctl(fd, DRAW_HLINE, &ioctrl);
    ioctrl.hLine.y = 40;
    ioctl(fd, DRAW_HLINE, &ioctrl);

    ioctrl.vLine.x = 28;
    ioctrl.vLine.y = 0;
    ioctrl.vLine.yLen = 63;
    ioctl(fd, DRAW_VLINE, &ioctrl);
    ioctrl.vLine.x = 100;
    ioctl(fd, DRAW_VLINE, &ioctrl);
}


void case3(int fd)
{
    int i = 0;
    struct io_ctrl ioctrl;

    display_str(40, 20, "case3", fd);

    ioctrl.imageView.image = (unsigned char *)malloc(1024);

    for (i = 0; i < 1; i++)
    {
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_apple, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_mi, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_wifi, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_ig, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_cat, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_lufei, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
        sleep(1);
        memcpy(ioctrl.imageView.image, gImage_ox, 1024);
        ioctl(fd, DRAW_IMAGE, &ioctrl);
    }
    
    free(ioctrl.imageView.image);
}

void case4(int fd)
{
    char *img = NULL;
    display_str(40, 20, "case4", fd);

    img = mmap(NULL, 1024*8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (img == MAP_FAILED)
	{
		printf("can not mmap file /dev/oled\n");
		return;
	}

    while (1)
    {
        sleep(1);
        memcpy(img, gImage_apple, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_mi, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_wifi, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_ig, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_cat, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_lufei, 1024);
        ioctl(fd, REFRESH_SCREEN);
        sleep(1);
        memcpy(img, gImage_ox, 1024);
        ioctl(fd, REFRESH_SCREEN);
    }
}

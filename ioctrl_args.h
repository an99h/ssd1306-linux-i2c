#ifndef __OLED_H__
#define __OLED_H__

struct write_msg
{
    int x;
    int y;
    int font;
    char str[128];
};

struct draw_rect
{
    int x;
    int y;
    int xLen; // x + xLen < 125
    int yLen; // y + yLen < 63
    int fill; // 1 fill, 0 not fill
};

struct draw_hLine
{
    int x;
    int y;
    int xLen;
};

struct draw_vLine
{
    int x;
    int y;
    int yLen;
};

struct draw_image
{
    unsigned char *image;
};

struct io_ctrl
{
    struct draw_rect  rect;
    struct draw_vLine vLine;
    struct draw_hLine hLine;
    struct draw_image imageView;
};

/*
_IO(type,nr)        // 没有参数的命令
_IOR(type,nr,size)  // 从驱动读取数据
_IOW(type,nr,size)  // 向驱动写入数据
_IOWR(type,nr,size) // 双向数据传输
*/

#define OPEN_OLED_DISPLAY        _IO(0XEF, 0x1)
#define CLOSE_OLED_DISPLAY       _IO(0XEF, 0x2)
#define CLEAR_SCREEN             _IO(0XEF, 0x3)
#define REFRESH_SCREEN           _IO(0XEF, 0x8)
#define DRAW_RECT                _IOW(0XEF, 0x4, struct io_ctrl)
#define DRAW_HLINE               _IOW(0XEF, 0x5, struct io_ctrl)
#define DRAW_VLINE               _IOW(0XEF, 0x6, struct io_ctrl)
#define DRAW_IMAGE               _IOW(0XEF, 0x7, struct io_ctrl)

#endif

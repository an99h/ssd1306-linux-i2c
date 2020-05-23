KERN_DIR = /root/linux/100ask_imx6ull-sdk/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules
	$(CROSS_COMPILE)gcc oled_app.c -o oled_app

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order oled_app

oled-y := oled_drv.o ssd1306.o
obj-m  += oled.o

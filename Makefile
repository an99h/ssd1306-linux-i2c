KERN_DIR = /home/pi/Desktop/linux/rasberry_kernel

all:
	make -C $(KERN_DIR) M=`pwd` modules
	$(CROSS_COMPILE)gcc oled_app.c -o oled_app

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order oled_app

oled-y := oled_drv.o ssd1306.o
obj-m  += oled.o

install:
	sudo insmod oled.ko

uninstall:
	sudo rmmod oled

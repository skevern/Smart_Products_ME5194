obj-m:=interrupt_module_mmap.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/pi/Desktop/Smart Products/lab3/Make_folder modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/pi/Desktop/Smart Products/lab3/Make_folder clean



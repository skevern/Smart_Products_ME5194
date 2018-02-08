//=====================================================================
// -------------------------------interrupt_module_mmap.c ------------------------
//=====================================================================
/*
This file is used to make a character device for a interrupt driver. which can
 autonomously access the information from the main user space application.
 In this particular this module can act as a background process to asychronously
 handle the permission/requests signals from a bus arbiter as interrupts, then sending
 acknowledgement signals back to the arbiter within the function
 	 -------------------------------------------------------
 	static  irqreturn_t irq_handler(int irq, void *dev_id)
 	 -------------------------------------------------------
 This function will handle the inital interrupt, send acknowledgements signals,
 and schedule a worker function to be queued in the workerqueue.
 The function that then handles the bottom half of the ISR is 
	 ------------------------------------------------------
 	static void user_callhelper(struct work_struct *work)
 	 ------------------------------------------------------
 which retrieves the memeory mapped data structure from userspace, encodes its 
 contents single string, and passes it into an auxillary application in userspace that
 can perform more advanced operations and transmit it to other devices. 
 
 One must specify the GPIO pins that are used as interrupts and acknowledgement 
 signals, as well as the buffer size (in number of bytes) of the packets to be stored
 and encoded into the auxillary function.
======================================================================
	@author Dylan DeSantis	Github: DDDeSantis
	@version 1.0.0
	@date 1/18/2018
*/
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include<linux/uaccess.h>
#include<linux/interrupt.h>
#include<linux/string.h>
#include<linux/workqueue.h>
#include<linux/slab.h>
#include<linux/mm.h>
#include<linux/proc_fs.h>
#include<linux/types.h>
#include <linux/version.h>  
#include <linux/fs.h>  
#include <linux/cdev.h>  
#include <linux/vmalloc.h>  
#ifdef MODVERSIONS  
#  include <linux/modversions.h>  
#endif  
#include <asm/io.h>  
  
//========================================================
// --------------------------MODULE INFO--------------------------
//========================================================
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dylan DeSantis");
MODULE_DESCRIPTION("Linux Kernel module using GPIO interrupts and BH handlers");
MODULE_VERSION("0.1");
//========================================================

//========================================================
// --------------GLOBAL CONSTS/VARS-----------------------
//========================================================
#define WQ_NAME "IRQ_queue" // Name for the workqueue
static char *envp[ ] = {
	"SHELL=/bin/bash",
	"HOME=/home/pi",
	"USER=pi",
	"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/games:/usr/games",
	//"DISPLAY=:0.0",
	"PWD=/home/pi",
	NULL};//				Envirorment setting
static struct gpio ack[1]; //	GPIO acknowledgement structure
static struct gpio trig[1];//	GPIO interrupt structure
static int irq_trig = 1;//		Interrupt index in IVT



//========================================================
// -----------MODULE INPUT PARAM-----------------
//========================================================
static int ack_pnum ; //	GPIO Pin to transmit requests and acknowledgements
static int trig_pnum; //	GPIO Pin to recieve external interrupts
 char *exec_path; // 	Path to the executable application in userspace
 int buff_size; //		How many bytes to read from userspace
//Register these parameters and describe there type
module_param(ack_pnum, int, 0);
module_param(trig_pnum, int, 0);
module_param(exec_path, charp, 0);
module_param(buff_size, int, 0);
//=========================================================

//========================================================
// ----------CHAR DEV DECLARE/ GLOABL VARS-----------
//========================================================
/* character device structures */  
static dev_t mmap_dev;  
static struct cdev mmap_cdev;  
 /* methods of the character device */  
static int mmap_open(struct inode *inode, struct file *filp);  
static int mmap_release(struct inode *inode, struct file *filp);  
static int mmap_mmap(struct file *filp, struct vm_area_struct *vma);   

/* the file operations, i.e. all character device methods */  
static struct file_operations mmap_fops = {  
        .open = mmap_open,  
        .release = mmap_release,  
        .mmap = mmap_mmap,  
        .owner = THIS_MODULE  
};  
// internal data  
// length of the two memory areas (1 page each)  
#define NPAGES 4 
// pointer to the vmalloc'd area - alway page aligned  
static int *vmalloc_area;  
// pointer to the kmalloc'd area, rounded up to a page boundary  
static int *kmalloc_area;  
// original pointer for kmalloc'd area as returned by kmalloc  
static void *kmalloc_ptr;    
//========================================================
//========================================================
// ---------------MMAP DEFINITIONS----------------
//========================================================
/* character device open method */  
static int mmap_open(struct inode *inode, struct file *filp)  
{  
        return 0;  
}  
/* character device last close method */  
static int mmap_release(struct inode *inode, struct file *filp)  
{  
        return 0;  
}  
  
// helper function, mmap's the kmalloc'd area which is physically contiguous  
int mmap_kmem(struct file *filp, struct vm_area_struct *vma)  
{  
        int ret;  
        long length = vma->vm_end - vma->vm_start;  
        /* check length - do not allow larger mappings than the number of 
           pages allocated */  
        if (length > NPAGES * PAGE_SIZE)  
                return -EIO;  
        /* map the whole physically contiguous area in one piece */  
        if ((ret = remap_pfn_range(vma,  
                                   vma->vm_start,  
                                   virt_to_phys((void *)kmalloc_area) >> PAGE_SHIFT,  
                                   length,  
                                   vma->vm_page_prot)) < 0) {  
                return ret;  
        }  
        return 0;  
}  
// helper function, mmap's the vmalloc'd area which is not physically contiguous  
int mmap_vmem(struct file *filp, struct vm_area_struct *vma)  
{  
        int ret;  
        long length = vma->vm_end - vma->vm_start;  
        unsigned long start = vma->vm_start;  
        char *vmalloc_area_ptr = (char *)vmalloc_area;  
        unsigned long pfn;  
  
        /* check length - do not allow larger mappings than the number of 
           pages allocated */  
        if (length > NPAGES * PAGE_SIZE)  
                return -EIO;  
  
        /* loop over all pages, map it page individually */  
        while (length > 0) {  
                pfn = vmalloc_to_pfn(vmalloc_area_ptr);  
                if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,  
                                           PAGE_SHARED)) < 0) {  
                        return ret;  
                }  
                start += PAGE_SIZE;  
                vmalloc_area_ptr += PAGE_SIZE;  
                length -= PAGE_SIZE;  
        }  
        return 0;  
}  
  
/* character device mmap method */  
static int mmap_mmap(struct file *filp, struct vm_area_struct *vma)  
{  
        /* at offset 0 we map the vmalloc'd area */  
        if (vma->vm_pgoff == NPAGES) {  
                return mmap_vmem(filp, vma);  
        }  
        /* at offset NPAGES we map the kmalloc'd area */  
        if (vma->vm_pgoff ==0) {  
                return mmap_kmem(filp, vma);  
        }  
        /* at any other offset we return an error */  
        return -EIO;  
}  
//========================================================
// ----------------WORKQUEUE-------------------
//========================================================
//This is how we register a bottom half handler
struct irq_work_struct {
	struct work_struct work;
	int b_size;
	struct mutex buff_lock;
};
//	Declaring a global workqueue
static struct workqueue_struct *wq;
static void user_callhelper(struct work_struct *work)
{
	//	Setup our worker 
	struct irq_work_struct *temp_work;
	temp_work = (struct irq_work_struct *)work; //	Initializing a temporary work struct
	printk(" Executing Worker Function: Buffer size = %d  \n", temp_work->b_size);
	// 	CRITICAL SECTION
	mutex_lock(&temp_work->buff_lock);
	int i;
	int CSIZE = sizeof(kmalloc_area[0]+1); //Make sure our packets are all the same size
	printk(" user helper pointer  %p \n", kmalloc_ptr);
	char *cpy_buffer = (char*)kmalloc(CSIZE,GFP_KERNEL); 
	char *mess;
	mess = "x"; // String message which we encode our packets into
	const char delim[] = "x"; //This is the deliminator to seperate our packets
	for( i= 1; i <temp_work->b_size+1; i++)
	{
		snprintf(cpy_buffer, sizeof(kmalloc_area[i]), "%u", kmalloc_area[i]);
		mess=strncat(mess, cpy_buffer,sizeof(kmalloc_area[i]));
		mess=strncat(mess, delim, sizeof(delim));
		printk("element  %d , %X \n", i, (int)kmalloc_area[i]);
		printk("message %d, %s \n", i, mess);
		kmalloc_area[i] = 0;
		kmalloc_area[0] = kmalloc_area[0] -1;
	}
	//	Parameters passed into the userspace application
	char *argv[ ] = {exec_path, mess, NULL};			
	call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC); //wait until application finishes (alt: UMH_WAIT_EXEC)
	//	Free the allocated memory 
	kfree(cpy_buffer);
	mutex_unlock(&temp_work->buff_lock);
	kfree(work);
	return;
}
//========================================================

//========================================================
// ---------INTERRUPT  FUNC-----------------
//========================================================
//	Interrupt Service Routine (ISR)
static  irqreturn_t irq_handler(int irq, void *dev_id)
{
	// -send acknowledgement --
	//===========================
	// flips the state of the acknowledgement line
	// ***this needs more logic if to be used as an actual acknowledgement***
	//***for now it just flips on or off an LED to identify when an 
	//  	interrupt has occurred ****
	//============================
	int sig;
	sig = gpio_get_value(ack[0].gpio); //get the current state of ack. line
	if (sig ==0)
	 {
	 	sig = ~sig;
	 	gpio_set_value(ack[0].gpio, sig);
	 }
	else{ 
		//Do something if you expect interrupts
		// to occur asyn. with the BH ISR
	}
	//============================
	
	// ----- Register Bottom Half -------
	//===========================
	// create a new worker to putinto the workqueue
	// we can put in different workers depending on the type of interrupt
	//============================
	printk("INTERRUPT");
	//schedule the tasklet handler
	struct irq_work_struct *temp_work;
	temp_work = (struct irq_work_struct *)kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!temp_work) { pr_err("failed to kmalloc temp_work");}
	else {
		INIT_WORK( (struct work_struct *)temp_work, &user_callhelper);
		temp_work->b_size = buff_size;
		mutex_init(&temp_work->buff_lock);
		queue_work(wq, (struct work_struct *)temp_work);
	}
	//============================
	return IRQ_HANDLED; //notify that we have handled the IRQ
}

//	Initialization Function
static int  __init irq_init_man(void)
{
	int ret, i;
	ret = 0;
	
	//============================ WORKQUEUE INIT ==========================
	//	Workqueue Allocation
	wq = alloc_ordered_workqueue(WQ_NAME, WQ_MEM_RECLAIM);
	//========================= GPIO REGISTER ==========================
	//	GPIO acknowledge structure
	ack[0].gpio = ack_pnum;
	ack[0].flags = GPIOF_OUT_INIT_LOW; //initially ouput is low
	ack[0].label = "Output Pin";
	//	GPIO irq trigger structure
	trig[0].gpio = trig_pnum;
	trig[0].flags = GPIOF_IN;
	trig[0].label = "Input Pin";
	//	Registering GPIO
	ret = gpio_request_array(ack, ARRAY_SIZE(ack)); //returns 0 if successful
	if (ret) {printk(KERN_ERR "Unable to request GPIOs for acknowledge pins: %d \n", ret);}
	ret = gpio_request_array(trig, ARRAY_SIZE(trig));//returns 0 if successful
	if(ret) {printk(KERN_ERR  "Unable to request GPIOs for trigger pins %d \n", ret);}
	//=================== INTERRUPT REGISTER =========================
	//	Registering IRQ /ISR
	ret = gpio_to_irq( trig[0].gpio);//returns the irq index in the vector table
	irq_trig = ret;
	ret = request_irq(irq_trig,  //irq number corresponding to the gpio trigger pin
				irq_handler, //function to handle the interrupt request
				IRQF_TRIGGER_RISING,  // what is triggering the interrrupt (goes from low to high)
				"GPIO_trigger_1", // give it a name (can be seen in /proc/interrupt)
				 NULL); //register the ISR for the trigger,

	//============================ MMAP INIT ==========================
        /* allocate a memory area with kmalloc. Will be rounded up to a page boundary */  
        ret = 0;
        if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) == NULL) { 
                ret = -ENOMEM;  
                goto out;  
        }  
        /* round it up to the page bondary */  
        kmalloc_area = (int *)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);  
        /* allocate a memory area with vmalloc. */  
        if ((vmalloc_area = (int *)vmalloc(NPAGES * PAGE_SIZE)) == NULL) {  
                ret = -ENOMEM;  
                goto out_kfree;  
        }  
        /* get the major number of the character device */  
        if ((ret = alloc_chrdev_region(&mmap_dev, 0, 1, "lab3_dev")) < 0) {  
                printk(KERN_ERR "could not allocate major number for mmap\n");  
                goto out_vfree;  
        }  
        /* initialize the device structure and register the device with the kernel */  
        cdev_init(&mmap_cdev, &mmap_fops);  
        if ((ret = cdev_add(&mmap_cdev, mmap_dev, 1)) < 0) {  
                printk(KERN_ERR "could not allocate chrdev for mmap\n");  
                goto out_unalloc_region;  
        }  
        /* mark the pages reserved */  
        for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {  
                SetPageReserved(vmalloc_to_page((void *)(((unsigned long)vmalloc_area) + i)));  
                SetPageReserved(virt_to_page(((unsigned long)kmalloc_area) + i));  
        }  
        /* initalize memory */  
        for (i = 0; i < (NPAGES * PAGE_SIZE / sizeof(int)); i ++) {  
                vmalloc_area[i] = (0x0);// initially set all memory values to zero
                kmalloc_area[i] = (0x0);// initally set all memory values to zero
        }   
        return ret;  
  out_unalloc_region:  
        unregister_chrdev_region(mmap_dev, 1);  
  out_vfree:  
        vfree(vmalloc_area);  
  out_kfree:  
        kfree(kmalloc_ptr);  
  out:  
        return ret;  
       	//============================ END INIT ==========================
}

//	Destroy Function
static void __exit irq_exit_man(void)
{
	 int i;  
        /* remove the character deivce */  
        cdev_del(&mmap_cdev);  
        unregister_chrdev_region(mmap_dev, 1);  
 
        /* unreserve the pages */  
        for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {  
                SetPageReserved(vmalloc_to_page((void *)(((unsigned long)vmalloc_area) + i)));  
                SetPageReserved(virt_to_page(((unsigned long)kmalloc_area) + i));  
        }  
        /* free the memory areas */  
        vfree(vmalloc_area);  
        kfree(kmalloc_ptr);  
        
        
	free_irq(irq_trig, NULL); // unregister the ISR from the vector table
	gpio_free_array(trig, ARRAY_SIZE(trig));//free the gpio acknowledgement 
	gpio_free_array(ack, ARRAY_SIZE(ack));//free the gpio trigger irq
	flush_workqueue(wq);// empty our queue
	destroy_workqueue(wq);// free  the allocated memory

	return;
}
//=======================================================

//========================================================
// ---------REGISTER INIT/EXIT FUNC---------
//========================================================
 //functions to call when we insert and remove the module into kernel space
module_init(irq_init_man);// inserted  with ~ insmod 
module_exit(irq_exit_man); // removed with ~ rmmod
//=======================================================




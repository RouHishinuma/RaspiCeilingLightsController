#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>


MODULE_AUTHOR("Ryuichi Ueda and Akifumi Takagi");
MODULE_DESCRIPTION("driver for IRled control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

void T1High(int times);
void T1Low(int times);
void Bit1(void);
void Bit0(void);
void sendBit(int onebit);

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	char c;  
	if(copy_from_user(&c,buf,sizeof(char)))
		return -EFAULT;


	int signal;
	char flag = 0;
	switch(c)
	{
		case 1 :
			signal = 0x02FD807F;
			break;
		case 2 : 
			signal = 0x02FD40BF;
			break;
		case 3 : 
			signal = 0x02FDC03F;
			break;
		case 4 : 
			signal = 0x02FD20DF;
			break;
		case 5 : 
			signal = 0x02FDA05F;
			break;
		case 6 : 
			signal = 0x02FD609F;
			break;
		case 7 : 
			signal = 0x02FDE01F;
			break;
		case 8 : 
			signal = 0x02FD10EF;
			break;
		case 9 : 
			signal = 0x02FD906F;
			break;
		case 10 : 
			signal = 0x02FD50AF;
			break;
		case 11 : 
			signal = 0x02FDD02F;
			break;
		case 12 : 
			signal = 0x02FD30CF;
			break;
		default :
			flag = 1;
			break;
	}	
	
	if(flag == 0)
	{		
                T1High(15);
                T1Low(7);

                int shift = 0x1;
                int onebit;
                int count;
                for(count=31;count>=0;count--)
                {
                        onebit = signal & (shift<<count);
                        onebit >>= count;
                        sendBit(onebit);
                }
                T1High(1);
	}
		
	/*if(c == '1')
	{
		int signal  = 0x02FDD827;
		int shift = 0x1;
		int onebit = 0;
		int count = 0;
		T1High(15);
                T1Low(7);
		for(count=31;count>=0;count--)
		{
			onebit = signal & (shift<<count);
			onebit >>= count;
			sendBit(onebit);
		}
		T1High(1);
	}*/
	//printk(KERN_INFO "receive %c\n",c);
        return 1; 
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xF0,0x9F,0x8D,0xA3,0x0A}; 
    	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
    		printk(KERN_ERR "sushi: copy_to_user failed.\n");
     		return -EFAULT;
    	}
    	size += sizeof(sushi);
    	return size;
}

static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .write = led_write,
	.read = sushi_read
};



static int __init init_mod(void)
{
	int retval;
	retval = alloc_chrdev_region(&dev,0,1,"myled");
	if(retval<0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

	cdev_init(&cdv, &led_fops);
        retval = cdev_add(&cdv, dev, 1);
        if(retval < 0){
                printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
                return retval;
        }
	
	cls = class_create(THIS_MODULE,"myled");
        if(IS_ERR(cls)){
                printk(KERN_ERR "class_create failed.");
                return PTR_ERR(cls);
        }
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));
	
	gpio_base = ioremap_nocache(0x3f200000, 0xA0);

	const u32 led = 25;
	const u32 index = led/10;  // GPFSEL2
 	const u32 shift = (led%10)*3;  // 15bit
    	const u32 mask = ~(0x7 << shift);  // 11111111111111000111111111111111
    	gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift); // 001: output flag

    	// 11111111111111001111111111111111

	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
    	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

/****************************************/
void T1High(int times)
{
	int j;
	for(j=0; j<times; j++)
	{
		int i;
        	for(i=0; i<23; i++)
		{ //23
			gpio_base[7] = 1 << 25;
			udelay(8);
        		gpio_base[10] = 1 << 25;
			udelay(18);
        	}        
    	}
}

void T1Low(int times){
    int j;
    for(j=0; j<times; j++)
    { //23
	int i;
        for(i=0; i<23; i++)
	{
            udelay(26);
        }
    }
}

void Bit1(void){
    T1High(1);
    T1Low(3);
}

void Bit0(void){
    T1High(1);
    T1Low(1);
}

void sendBit(int onebit)
{
	if(onebit==1)
	{
		T1High(1);
    		T1Low(3);
	}
	if(onebit==0)
	{
		T1High(1);
    		T1Low(1);
	}
}
/**************************************/

module_init(init_mod);
module_exit(cleanup_mod);

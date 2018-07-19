#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>

#include <asm/io.h>
#include <asm/uaccess.h>

static volatile unsigned int* gpio;

#define BCM_IO_BASE 		0x3F000000                       
#define GPIO_BASE           (BCM_IO_BASE + 0x200000)     
#define GPIO_SIZE           (256) 

#define GPIO_IN(g)  	(*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))) 
#define GPIO_OUT(g) 	(*(gpio+((g)/10)) |= (1<<(((g)%10)*3)))
#define GPIO_SET(g) 	(*(gpio+7) = 1<<g)
#define GPIO_CLR(g)		(*(gpio+10) = 1<<g)
#define GPIO_GET(g)		(*(gpio+13)&(1<<g))


#define LED_RATIO _IOW('c', 1, int)
#define BUZ_ON	  _IO('c',2)

/* Buzzer Device Driver */
//wiringPi : 21 BCM : 5
#define PIN 5
static DECLARE_WAIT_QUEUE_HEAD(my_wait);
//static int scale[8] = {262, 294, 330, 349, 392, 440, 494, 523};
static int numTones = 8;
// 음계 표준 주파수(4옥타브) :
static int tones[] = {349, 470, 392, 243, 349, 392, 470, 349};
static int freqs;

static struct task_struct *play_thread = NULL;
static struct work_struct my_work;

void softToneWrite(int pin, int freq)
{
	pin &= 63;

	if(freq < 0)
		freq = 0;
	else if(freq > 5000)
		freq = 5000;
	
	freqs = freq;
}

void my_buzzer_handler(void)
{
	//printk("my_buzzer_handler()\n");
	schedule_work(&my_work);
}

void my_work_handler(struct work_struct *work)
{
	int i;
	//printk("my_work_handler(%p)\n",work);
	wake_up_interruptible(&my_wait);
	for(i = 0; i < numTones ; ++i)
	{
		//printk("%3d\n",i);
		softToneWrite(PIN,tones[i]);
		if(i==3)
			mdelay(1200);
		else 
			mdelay(600);
	}
	freqs=0;
}

static int my_play_thread(void *data)
{
	int halfPeriod;
	//printk("my_play_thread start(%p)\n",data);

	do
	{
		wait_event_interruptible(my_wait, freqs!=0);
		if(freqs !=0 )
		{
			halfPeriod = 500000/freqs;
			gpio_set_value(PIN, 1);
			udelay(halfPeriod);
			gpio_set_value(PIN, 0);
			udelay(halfPeriod);
		}
		mdelay(1);
	}while(!kthread_should_stop());
	return 0;
}

static int led_gpios[] = {16, 19, 20, 21 };

static void update_led(int level)
{
	int i;
	if(level<0 || level>4)
	{
	//	printk("ERROR LED LEVEL!!!\n");
		return;
	}
	if(level == 0){
		for(i=0;i<4;i++)
    		GPIO_CLR(led_gpios[i]);
    	return ;
	}

	for(i=0;i<level;i++)
    	GPIO_SET(led_gpios[i]);
	
	for(i=level;i<4;i++)
    	GPIO_CLR(led_gpios[i]);
}

static long my_ioctl(struct file* filp, unsigned int arg, unsigned long opt)
{
	int ret, LED_level, size;
//	printk("my_ioclt()\n");
	if( _IOC_TYPE(arg) != 'c')
		return -EINVAL;
	size = _IOC_SIZE(arg);
	switch(arg)
	{
	case LED_RATIO:
		ret = copy_from_user(&LED_level, (void*)opt, size);
//		printk("cds = %d\n",LED_level);
		update_led(LED_level);
		break;
	case BUZ_ON:
//		printk("BUZ_ON\n");
		my_buzzer_handler();
		break;
	}
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = my_ioctl,
    .compat_ioctl = my_ioctl,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mydev",
    .fops = &fops,
};

static int my_init(void)
{	
	static void *map;
	int i;
	freqs = 0;
	
	printk("my_init()\n");
	map = ioremap(GPIO_BASE,GPIO_SIZE);
	gpio = (volatile unsigned int*)map;

	play_thread = kthread_run(my_play_thread, NULL, "my_play_thread");
	INIT_WORK(&my_work, my_work_handler);
	gpio_direction_output(PIN,0);

	misc_register(&misc);

	for(i=0;i<4;i++)
	{
		GPIO_IN(led_gpios[i]);
		GPIO_OUT(led_gpios[i]);
	}

	return 0;
}

static void my_exit(void)
{
	printk("my_exit()\n");
	freqs = 1;
	wake_up_interruptible(&my_wait);
	kthread_stop(play_thread);
		
	if(gpio)
		iounmap(gpio);


	misc_deregister(&misc);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");

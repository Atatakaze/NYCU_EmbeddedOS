# include <linux/init.h> //driver initialization entry points module_init()/module_exit()
# include <linux/module.h> //needed by any kernel modules
# include <linux/kernel.h> //printk()
# include <linux/fs.h> //file read(), write(), open()...
#include <linux/uaccess.h> //copy_from_user(), copy_to_user()
#include <linux/gpio.h>     //GPIO
#include <linux/delay.h>    //mdelay()

#define LED1 (21)
#define LED2 (20)
#define LED3 (16)
#define LED4 (12)
#define LED5 (1)
#define LED6 (7)
#define LED7 (8)
#define LED8 (25)
#define LED9 (24)

#define seg_a (26)
#define seg_b (19)
#define seg_c (13)
#define seg_d (6)
#define seg_e (5)
#define seg_f (0)
#define seg_g (11)
#define seg_dp (9)

/* LED(1-9) + 7-seg */
char buf_kernel[17];

static int __init hw1_driver_init ( void );
static void __exit hw1_driver_exit ( void );

/***************** Driver functions ******************/
static ssize_t hw1_driver_write ( struct file *fp , const char *buf , size_t count ,loff_t * fpos );
static int hw1_driver_open ( struct inode *inode , struct file *fp);
static int hw1_driver_release ( struct inode *inode , struct file *fp);
/*****************************************************/

static struct file_operations fops = {
    .owner      = THIS_MODULE, 
    .write      = hw1_driver_write,
    .open       = hw1_driver_open, 
    .release    = hw1_driver_release,
};

static int hw1_driver_open ( struct inode *inode , struct file *fp) {
    printk("Device Opened...!!!\n");
    return 0;
}

static int hw1_driver_release ( struct inode *inode , struct file *fp) {
    printk("Device Closed...!!!\n");
    return 0;
}

static ssize_t hw1_driver_write ( struct file *fp , const char *buf , size_t count ,loff_t * fpos ) {
    int ret, i, j, cases = 0, where;
    
    printk("Call driver write.\n");
    
    //copy from buf to buf_kernel
    ret = copy_from_user(&buf_kernel, buf, count); //suceed: ret=0; fail: ret=count
    if(ret){
        return -1;
    }
    
    printk("receive: ");
    for(i = 0; i < count; i++){
        printk(" %c", buf_kernel[i]);
    }
    printk("\n");

    /* control of LEDs */
    if(buf_kernel[0] == '1'){ gpio_set_value(LED1, 1); }
    else{ gpio_set_value(LED1, 0); }

    if(buf_kernel[1] == '1'){ gpio_set_value(LED2, 1); }
    else{ gpio_set_value(LED2, 0); }

    if(buf_kernel[2] == '1'){ gpio_set_value(LED3, 1); }
    else{ gpio_set_value(LED3, 0); }

    if(buf_kernel[3] == '1'){ gpio_set_value(LED4, 1); }
    else{ gpio_set_value(LED4, 0); }

    if(buf_kernel[4] == '1'){ gpio_set_value(LED5, 1); }
    else{ gpio_set_value(LED5, 0); }

    if(buf_kernel[5] == '1'){ gpio_set_value(LED6, 1); }
    else{ gpio_set_value(LED6, 0); }

    if(buf_kernel[6] == '1'){ gpio_set_value(LED7, 1); }
    else{ gpio_set_value(LED7, 0); }

    if(buf_kernel[7] == '1'){ gpio_set_value(LED8, 1); }
    else{ gpio_set_value(LED8, 0); }

    if(buf_kernel[8] == '1'){ gpio_set_value(LED9, 1); }
    else{ gpio_set_value(LED9, 0); }

    /* control of 7-seg */
    if(buf_kernel[9] == '1'){ gpio_set_value(seg_a, 1); }
    else{ gpio_set_value(seg_a, 0); }

    if(buf_kernel[10] == '1'){ gpio_set_value(seg_b, 1); }
    else{ gpio_set_value(seg_b, 0); }

    if(buf_kernel[11] == '1'){ gpio_set_value(seg_c, 1); }
    else{ gpio_set_value(seg_c, 0); }

    if(buf_kernel[12] == '1'){ gpio_set_value(seg_d, 1); }
    else{ gpio_set_value(seg_d, 0); }

    if(buf_kernel[13] == '1'){ gpio_set_value(seg_e, 1); }
    else{ gpio_set_value(seg_e, 0); }

    if(buf_kernel[14] == '1'){ gpio_set_value(seg_f, 1); }
    else{ gpio_set_value(seg_f, 0); }

    if(buf_kernel[15] == '1'){ gpio_set_value(seg_g, 1); }
    else{ gpio_set_value(seg_g, 0); }

    if(buf_kernel[16] == '1'){ gpio_set_value(seg_dp, 1); }
    else{ gpio_set_value(seg_dp, 0); }

    return count;
}

# define MAJOR_NUM 255
# define DRIVER_NAME "hw1_driver"

static int __init hw1_driver_init ( void ) {
    printk("Call driver init.\n");

    if( register_chrdev ( MAJOR_NUM , DRIVER_NAME , &fops ) < 0) {
        pr_info(" Can not get major %d\n", MAJOR_NUM );
        return (-EBUSY);
    }

    printk("My device is started and the major is %d\n", MAJOR_NUM );
    
    gpio_direction_output(LED1, 0);
    gpio_direction_output(LED2, 0);
    gpio_direction_output(LED3, 0);
    gpio_direction_output(LED4, 0);
    gpio_direction_output(LED5, 0);
    gpio_direction_output(LED6, 0);
    gpio_direction_output(LED7, 0);
    gpio_direction_output(LED8, 0);
    gpio_direction_output(LED9, 0);

    gpio_direction_output(seg_a, 0);
    gpio_direction_output(seg_b, 0);
    gpio_direction_output(seg_c, 0);
    gpio_direction_output(seg_d, 0);
    gpio_direction_output(seg_e, 0);
    gpio_direction_output(seg_f, 0);
    gpio_direction_output(seg_g, 0);
    gpio_direction_output(seg_dp, 0);

    return 0;
}

static void __exit hw1_driver_exit ( void ) {
    unregister_chrdev ( MAJOR_NUM ,  DRIVER_NAME );

    gpio_free(LED1);
    gpio_free(LED2);
    gpio_free(LED3);
    gpio_free(LED4);
    gpio_free(LED5);
    gpio_free(LED6);
    gpio_free(LED7);
    gpio_free(LED8);
    gpio_free(LED9);

    gpio_free(seg_a);
    gpio_free(seg_b);
    gpio_free(seg_c);
    gpio_free(seg_d);
    gpio_free(seg_e);
    gpio_free(seg_f);
    gpio_free(seg_g);
    gpio_free(seg_dp);

    printk("Call exit.\n");
}

module_init ( hw1_driver_init );
module_exit ( hw1_driver_exit );

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("bobo chen (bobo511326.ee10@nycu.edu.tw)");
MODULE_DESCRIPTION("hw1 driver");
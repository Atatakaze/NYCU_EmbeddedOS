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

/* which action, which area + how many */
char buf_kernel[4];

/* number of confirm cases in each area */
int area[9] = {0};
int total_num = 0;

static int seg_display[10][8] = {
    {1, 1, 1, 1, 1, 1, 0, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1, 0}, // 2
    {1, 1, 1, 1, 0, 0, 1, 0}, // 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4
    {1, 0, 1, 1, 0, 1, 1, 0}, // 5
    {1, 0, 1, 1, 1, 1, 1, 0}, // 6
    {1, 1, 1, 0, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8
    {1, 1, 1, 0, 0, 1, 1, 0}  // 9
};

static int __init hw1_driver_init ( void );
static void __exit hw1_driver_exit ( void );

/***************** Driver functions ******************/
static ssize_t hw1_driver_write ( struct file *fp , const char *buf , size_t count ,loff_t * fpos );
static int hw1_driver_open ( struct inode *inode , struct file *fp);
static int hw1_driver_release ( struct inode *inode , struct file *fp);
/*****************************************************/

void blink(char num, int state){
    switch(num){
            case '0':
                gpio_set_value(LED1, state);
                break;
            case '1':
                gpio_set_value(LED2, state);
                break;
            case '2':
                gpio_set_value(LED3, state);
                break;
            case '3':
                gpio_set_value(LED4, state);
                break;
            case '4':
                gpio_set_value(LED5, state);
                break;
            case '5':
                gpio_set_value(LED6, state);
                break;
            case '6':
                gpio_set_value(LED7, state);
                break;
            case '7':
                gpio_set_value(LED8, state);
                break;
            case '8':
                gpio_set_value(LED9, state);
                break;
        }
}

void show_number(int num){
    gpio_set_value(seg_a, 0);
    gpio_set_value(seg_b, 0);
    gpio_set_value(seg_c, 0);
    gpio_set_value(seg_d, 0);
    gpio_set_value(seg_e, 0);
    gpio_set_value(seg_f, 0);
    gpio_set_value(seg_g, 0);
    gpio_set_value(seg_dp, 0);

    switch(num){
            case 0:
                gpio_set_value(seg_a, seg_display[0][0]);
                gpio_set_value(seg_b, seg_display[0][1]);
                gpio_set_value(seg_c, seg_display[0][2]);
                gpio_set_value(seg_d, seg_display[0][3]);
                gpio_set_value(seg_e, seg_display[0][4]);
                gpio_set_value(seg_f, seg_display[0][5]);
                gpio_set_value(seg_g, seg_display[0][6]);
                gpio_set_value(seg_dp, seg_display[0][7]);
                break;
            case 1:
                gpio_set_value(seg_a, seg_display[1][0]);
                gpio_set_value(seg_b, seg_display[1][1]);
                gpio_set_value(seg_c, seg_display[1][2]);
                gpio_set_value(seg_d, seg_display[1][3]);
                gpio_set_value(seg_e, seg_display[1][4]);
                gpio_set_value(seg_f, seg_display[1][5]);
                gpio_set_value(seg_g, seg_display[1][6]);
                gpio_set_value(seg_dp, seg_display[1][7]);
                break;
            case 2:
                gpio_set_value(seg_a, seg_display[2][0]);
                gpio_set_value(seg_b, seg_display[2][1]);
                gpio_set_value(seg_c, seg_display[2][2]);
                gpio_set_value(seg_d, seg_display[2][3]);
                gpio_set_value(seg_e, seg_display[2][4]);
                gpio_set_value(seg_f, seg_display[2][5]);
                gpio_set_value(seg_g, seg_display[2][6]);
                gpio_set_value(seg_dp, seg_display[2][7]);
                break;
            case 3:
                gpio_set_value(seg_a, seg_display[3][0]);
                gpio_set_value(seg_b, seg_display[3][1]);
                gpio_set_value(seg_c, seg_display[3][2]);
                gpio_set_value(seg_d, seg_display[3][3]);
                gpio_set_value(seg_e, seg_display[3][4]);
                gpio_set_value(seg_f, seg_display[3][5]);
                gpio_set_value(seg_g, seg_display[3][6]);
                gpio_set_value(seg_dp, seg_display[3][7]);
                break;
            case 4:
                gpio_set_value(seg_a, seg_display[4][0]);
                gpio_set_value(seg_b, seg_display[4][1]);
                gpio_set_value(seg_c, seg_display[4][2]);
                gpio_set_value(seg_d, seg_display[4][3]);
                gpio_set_value(seg_e, seg_display[4][4]);
                gpio_set_value(seg_f, seg_display[4][5]);
                gpio_set_value(seg_g, seg_display[4][6]);
                gpio_set_value(seg_dp, seg_display[4][7]);
                break;
            case 5:
                gpio_set_value(seg_a, seg_display[5][0]);
                gpio_set_value(seg_b, seg_display[5][1]);
                gpio_set_value(seg_c, seg_display[5][2]);
                gpio_set_value(seg_d, seg_display[5][3]);
                gpio_set_value(seg_e, seg_display[5][4]);
                gpio_set_value(seg_f, seg_display[5][5]);
                gpio_set_value(seg_g, seg_display[5][6]);
                gpio_set_value(seg_dp, seg_display[5][7]);
                break;
            case 6:
                gpio_set_value(seg_a, seg_display[6][0]);
                gpio_set_value(seg_b, seg_display[6][1]);
                gpio_set_value(seg_c, seg_display[6][2]);
                gpio_set_value(seg_d, seg_display[6][3]);
                gpio_set_value(seg_e, seg_display[6][4]);
                gpio_set_value(seg_f, seg_display[6][5]);
                gpio_set_value(seg_g, seg_display[6][6]);
                gpio_set_value(seg_dp, seg_display[6][7]);
                break;
            case 7:
                gpio_set_value(seg_a, seg_display[7][0]);
                gpio_set_value(seg_b, seg_display[7][1]);
                gpio_set_value(seg_c, seg_display[7][2]);
                gpio_set_value(seg_d, seg_display[7][3]);
                gpio_set_value(seg_e, seg_display[7][4]);
                gpio_set_value(seg_f, seg_display[7][5]);
                gpio_set_value(seg_g, seg_display[7][6]);
                gpio_set_value(seg_dp, seg_display[7][7]);
                break;
            case 8:
                gpio_set_value(seg_a, seg_display[8][0]);
                gpio_set_value(seg_b, seg_display[8][1]);
                gpio_set_value(seg_c, seg_display[8][2]);
                gpio_set_value(seg_d, seg_display[8][3]);
                gpio_set_value(seg_e, seg_display[8][4]);
                gpio_set_value(seg_f, seg_display[8][5]);
                gpio_set_value(seg_g, seg_display[8][6]);
                gpio_set_value(seg_dp, seg_display[8][7]);
                break;
            case 9:
                gpio_set_value(seg_a, seg_display[9][0]);
                gpio_set_value(seg_b, seg_display[9][1]);
                gpio_set_value(seg_c, seg_display[9][2]);
                gpio_set_value(seg_d, seg_display[9][3]);
                gpio_set_value(seg_e, seg_display[9][4]);
                gpio_set_value(seg_f, seg_display[9][5]);
                gpio_set_value(seg_g, seg_display[9][6]);
                gpio_set_value(seg_dp, seg_display[9][7]);
                break;
        }

        mdelay(1000);
}

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

    /* action 1 entering confirm page (show total numebr of cases) */
    if(buf_kernel[0] == '1'){
        printk("\n > action 1\n");

        /* display total_num to 7-seg */
        if(total_num >= 10){
            show_number((int)(total_num / 10));
            show_number((int)(total_num % 10));
        }
        else{
            show_number(total_num);
        }
    }
    
    /* action 2 check specific area (show total number in 7-seg and blink LED) */
    if(buf_kernel[0] == '2'){
        printk("\n > action 2\n");

        /* display number of cases in area[ buf_kernel[1] ] to 7-seg */
        where = ((int)buf_kernel[1] - 48);
        if(area[where] >= 10){
            show_number((int)(area[where] / 10));
            show_number((int)(area[where] % 10));
        }
        else{
            show_number(area[where]);
        }

        /* blink LED in area[ buf_kernel[1] ] */
        blink(buf_kernel[1], 1);
        mdelay(500);
        blink(buf_kernel[1], 0);
        mdelay(500);
        blink(buf_kernel[1], 1);
        mdelay(500);
        blink(buf_kernel[1], 0);
        mdelay(500);
        blink(buf_kernel[1], 1);
        mdelay(500);
        blink(buf_kernel[1], 0);
        mdelay(500);
        blink(buf_kernel[1], 1);
    }

    /* action report page */
    if(buf_kernel[0] == '3'){
        printk("\n > action 3\n");

        /* update the number of confirmed cases */
        if(count = 3){
            cases = ((int)buf_kernel[2] - 48);
        }
        if(count = 4){
            cases = 10 * ((int)buf_kernel[2] - 48) + ((int)buf_kernel[3] - 48);
        }

        area[((int)buf_kernel[1] - 48)] += cases;
        total_num += cases;

        /* turn on the LED in area[ buf_kernel[1] ] */
        blink(buf_kernel[1], 1);

        /* display total_num to 7-seg */
        if(total_num >= 10){
            show_number((int)(total_num / 10));
            show_number((int)(total_num % 10));
        }
        else{
            show_number(total_num);
        }
    }

    /* action 4 exiting program (turn off all LED, 7-seg) */
    if(buf_kernel[0] == '4'){
        printk("\n > action 4\n");

        /* turn off 7-seg */
        gpio_set_value(seg_a, 0);
        gpio_set_value(seg_b, 0);
        gpio_set_value(seg_c, 0);
        gpio_set_value(seg_d, 0);
        gpio_set_value(seg_e, 0);
        gpio_set_value(seg_f, 0);
        gpio_set_value(seg_g, 0);
        gpio_set_value(seg_dp, 0);

        /* turn off LEDs */
        gpio_set_value(LED1, 0);
        gpio_set_value(LED2, 0);
        gpio_set_value(LED3, 0);
        gpio_set_value(LED4, 0);
        gpio_set_value(LED5, 0);
        gpio_set_value(LED6, 0);
        gpio_set_value(LED7, 0);
        gpio_set_value(LED8, 0);
        gpio_set_value(LED9, 0);
    }

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
    gpio_free(seg_pd);

    printk("Call exit.\n");
}

module_init ( hw1_driver_init );
module_exit ( hw1_driver_exit );

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("bobo chen (bobo511326.ee10@nycu.edu.tw)");
MODULE_DESCRIPTION("hw1 driver");
# include <linux/init.h> //driver initialization entry points module_init()/module_exit()
# include <linux/module.h> //needed by any kernel modules
# include <linux/kernel.h> //printk()
# include <linux/fs.h> //file read(), write(), open()...
#include <linux/uaccess.h> //copy_from_user(), copy_to_user()
MODULE_LICENSE ("GPL"); //GNU Public License v2 or later

//static: this function can not be used by other .c
static int seg_for_c[27] = {
    0b1111001100010001, // A
    0b0000011100000101, // b
    0b1100111100000000, // C
    0b0000011001000101, // d
    0b1000011100000001, // E
    0b1000001100000001, // F
    0b1001111100010000, // G
    0b0011001100010001, // H
    0b1100110001000100, // I
    0b1100010001000100, // J
    0b0000000001101100, // K
    0b0000111100000000, // L
    0b0011001110100000, // M
    0b0011001110001000, // N
    0b1111111100000000, // O
    0b1000001101000001, // P
    0b0111000001010000, //q
    0b1110001100011001, //R
    0b1101110100010001, //S
    0b1100000001000100, //T
    0b0011111100000000, //U
    0b0000001100100010, //V
    0b0011001100001010, //W
    0b0000000010101010, //X
    0b0000000010100100, //Y
    0b1100110000100010, //Z
    0b0000000000000000
};


char buf_kernel; //store the value written into the driver

// File Operations
static ssize_t my_read ( struct file *fp , char *buf , size_t count , loff_t *fpos ) {
    //printk (" call read \n");

    //change the char to 16seg bits by ASCII
    int index;

    if(buf_kernel<='Z' && buf_kernel>='A'){
        index=(int)(buf_kernel-'A');
    }
    else if(buf_kernel<='z' && buf_kernel>='a'){
        index=(int)(buf_kernel-'a');
    }
    else
    {
        index=26;
    }
    
    //change the type from int to char with 16 bits
    char seg[16];
    int i=0;
    for(;i<16;i++){
        seg[15-i]=(seg_for_c[index]>>i)&(0b0000000000000001);
    }
    
    //copy seg to buf
    int ret;
    ret=copy_to_user(buf, seg, 16); //suceed: ret=0; fail: ret=count
    if(ret){
        return -1;
    }
    
    return count;
}

static ssize_t my_write ( struct file *fp , const char *buf , size_t count ,loff_t * fpos ) {
    //printk (" call write \n");
    int ret;

    //copy from buf to buf_kernel
    ret=copy_from_user(&buf_kernel, buf, count); //suceed: ret=0; fail: ret=count
    if(ret){
        return -1;
    }
    
    //printk("%d\n", count);
    //printk("%c\n", buf_kernal);
    return count;
}

static int my_open ( struct inode *inode , struct file *fp) {
    printk (" call open \n");
    return 0;
}

struct file_operations my_fops = {
    read : my_read ,
    write : my_write ,
    open : my_open
};

# define MAJOR_NUM 244
# define DRIVER_NAME "my_dev"

static int my_init ( void ) {
    printk (" call init \n");
    if( register_chrdev ( MAJOR_NUM , DRIVER_NAME , &my_fops ) < 0) {
        printk (" Can not get major %d\n", MAJOR_NUM );
        return (-EBUSY);
    }

    printk ("My device is started and the major is %d\n", MAJOR_NUM );
    return 0;
}

static void my_exit ( void ) {
    unregister_chrdev ( MAJOR_NUM ,  DRIVER_NAME );
    printk (" call exit \n");
}


module_init ( my_init );
module_exit ( my_exit );
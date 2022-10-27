#include <linux/kernel.h>
#include <linux/fs.h>
#include <fcntl.h>  // open()
#include <unistd.h> //unix standard -> driver read(), write(), close()
#include <string.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    int f;
    unsigned int i;
    char data[10] = {};

    sprintf(data, "%s", argv[1]);
    if ((f = open("/dev/lab4_driver", O_RDWR)) < 0)
    {
        pr_info("Failure to open /dev/lab4_driver");
        return 0;
    }
    pr_info("the input name: %s\n", data);

    for (i=0; i < strlen(data); i++)
    {
        sleep(1);
        write(f, &data[i], 1);
    }

    close(f); /* 關檔 */
}
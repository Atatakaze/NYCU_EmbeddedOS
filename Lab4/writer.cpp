/*
使用者將想輸入的字串打入，然後會被放入/dev/mydev裡面
*/

#include <linux/kernel.h>
#include <linux/fs.h>
#include <fcntl.h>  // open()
#include <unistd.h> //unix standard -> driver read(), write(), close()
#include <string.h>
#include <cstdio>  // fprintf(), perror()
#include <cstdlib> // exit()

#include <stdio.h>

int main(int argc, char *argv[])
{
    int f;
    char data[10] = {};
    sprintf(data, "%s", argv[1]);
    //f = open("/dev/mydev", O_WRONLY|O_CREAT, 0700); /* 開個名叫hello的檔 */
    if ((f = open("/dev/mydev", O_RDWR)) < 0)
    {
        perror("/dev/mydev");
        exit(EXIT_FAILURE);
    }

    printf("the input name: %s\n", data);

    unsigned int i = 0;
    for (; i < strlen(data); i++)
    {
        sleep(1);
        write(f, &data[i], 1); /* 用write()把hello world寫進hello這個檔裡 */
        //printf("%s\n",data_3);
        //fflush();
    }

    close(f); /* 關檔 */
}
/*
 ******************************************************************************
 * @file           : hw1.c
 * @brief          : Embedded operating system HW#1
 ******************************************************************************
 */

#include <linux/kernel.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int f, total_num = 0;
char data[17] = {};

typedef struct n_case{
    // number of mild case in the area
    int mild;
    // number of severe case in the area
    int severe;
} n_case;

static char seg_display[10][8] = {
    {'1', '1', '1', '1', '1', '1', '0', '0'}, // 0
    {'0', '1', '1', '0', '0', '0', '0', '0'}, // 1
    {'1', '1', '0', '1', '1', '0', '1', '0'}, // 2
    {'1', '1', '1', '1', '0', '0', '1', '0'}, // 3
    {'0', '1', '1', '0', '0', '1', '1', '0'}, // 4
    {'1', '0', '1', '1', '0', '1', '1', '0'}, // 5
    {'1', '0', '1', '1', '1', '1', '1', '0'}, // 6
    {'1', '1', '1', '0', '0', '0', '0', '0'}, // 7
    {'1', '1', '1', '1', '1', '1', '1', '0'}, // 8
    {'1', '1', '1', '0', '0', '1', '1', '0'}  // 9
};

/* state 1 (menu) */
void menu_page(int *state)
{
    int input_n = 0;
    
    printf("\n==========================\n");
    printf("1. Confirmed case\n");
    printf("2. Reporting system\n");
    printf("3. Exit\n");

    scanf(" %d", &input_n);

    if( input_n == 1 || input_n == 2 || input_n == 3){
        *state = input_n;
    }

    else{
        printf(" > Wrong input @ menu page !!!\n");
        printf(" > input = %d\n", input_n);
    }
}

/* state 2 (query) */
void query_page(int *state, n_case *area)
{
    unsigned int i;
    int input_n;
    char input_c;
    
    printf("\n==========================\n");
    for(i = 0; i < 9; i++){
        printf("%d : %d\n", i, (area[i].mild + area[i].severe));
    }

    printf("(Press 'q' to leave query page or select one area to check (0-8).) ");
    scanf(" %c", &input_c);

    if(input_c == 'q'){
        *state = 0;
        //printf("Press 'q' @ query page.\n");
    }
    else{
        input_n = ((int)input_c - 48);
        //printf("input_c -> input_n : %c -> %d\n", input_c, input_n);

        if(input_n < 0 || input_n > 8){
            printf(" > Wrong input @ query page !!!\n");
            printf(" > input_n : %d\n", input_n);
        }
        else{
            printf("\n==========================\n");
            printf("Mild : %d\n", area[input_n].mild);
            printf("Severe : %d\n", area[input_n].severe);

            /* display number of cases in 7-seg */
            if((area[input_n].mild + area[input_n].severe) >= 100){
                for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(int)((area[input_n].mild + area[input_n].severe) / 100)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
		write(f, &data, 17);
                usleep(500000);
		for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(int)(((area[input_n].mild + area[input_n].severe) / 10) % 10)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
		write(f, &data, 17);
                usleep(500000);
                for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(int)((area[input_n].mild + area[input_n].severe) % 10)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
                write(f, &data, 17);
            }
	    else if((area[input_n].mild + area[input_n].severe) >= 10){
                for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(int)((area[input_n].mild + area[input_n].severe) / 10)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
		write(f, &data, 17);
                usleep(500000);
                for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(int)((area[input_n].mild + area[input_n].severe) % 10)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
                write(f, &data, 17);
            }
            else{
                for(i = 0; i < 8; i++){
                    data[9 + i] = seg_display[(area[input_n].mild + area[input_n].severe)][i];
                }
		/*
		printf("data: ");
		for(i = 0; i < 17; i ++){
		    printf("%c ", data[i]);
		}
		printf("\n");
		*/
		write(f, &data, 17);
            }

            /* blink LED */
            data[input_n] = '0';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            data[input_n] = '1';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            data[input_n] = '0';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            data[input_n] = '1';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            data[input_n] = '0';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            data[input_n] = '1';
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);

            printf("(Press any key to leave this page.) ");
            scanf(" %c", &input_c);
        }
    }
}

/* state 3 (report) */
void report_page(int *state, n_case *area)
{
    int input_n, number;
    char input_c;
    unsigned int i;

    printf("\n==========================\n");
    printf("Area (0~8) : ");
    scanf(" %d", &input_n);

    printf("Mild or Severe ('m' or 's') : ");
    scanf(" %c", &input_c);

    printf("The number of confirmed case : ");
    scanf(" %d", &number);

    if(input_c == 'm'){
        area[input_n].mild += number;
    }
    else if(input_c == 's'){
        area[input_n].severe += number;
    }
    else{
        printf(" > Wrong input @ report page !!!\n");
    }
    total_num += number;

    /* turn on LED in area[input_n] */
    data[input_n] = '1';
    /* display total cases in 7-seg */
    if(total_num >= 100){
        for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[(int)(total_num / 100)][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
        usleep(500000);
	for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[(int)((total_num / 10) % 10)][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
        usleep(500000);
        for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[(int)(total_num % 10)][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
    }
    else if(total_num >= 10){
        for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[(int)(total_num / 10)][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
        usleep(500000);
        for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[(int)(total_num % 10)][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
    }
    else{
        for(i = 0; i < 8; i++){
            data[9 + i] = seg_display[total_num][i];
        }
	/*
	printf("data: ");
	for(i = 0; i < 17; i ++){
	    printf("%c ", data[i]);
	}
	printf("\n");
	*/
        write(f, &data, 17);
    }


    printf("(Press 'e' to leave this page or 'c' to report another case.) ");
    scanf(" %c", &input_c);
    if(input_c == 'e'){
        *state = 0;
    }
}

int main()
{
    /* menu state
     * 0: (Default) Menu.
     * 1: Show the number of confirmed cases in each area
     * 2: Report new cases
     * 3: Exit
     */
    int state = 0;

    // 9 area
    unsigned int i;
    n_case area[9];
    for(i = 0; i < 9; i++){
        area[i].mild = 0;
        area[i].severe = 0;
    }

    /* initial data */
    for(i = 0; i < 17; i++){
        data[i] = '0';
    }

    if ((f = open("/dev/etx_device", O_RDWR)) < 0){
        perror("/dev/etx_device");
        exit(EXIT_FAILURE);
    }

    while(1){
        /* display LED */
        for(i = 0; i < 9; i++){
            if(area[i].mild + area[i].severe != 0){
                data[i] = '1';
            }
            else{
                data[i] = '0';
            }
        }
        /* display total cases in 7-seg */
        if(total_num >= 100){
            for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[(int)(total_num / 100)][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
	    for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[(int)((total_num / 10) % 10)][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[(int)(total_num % 10)][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
        }
	else if(total_num >= 10){
            for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[(int)(total_num / 10)][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
            usleep(500000);
            for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[(int)(total_num % 10)][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
        }
        else{
            for(i = 0; i < 8; i++){
                data[9 + i] = seg_display[total_num][i];
            }
	    /*
	    printf("data: ");
	    for(i = 0; i < 17; i ++){
		printf("%c ", data[i]);
	    }
	    printf("\n");
	    */
            write(f, &data, 17);
        }

        /* (Default) Show menu page */
        if(state == 0){
            menu_page(&state);
        }
        /* Show the number of confirm cases in each area */
        else if (state == 1){
            query_page(&state, area);
        }
        /* Report new case */
        else if (state == 2)
        {
            report_page(&state, area);
        }
        /* Exit program */
        else{
            printf("Press 3 -> exiting program ...\n");
            break;
        }
    }

    /* Clean up and exit program (turn off all LEDs and 7-seg) */
    for(i = 0; i < 17; i++){
        data[i] = '0';
    }
    /*
    printf("data: ");
    for(i = 0; i < 17; i ++){
	printf("%c ", data[i]);
    }
    printf("\n");
    */
    write(f, &data, 17);

    close(f);
    return 0;
}

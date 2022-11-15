/*
 ******************************************************************************
 * @file           : hw1.c
 * @brief          : Embedded operating system HW#1
 ******************************************************************************
 */

# include <stdio.h>

typedef struct n_case{
    // number of mild case in the area
    int mild;
    // number of severe case in the area
    int severe;
} n_case;

/* state 1 (menu) */
void menu_page(int *state)
{
    int input_n = 0;
    
    printf("==========================\n");
    printf("1. Confirmed case\n");
    printf("2. Reporting system\n");
    printf("3. Exit\n");

    scanf("%d", &input_n);

    if( input_n == 1 || input_n == 2 || input_n == 3){
        *state = input_n;
    }

    else{
        printf("==========================\n");
        printf("Wrong input !!!\n");
    }
}

/* state 2 (query) */
void query_page(int *state, n_case *area)
{
    unsigned int i;
    int input_n;
    char input_c;
    
    
    printf("==========================\n");
    for(i = 0; i < 9; i++)
        printf("%d : %d\n", i, (area[i].mild + area[i].severe));

    scanf("%c", &input_c);

    if(input_c == 'q'){
        *state = 0;
    }
    else{
        input_n = (int)input_c;

        if(input_n < 0 || input_n > 8){
            printf("==========================\n");
            printf("Wrong input !!!\n");
        }
        else{
            printf("==========================\n");
            printf("Mild : %d\n", area[input_n].mild);
            printf("Severe : %d\n", area[input_n].severe);
        }
    }
}

/* state 3 (report) */
void report_page(int *state, n_case *area)
{
    int input_n, number;
    char input_c;
    
    printf("==========================\n");
    printf("Area (0~8) : ");
    scanf("%d", &input_n);

    printf("Mild or Severe ('m' or 's') : ");
    scanf("%c", &input_c);

    printf("The number of confirmed case : ");
    scanf("%d", &number);

    if(input_c == 'm'){
        area[input_n].mild += number;
    }
    else if(input_c == 's'){
        area[input_n].severe += number;
    }
    else{
        printf("==========================\n");
        printf("Wrong input !!!\n");
    }

    scanf("%c", &input_c);
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

    while(1){
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
            break;
        }
    }

    /* Clean up and exit program (turn off all LEDs and 7-seg) */
    return 0;
}
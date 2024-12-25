#include "headers/user_input.h"

int get_user_menu_choice(void)
{
    int user_choice = 0;
    char current_char = 0;

    while(1)
    {
        current_char = getc(stdin);

        if (current_char == EOF || current_char == '\r' || current_char == '\n') break;

        user_choice = current_char - '0';

        while(getc(stdin) != '\n' || feof(stdin));

        if (user_choice >= 0 && user_choice <= 4)
        {
            break;
        }
        else {
            printf("Error: invalid input!\n");
            printf("-> ");
        }
    }

    return user_choice;
}

void get_user_title(char *input_buffer)
{
    char current_char = 0;

    int loop_index = 0;

    printf("Enter title: ");

    while(1)
    {
        current_char = getc(stdin);

        if (current_char == EOF || current_char == '\n' || current_char == '\r') break;

        if (loop_index < TITLE_STORE_LIMIT)
        {
            input_buffer[loop_index] = current_char;
            loop_index += 1;
        }
    }
}

int get_user_title_choice(int *last_entry_index)
{
    int user_choice = 0;
    char current_char = 0;

    while(1)
    {
        printf("-> ");
        while(1)
        {
            current_char = getc(stdin);

            if(current_char == EOF || current_char == '\r' || current_char == '\n') break;

            user_choice *= 10;
            user_choice += current_char - '0';
        }

        if (user_choice > 0 && user_choice <= *last_entry_index) break;

        printf("Invalid input!\n");
        user_choice = 0;
    }

    return user_choice;
}

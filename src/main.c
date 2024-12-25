#include "headers/console.h"
#include "headers/user_input.h"
#include "headers/file.h"

int main(void)
{
    char continue_execution = 1;
    char title_buffer[TITLE_STORE_LIMIT] = {0};
    int entries_found = 0;
    int user_title_chosen_to_delete = 0;

    while (continue_execution)
    {
        print_menu_options();
        int user_menu_choice = get_user_menu_choice();

        switch (user_menu_choice)
        {
            case 1:
                print_watch_file();
                break;
            case 2:
                get_user_title(title_buffer);
                add_to_watch_file(title_buffer);
                break;
            case 3:
                entries_found = print_watch_file();
                user_title_chosen_to_delete = get_user_title_choice(&entries_found);
                remove_from_watch_file(user_title_chosen_to_delete);
                break;
            case 4:
                break;
            case 0:
                continue_execution = 0;
        }
    }
    return 0;
}

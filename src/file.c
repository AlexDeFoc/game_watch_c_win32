#include "headers/file.h"

static void validate_file_handle(HANDLE *watch_file)
{
    if (*watch_file == INVALID_HANDLE_VALUE)
    {
        printf("Error: cannot open watch file!\n");

        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            printf("Reason: file doesn't exist!\n");
            printf("Solution: auto-generating file...\n");

            *watch_file = CreateFileA(WATCH_FILE_PATH, FILE_READ_DATA, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

            if (*watch_file == INVALID_HANDLE_VALUE)
            {
                printf("Fatal error: couldn't generate watch file!\n");
                printf("Error code: %lu\n", GetLastError());
                printf("Press any key to exit...");
                getc(stdin);
                exit(1);
            }

            printf("Done: auto-generating watch file!\n");
        }
        else
        {
            printf("Fatal reason: couldn't identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            printf("Press any key to exit...");
            getc(stdin);
            exit(1);
        }
    }
}

static void get_game_entry(HANDLE *watch_file, struct GameEntry* return_game_entry, char *reached_end_of_file)
{
    struct GameEntry game_entry = {
        .title = {0},
        .time = {0}
    };

    char current_char = 0;
    DWORD bytes_read = 0;

    char reading_time_member = 0;

    int title_index = 0;
    int time_index = 0;

    while (1) {
        if (ReadFile(*watch_file, &current_char, 1, &bytes_read, NULL) == FALSE)
        {
            CloseHandle(*watch_file);
            printf("Fatal error: cannot read watch file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }

        if (bytes_read == 0)
        {
            *reached_end_of_file = 1;
            break;
        }

        switch (current_char)
        {
            case '\n':
                memcpy(return_game_entry->title, game_entry.title, TITLE_READ_LIMIT);
                memcpy(return_game_entry->time, game_entry.time, TIME_READ_LIMIT);
                return;

            case ',':
                reading_time_member = 1;
                continue;

            case '\r':
                continue;

            case ' ':
                if (reading_time_member == 1) continue;
        }

        if (reading_time_member == 0 && title_index < TITLE_READ_LIMIT)
        {
            game_entry.title[title_index] = current_char;
            title_index += 1;
        }
        else if (reading_time_member == 1 && time_index < TIME_READ_LIMIT)
        {
            game_entry.time[time_index] = current_char;
            time_index += 1;
        }
    }
}

int print_watch_file(void)
{
    char reached_eof = 0;
    struct GameEntry game_entry = {0};

    HANDLE watch_file = CreateFileA(WATCH_FILE_PATH, FILE_READ_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    validate_file_handle(&watch_file);

    int entries_found = 0;

    if (SetFilePointer(watch_file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        printf("Fatal error: cannot move file pointer to start of watch file!\n");
        printf("Fatal reason: cannot identify reason!\n");
        printf("Error code: %lu\n", GetLastError());
        printf("Press any key to exit...");
        getc(stdin);
        exit(1);
    }

    while (1)
    {
        get_game_entry(&watch_file, &game_entry, &reached_eof);

        if (reached_eof == 1)
        {
            CloseHandle(watch_file);
            break;
        }

        printf("'%s' - %s\n", game_entry.title, game_entry.time);
        entries_found += 1;
    }

    return entries_found;
}

void add_to_watch_file(char *title)
{
    HANDLE watch_file = CreateFileA(WATCH_FILE_PATH, FILE_WRITE_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    validate_file_handle(&watch_file);

    if (SetFilePointer(watch_file, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
    {
        printf("Fatal error: cannot move file pointer to end of watch file!\n");
        printf("Fatal reason: cannot identify reason!\n");
        printf("Error code: %lu\n", GetLastError());
        printf("Press any key to exit...");
        getc(stdin);
        exit(1);
    }

    if (WriteFile(watch_file, title, strlen(title), NULL, NULL) == FALSE)
    {
        CloseHandle(watch_file);
        printf("Fatal error: cannot write title to watch file!\n");
        printf("Fatal reason: cannot identify reason!\n");
        printf("Error code: %lu\n", GetLastError());
        getc(stdin);
        exit(1);
    }

    const char space_char = ' ';

    WriteFile(watch_file, ",", 1, NULL, NULL);
    WriteFile(watch_file, &space_char, 1, NULL, NULL);
    WriteFile(watch_file, "0", 1, NULL, NULL);
    WriteFile(watch_file, "\r\n", 2, NULL, NULL);

    memset(title, 0, TITLE_READ_LIMIT);

    CloseHandle(watch_file);
}

void remove_from_watch_file(int remove_index)
{
    HANDLE watch_file = CreateFileA(WATCH_FILE_PATH, FILE_READ_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE temp_file = CreateFileA(TEMP_FILE_PATH, FILE_WRITE_DATA, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    validate_file_handle(&watch_file);

    if (SetFilePointer(watch_file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        printf("Fatal error: cannot move file pointer to start of watch file!\n");
        printf("Fatal reason: cannot identify reason!\n");
        printf("Error code: %lu\n", GetLastError());
        printf("Press any key to exit...");
        getc(stdin);
        exit(1);
    }

    int current_entry_index = 1;
    char current_char = 0;
    DWORD bytes_read = 0;

    while (1)
    {
        if (ReadFile(watch_file, &current_char, 1, &bytes_read, NULL) == FALSE)
        {
            CloseHandle(watch_file);
            printf("Fatal error: cannot read watch file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }

        if (WriteFile(temp_file, &current_char, 1, NULL, NULL) == FALSE)
        {
            CloseHandle(temp_file);
            printf("Fatal error: cannot write to temp file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }

        if (current_char == '\n') current_entry_index += 1;

        if (current_entry_index >= remove_index) break;
    }

    // Skip over the line to be removed
    while (1)
    {
        if (ReadFile(watch_file, &current_char, 1, &bytes_read, NULL) == FALSE)
        {
            CloseHandle(watch_file);
            printf("Fatal error: cannot read watch file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }

        if (current_char == '\n') break;
    }

    // Copy the remaining content
    while (1)
    {
        if (ReadFile(watch_file, &current_char, 1, &bytes_read, NULL) == FALSE)
        {
            CloseHandle(watch_file);
            printf("Fatal error: cannot read watch file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }

        if (bytes_read == 0) break;

        if (WriteFile(temp_file, &current_char, 1, NULL, NULL) == FALSE)
        {
            CloseHandle(temp_file);
            printf("Fatal error: cannot write to temp file!\n");
            printf("Fatal reason: cannot identify reason!\n");
            printf("Error code: %lu\n", GetLastError());
            getc(stdin);
            exit(1);
        }
    }

    CloseHandle(watch_file);
    CloseHandle(temp_file);

    // Replace the original file with the temp file
    if (!DeleteFileA(WATCH_FILE_PATH))
    {
        printf("Fatal error: failed to delete original watch file!\n");
        printf("Error code: %lu\n", GetLastError());
        getc(stdin);
        exit(1);
    }

    if (!MoveFileA(TEMP_FILE_PATH, WATCH_FILE_PATH))
    {
        printf("Fatal error: failed to rename temp file to watch file!\n");
        printf("Error code: %lu\n", GetLastError());
        getc(stdin);
        exit(1);
    }
}

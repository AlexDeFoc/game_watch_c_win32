#include <windows.h>
#include <stdio.h>

#define WATCH_FILE_PATH "./watch.txt"
#define TEMP_FILE_PATH "./temp.txt"

#define TITLE_READ_LIMIT 128
#define TIME_READ_LIMIT 32

struct GameEntry {
    char title[TITLE_READ_LIMIT];
    char time[TIME_READ_LIMIT];
};

int print_watch_file(void);
void add_to_watch_file(char *title);
void remove_from_watch_file(int remove_index);

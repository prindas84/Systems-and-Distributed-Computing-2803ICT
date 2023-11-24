#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DIRECTORY_SIZE 1024
#define COMMAND_SIZE 512
#define LINE_SIZE 512
#define INPUT_SIZE 128
#define TIME_SIZE 64

// General Functions.
int argument_count(char* user_input);
void clear_input_buffer();
void make_directory(char* directory_name, int permissions);
void copy_file(char* file_name, char* directory_name);
void process_command(char* user_input);
void remove_directory(char* directory_name);
char** split_string(char* user_input, int word_count);

// Command Functions.
void command_calc(char** split_input, int word_count);
void command_get(char* file_name);
void command_path();
void command_put(char** split_input, int word_count, bool force);
void command_sys();
void command_time();
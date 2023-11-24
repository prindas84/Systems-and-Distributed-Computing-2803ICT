#include "include.h"

void command_sys()
{
    // Declare the operating system info in a struct utsname and a NULL pointer to store the cpu_version.
    struct utsname system_info;

    // PROCESS THE OS INFORMATION.
    // Initialise the system information and confirm correct return value, as per documentation.
    if (uname(&system_info) == -1)
    {
        printf("ERROR: COULD NOT DETECT THE CURRENT OPERATING SYSTEM\n");
        return;
    }

    // Assign the variables using the struct methods.
    char* os_name = system_info.sysname;
    char* os_version = system_info.version;

    // Print the OS information.
    printf("OS: %s %s\n", os_name, os_version);

    // PROCESS THE CPU INFORMATION.
    // Open the virtual file containing the CPU information.
    FILE *input_file = fopen("/proc/cpuinfo", "r");
    if (input_file == NULL)
    {      
        printf("ERROR: COULD NOT DETECT THE CPU TYPE\n");
        return;
    }

    // Declare a buffer to read each line of the file into.
    char str[LINE_SIZE];

    // Read each line of the file.
    while((fgets(str, LINE_SIZE, input_file)) != NULL)
    {
        // Compare the first 10 characters of each line of the file to the string "model name". 
        // If it is a match, process the line.
        if (strncmp(str, "model name", 10) == 0)
        {
            // Split the line at the : and user the remaining cpu version description.
            char* cpu_version = strchr(str, ':');

            // If there is no error, remove the first two characters of the string ": " and quit the loop.
            if (cpu_version != NULL)
            {
                cpu_version += 2;
                // Print the CPU information.
                printf("CPU: %s", cpu_version);
                break;
            }
        }
    }
    return;
}



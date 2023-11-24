#include "include.h"

void command_time()
{
    // Initialise the variables for use.
    time_t world_time;
    struct tm *local_time;
    char timestamp[TIME_SIZE];

    // Get the current world time as an integer of seconds passed 1-1-1970.
    time(&world_time);
    
    // Convert the world time integer into local timezone and readable form using the local_time struct.
    local_time = localtime(&world_time);

    // Format the time and date as a string using the function definitions.
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %I:%M:%S%p", local_time);
    
    // Print the local time and date
    printf("%s \n", timestamp);
    
    return;
}


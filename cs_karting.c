// Assignment 2 26T1 COMP1511: CS Karting
// cs_karting.c
//
// Written by <YOUR-NAME>, YOUR-ZID z5555555
// on <TODAYS-DATE>
//
// DESCRIPTION OF YOUR PROGRAM HERE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cs_karting.h"

// Stage 1.1
// Function to create the Racing Series
// Params:
//      name - the name of the series
// Returns: a pointer to the series
struct series *create_series(char name[MAX_SIZE]) {

    // TODO: Implement this function
    printf("create_series() not implemented yet!\n");

    return NULL;
}

// Stage 1.1
// Function to create a race
// Params:
//      name - the name of the race
//      location - the location of the race
// Returns: a pointer to the race
struct race *create_race(char name[MAX_SIZE], enum location location) {

    // TODO: Implement this function
    printf("create_race() not implemented yet!\n");

    return NULL;
}

// Stage 1.1
// Function to create a track piece
// Params:
//      type - the type of track piece
// Returns: a pointer to the track piece
struct track_piece *create_track(enum piece_type type) {

    // TODO: Implement this function
    printf("create_track() not implemented yet!\n");

    return NULL;
}

// Stage 1.1
// Function to create a driver
// Params:
//      name - the name of the driver
//      speed - the speed of the driver
// Returns: a pointer to the driver
struct driver *create_driver(char name[MAX_SIZE], int speed) {

    // TODO: Implement this function
    printf("create_driver() not implemented yet!\n");

    return NULL;
}

// Stage 1.2
// Function to run the main command loop for the program
// Params:
//      series - a pointer to the racing series
// Returns: None
void command_loop(struct series *series) {
    
    // TODO: Implement this function
    printf("command_loop() not yet implemented!\n");
    
    return;
}

// Stage 1.3
// Function to add a race
// Params:
//      series - a pointer to the racing series
// Returns: None
void add_race(struct series *series) {

    // TODO: Implement this function
    printf("add_race() not yet implemented!\n");
    
    return;
}

// Stage 1.3
// Function to add a track piece
// Params:
//      series - a pointer to the racing series
// Returns: None
void add_track_piece(struct series *series) {

    // TODO: Implement this function
    printf("add_track_piece() not yet implemented!\n");
    
    return;
}

// Stage 1.3
// Function to add a driver
// Params:
//      series - a pointer to the racing series
// Returns: None
void add_driver(struct series *series) {
 
    // TODO: Implement this function
    printf("add_driver() not yet implemented!\n");
    
    return;
}

// Stage 1.4
// Function to print out the racing series
// Params:
//      series - a pointer to the racing series
// Retruns: None
void print_series(struct series *series) {

    // TODO: Implement this function
    printf("print_series() not yet implemented!\n");
    
    return;
}


// Add your function definitions below this line


///////////////////////////////////////////////////////////////////////////////
// Provided function definitions
////////////////////////////////////////////////////////////////////////////////

// Function to print the program usage information
// Params: None
// Returns: None
void print_usage() {

    printf(
       "======================[ Usage Info ]=====================\n"
       "                                                         \n"
       "  ?                                                      \n"
       "    Show this help information.                          \n"
       "  a r [race_name] [location]                             \n"
       "    Add a new race with the given name and location.     \n"
       "  a t [race_name] [piece_type]                           \n"
       "    Add a track piece of the given type to the race.     \n"
       "  a d [race_name] [driver_name] [speed]                  \n"
       "    Add a driver to the specified race.                  \n"
       "  *                                                      \n"
       "    Print the racing series.                             \n"
       "  i t [n] [race_name] [piece_type]                       \n"
       "    Insert a track piece at position [n] in the race.    \n"
       "  i d [n] [race_name] [driver_name] [speed]              \n"
       "    Insert a driver at [position] in the specified race. \n"
       "  c [existing_race] [new_race]                           \n"
       "    Adds a new race with the same track layout.          \n"
       "  s [race_name]                                          \n"
       "    Start the specified race.                            \n"
       "  v [race_name]                                          \n"
       "    Validates the specified rules meets track compliance.\n"
       "  m [driver_name] [current_race] [new_race]              \n"
       "    Moves the driver from one race to another.           \n"
       "  q                                                      \n"
       "    Cancels the karting series.                          \n"
       "  r [driver_name]                                        \n"
       "    Removes the driver from all races.                   \n"
       "  t [race_name] [turns]                                  \n"
       "    Advance the race by [turns].                         \n"
       "  e [race_name]                                          \n"
       "    Automatically run turns until the race finishes.     \n"
       "  f [location]                                           \n"
       "    Create a finals race for the given location.         \n"
       "                                                         \n"
       "=========================================================\n"
    );

}

// Function to print the welcome banner
// Params: None
// Returns: None
void print_welcome_banner() {

    printf(""
    "          Welcome to the CS Karting Racing Series!\n"
    "                  .\n"
    "                  |\\\n"
    "                  |_\\\n"
    "                  |\n"
    "       .==========||=======.    .===================.\n"
    "      /    _______||____    \\  /   _[_0|_[?]_______   \\\n"
    "     /   /'              '\\  \\/  /'               '\\   \\\n"
    "    |   |                  \\/   /                   |   |\n"
    "    |   |                  /   / \\                  |   |\n"
    "     \\  \\                 /   /\\  \\                /   /\n"
    "      \\  '---------------'   /  \\  '--------------'   /\n"
    "       \\.===[_0|====[_0|===./    \\.========|0_]=====./\n"
    "\n"
    );

}

// Helper function to print out the track information
// PARAMS:
//      track - the first track piece in a given race
// RETURNS: None
void print_track(struct track_piece *track) {

    if (track == NULL) {
        printf("        Empty Track!\n");
        return;
    }
    
    while (track->next != NULL) {
        printf("        %s\n", type_to_string(track->type));
        printf("        |\n");
        printf("        ⌄\n");
        track = track->next;
    }
    printf("        %s\n", type_to_string(track->type));
    
}

// Helper function to print out the drivers within a race
// PARAMS:
//      race - the race containing the drivers to be printed
// RETURNS: None
void print_drivers(struct race *race) {

    printf("        Drivers:\n");

    struct driver *current = race->drivers;
    if (current == NULL) {
        printf("            Empty Driver List!\n");
    }

    int count = 1;
    while (current != NULL) {
        printf("            %d. %s (speed: %d)\n", 
               count, current->name, current->speed);
        count++;
        current = current->next;
    }

}

// Scan in the a name string into the provided buffer, placing
// '\0' at the end.
//
// Params:
//      name - a char array of length MAX_SIZE, which will be used
//                  to store the name.
// Returns: None
// Usage:
// ```
//      char name[MAX_SIZE];
//      scan_name(name);
// ```
void scan_name(char name[MAX_SIZE]) {
    scan_token(name, MAX_SIZE);
}

// Scans a string and converts it to a location.
//
// Returns:
//      The corresponding location, if the string was valid,
//      Otherwise, returns INVALID.
//
// Usage:
// ```
//      enum location location = scan_location();
// ```
//
enum location scan_location(void) {
    char type[MAX_SIZE];
    scan_token(type, MAX_SIZE);
    return string_to_location(type);
}

// Scans a string and converts it to a location.
//
// Returns:
//      The corresponding pieve_type, if the string was valid,
//      Otherwise, returns INVALID.
//
// Usage:
// ```
//      enum piece_type type = scan_location();
// ```
//
enum piece_type scan_piece_type(void) {
    char type[MAX_SIZE];
    scan_token(type, MAX_SIZE);
    return string_to_type(type);
}

////////////////////////////////////////////////////////////////////////////////
// Additional provided function definitions
////////////////////////////////////////////////////////////////////////////////

// You don't need to use any of these, or understand how they work!
// We use them to implement some of the provided helper functions.

enum points place_to_points(int place) {
    if (place == 1) {
        return FIRST;
    }
    if (place == 2) {
        return SECOND;
    }
    if (place == 3) {
        return THIRD;
    }
    if (place == 4) {
        return FOURTH;
    }
    if (place == 5) {
        return FIFTH;
    }
    if (place == 6) {
        return SIXTH;
    }
    if (place == 7) {
        return SEVENTH;
    } else {
        return EIGHTH;
    }
}

enum location string_to_location(char *type_str) {
    int len = strlen(type_str);

    if (strncasecmp(type_str, "ALBERT_PARK", len) == 0) {
        return ALBERT_PARK;
    }
    if (strncasecmp(type_str, "MARINA_BAY", len) == 0) {
        return MARINA_BAY;
    }
    if (strncasecmp(type_str, "MONZA", len) == 0) {
        return MONZA;
    }
    if (strncasecmp(type_str, "SUZUKA", len) == 0) {
        return SUZUKA;
    } else {
        return ZANDVOORT;
    }
}

char *location_to_string(enum location location) {

    if (location == ALBERT_PARK) {
        return "ALBERT_PARK";
    }
    if (location == MARINA_BAY) {
        return "MARINA_BAY";
    }
    if (location == MONZA) {
        return "MONZA";
    }
    if (location == SUZUKA) {
        return "SUZUKA";
    } else  {
        return "ZANDVOORT";
    }

}
enum piece_type string_to_type(char *type_str) {
    int len = strlen(type_str);

    if (strncasecmp(type_str, "START_LINE", len) == 0) {
        return START_LINE;
    }
    if (strncasecmp(type_str, "FINISH_LINE", len) == 0) {
        return FINISH_LINE;
    }
    if (strncasecmp(type_str, "STRAIGHT", len) == 0) {
        return STRAIGHT;
    }
    if (strncasecmp(type_str, "LEFT_TURN", len) == 0) {
        return LEFT_TURN;
    }
    if (strncasecmp(type_str, "RIGHT_TURN", len) == 0) {
        return RIGHT_TURN;
    }
    if (strncasecmp(type_str, "JUMP", len) == 0) {
        return JUMP;
    }
    if (strncasecmp(type_str, "MYSTERY_BOX", len) == 0) {
        return MYSTERY_BOX;
    }

    return INVALID;
}

char *type_to_string(enum piece_type type) {
    if (type == START_LINE) {
        return "START_LINE";
    }
    if (type == FINISH_LINE) {
        return "FINISH_LINE";
    }
    if (type == STRAIGHT) {
        return "STRAIGHT";
    }
    if (type == LEFT_TURN) {
        return "LEFT_TURN";
    }
    if (type == RIGHT_TURN) {
        return "RIGHT_TURN";
    }
    if (type == JUMP) {
        return "JUMP";
    }
    if (type == MYSTERY_BOX) {
        return "MYSTERY_BOX";
    }
    return "INVALID";
}

char *status_to_string(enum race_status status) {
    
    if (status == NOT_STARTED) {
        return "NOT_STARTED";
    } else if (status == ACTIVE) {
        return "ACTIVE";
    } else {
        return "COMPLETED";
    }

}

int scan_token(char *buffer, int buffer_size) {
    if (buffer_size == 0) {
        return 0;
    }

    char c;
    int i = 0;
    int num_scanned = 0;

    scanf(" ");

    while (i < buffer_size - 1 && (num_scanned = scanf("%c", &c)) == 1 &&
           !isspace(c)) {
        buffer[i++] = c;
    }

    if (i > 0) {
        buffer[i] = '\0';
    }

    return num_scanned;
}

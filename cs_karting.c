// Assignment 2 26T1 COMP1511: CS Karting
// cs_karting.c
//
// Written by <YOUR-NAME>, YOUR-ZID z5555555
// on <TODAYS-DATE>
//
// CS Karting racing series management system core implementation
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cs_karting.h"

///////////////////////////////////////////////////////////////////////////////
// Stage 1.1 - Core Struct Creation Functions
///////////////////////////////////////////////////////////////////////////////
// Function to create the Racing Series
struct series *create_series(char name[MAX_SIZE]) {
    struct series *new_series = malloc(sizeof(struct series));
    if (new_series == NULL) {
        perror("malloc failed for series");
        exit(EXIT_FAILURE);
    }
    strcpy(new_series->name, name);
    new_series->races = NULL;
    return new_series;
}

// Function to create a race
struct race *create_race(char name[MAX_SIZE], enum location location) {
    struct race *new_race = malloc(sizeof(struct race));
    if (new_race == NULL) {
        perror("malloc failed for race");
        exit(EXIT_FAILURE);
    }
    strcpy(new_race->name, name);
    new_race->location = location;
    new_race->status = NOT_STARTED;
    new_race->track = NULL;
    new_race->drivers = NULL;
    new_race->next = NULL;
    return new_race;
}

// Function to create a track piece
struct track_piece *create_track(enum piece_type type) {
    struct track_piece *new_track = malloc(sizeof(struct track_piece));
    if (new_track == NULL) {
        perror("malloc failed for track piece");
        exit(EXIT_FAILURE);
    }
    new_track->type = type;
    new_track->drivers = NULL;
    new_track->next = NULL;
    return new_track;
}

// Function to create a driver
struct driver *create_driver(char name[MAX_SIZE], int speed) {
    struct driver *new_driver = malloc(sizeof(struct driver));
    if (new_driver == NULL) {
        perror("malloc failed for driver");
        exit(EXIT_FAILURE);
    }
    strcpy(new_driver->name, name);
    new_driver->speed = speed;
    new_driver->next = NULL;
    return new_driver;
}

///////////////////////////////////////////////////////////////////////////////
// Core Helper Functions
///////////////////////////////////////////////////////////////////////////////
// Find a race by name in the series
struct race *find_race(struct series *series, char *name) {
    struct race *current = series->races;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Calculate the length of a track linked list
int track_length(struct track_piece *track) {
    int len = 0;
    struct track_piece *current = track;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

// Count the number of drivers in a driver linked list
int driver_count(struct driver *drivers) {
    int count = 0;
    struct driver *current = drivers;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Check if a driver exists in a race's driver list
int driver_exists_in_race(struct race *race, char *name) {
    struct driver *current = race->drivers;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Append a race to the end of the series' race list
void append_race(struct series *series, struct race *new_race) {
    if (series->races == NULL) {
        series->races = new_race;
        return;
    }
    struct race *current = series->races;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_race;
}

// Append a track piece to the end of a race's track list
void append_track(struct race *race, struct track_piece *new_track) {
    if (race->track == NULL) {
        race->track = new_track;
        return;
    }
    struct track_piece *current = race->track;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_track;
}

// Append a driver to the end of a driver linked list
void append_driver(struct driver **head, struct driver *new_driver) {
    if (*head == NULL) {
        *head = new_driver;
        return;
    }
    struct driver *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_driver;
}

// Insert a track piece at 1-indexed position n
void insert_track(struct race *race, struct track_piece *new_track, int n) {
    int len = track_length(race->track);
    if (n <= 1 || race->track == NULL) {
        new_track->next = race->track;
        race->track = new_track;
        return;
    }
    if (n > len) {
        append_track(race, new_track);
        return;
    }
    struct track_piece *current = race->track;
    for (int i = 1; i < n - 1; i++) {
        current = current->next;
    }
    new_track->next = current->next;
    current->next = new_track;
}

// Insert a driver at 1-indexed position n
void insert_driver(struct race *race, struct driver *new_driver, int n) {
    int count = driver_count(race->drivers);
    if (n <= 1 || race->drivers == NULL) {
        new_driver->next = race->drivers;
        race->drivers = new_driver;
        return;
    }
    if (n > count) {
        append_driver(&race->drivers, new_driver);
        return;
    }
    struct driver *current = race->drivers;
    for (int i = 1; i < n - 1; i++) {
        current = current->next;
    }
    new_driver->next = current->next;
    current->next = new_driver;
}

// Remove a driver from a linked list, return the removed driver
struct driver *remove_driver_from_list(struct driver **head, char *name) {
    if (*head == NULL) {
        return NULL;
    }

    struct driver *current = *head;
    if (strcmp(current->name, name) == 0) {
        *head = current->next;
        current->next = NULL;
        return current;
    }

    while (current->next != NULL) {
        if (strcmp(current->next->name, name) == 0) {
            struct driver *removed = current->next;
            current->next = removed->next;
            removed->next = NULL;
            return removed;
        }
        current = current->next;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Memory Management Functions
///////////////////////////////////////////////////////////////////////////////
// Free all memory for a driver linked list
void free_drivers(struct driver *drivers) {
    struct driver *current = drivers;
    while (current != NULL) {
        struct driver *temp = current;
        current = current->next;
        free(temp);
    }
}

// Free all memory for a track linked list
void free_track(struct track_piece *track) {
    struct track_piece *current = track;
    while (current != NULL) {
        struct track_piece *temp = current;
        current = current->next;
        free_drivers(temp->drivers);
        free(temp);
    }
}

// Free all memory for a single race
void free_race(struct race *race) {
    free_track(race->track);
    free_drivers(race->drivers);
    free(race);
}

// Free all memory for the entire series
void free_series(struct series *series) {
    struct race *current = series->races;
    while (current != NULL) {
        struct race *temp = current;
        current = current->next;
        free_race(temp);
    }
    free(series);
}

///////////////////////////////////////////////////////////////////////////////
// Stage 1.3 - Add Commands Implementation
///////////////////////////////////////////////////////////////////////////////
// Add a new race to the series
void add_race(struct series *series) {
    char race_name[MAX_SIZE];
    scan_name(race_name);
    enum location location = scan_location();

    if (find_race(series, race_name) != NULL) {
        printf("ERROR: Race %s already exists.\n", race_name);
        return;
    }

    struct race *new_race = create_race(race_name, location);
    append_race(series, new_race);
    printf("Race: '%s' added!\n", race_name);
}

// Add a track piece to an existing race
void add_track_piece(struct series *series) {
    char race_name[MAX_SIZE];
    scan_name(race_name);
    enum piece_type type = scan_piece_type();

    if (type == INVALID) {
        printf("ERROR: Invalid track piece type.\n");
        return;
    }

    struct race *race = find_race(series, race_name);
    if (race == NULL) {
        printf("ERROR: No race with name %s.\n", race_name);
        return;
    }

    if (race->status != NOT_STARTED) {
        printf("ERROR: Race %s has already started.\n", race_name);
        return;
    }

    struct track_piece *new_track = create_track(type);
    append_track(race, new_track);
    printf("Track Piece: '%s' added!\n", type_to_string(type));
}

// Add a driver to an existing race
void add_driver(struct series *series) {
    char race_name[MAX_SIZE], driver_name[MAX_SIZE];
    scan_name(race_name);
    scan_name(driver_name);
    int speed;
    scanf("%d", &speed);

    struct race *race = find_race(series, race_name);
    if (race == NULL) {
        printf("ERROR: No race with name %s.\n", race_name);
        return;
    }
    if (race->status != NOT_STARTED) {
        printf("ERROR: Race %s has already started.\n", race_name);
        return;
    }
    if (driver_exists_in_race(race, driver_name)) {
        printf("ERROR: Driver with name %s already exists in race.\n", driver_name);
        return;
    }
    if (speed < MIN_SPEED || speed > MAX_SPEED) {
        printf("ERROR: Driver speed must be within 1 and 3.\n");
        return;
    }
    if (driver_count(race->drivers) >= MAX_DRIVERS) {
        printf("ERROR: Maximum of 8 drivers per race.\n");
        return;
    }

    struct driver *new_driver = create_driver(driver_name, speed);
    append_driver(&race->drivers, new_driver);
    printf("Driver: '%s' added!\n", driver_name);
}

///////////////////////////////////////////////////////////////////////////////
// Stage 1.4 - Print Series Implementation
///////////////////////////////////////////////////////////////////////////////
// Print the entire racing series
void print_series(struct series *series) {
    printf("===================[ %s ]===================\n", series->name);

    if (series->races == NULL) {
        printf("The racing series is empty!\n");
        return;
    }

    printf("Races: \n");
    struct race *current_race = series->races;
    while (current_race != NULL) {
        printf("    %s [%s]\n", current_race->name, status_to_string(current_race->status));
        printf("    (%s)\n", location_to_string(current_race->location));
        print_track(current_race->track);
        print_drivers(current_race);
        current_race = current_race->next;
    }
    printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
// Modified Print Functions (Stage 2.4 compliant)
///////////////////////////////////////////////////////////////////////////////
// Print track with driver positions
void print_track(struct track_piece *track) {
    if (track == NULL) {
        printf("        Empty Track!\n");
        return;
    }
    while (track != NULL) {
        printf("        %s\n", type_to_string(track->type));
        if (track->drivers != NULL) {
            struct driver *current_driver = track->drivers;
            int pos = 1;
            while (current_driver != NULL) {
                printf("        %d. %s\n", pos, current_driver->name);
                pos++;
                current_driver = current_driver->next;
            }
        }
        if (track->next != NULL) {
            printf("        |\n");
            printf("        ⌄\n");
        }
        track = track->next;
    }
}

// Print drivers/leaderboard based on race status
void print_drivers(struct race *race) {
    printf("        Drivers:\n");
    if (race->status == NOT_STARTED || race->status == COMPLETED) {
        struct driver *current = race->drivers;
        if (current == NULL) {
            printf("            Empty Driver List!\n");
            return;
        }
        int count = 1;
        while (current != NULL) {
            printf("            %d. %s (speed: %d)\n",
                   count, current->name, current->speed);
            count++;
            current = current->next;
        }
    } else if (race->status == ACTIVE) {
        int rank = 1;
        int len = track_length(race->track);
        for (int i = len; i >= 1; i--) {
            struct track_piece *current_track = race->track;
            for (int j = 1; j < i; j++) {
                current_track = current_track->next;
            }
            struct driver *current_driver = current_track->drivers;
            while (current_driver != NULL) {
                printf("            %d. %s (speed: %d)\n",
                       rank, current_driver->name, current_driver->speed);
                rank++;
                current_driver = current_driver->next;
            }
        }
        if (rank == 1) {
            printf("            Empty Driver List!\n");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Race Simulation Helpers (Stage 3.4/3.5)
///////////////////////////////////////////////////////////////////////////////
// Check if all drivers are on the finish line
int is_race_finished(struct race *race) {
    struct track_piece *finish_line = NULL;
    struct track_piece *current_track = race->track;
    while (current_track != NULL) {
        if (current_track->type == FINISH_LINE) {
            finish_line = current_track;
            break;
        }
        current_track = current_track->next;
    }
    if (finish_line == NULL) return 0;

    int total_drivers = 0;
    current_track = race->track;
    while (current_track != NULL) {
        total_drivers += driver_count(current_track->drivers);
        current_track = current_track->next;
    }

    int finish_drivers = driver_count(finish_line->drivers);
    return (total_drivers == finish_drivers && total_drivers > 0);
}

// Execute a single turn of driver movement
void play_single_turn(struct race *race) {
    struct driver_node {
        struct driver *driver;
        struct track_piece *current_track;
        struct driver_node *next;
    };

    struct driver_node *move_list = NULL;
    struct driver_node **move_tail = &move_list;

    int len = track_length(race->track);
    for (int i = len; i >= 1; i--) {
        struct track_piece *current_track = race->track;
        for (int j = 1; j < i; j++) {
            current_track = current_track->next;
        }
        struct driver *current_driver = current_track->drivers;
        while (current_driver != NULL) {
            struct driver *next_driver = current_driver->next;
            remove_driver_from_list(&current_track->drivers, current_driver->name);

            struct driver_node *new_node = malloc(sizeof(struct driver_node));
            new_node->driver = current_driver;
            new_node->current_track = current_track;
            new_node->next = NULL;
            *move_tail = new_node;
            move_tail = &new_node->next;

            current_driver = next_driver;
        }
    }

    struct driver_node *current_node = move_list;
    while (current_node != NULL) {
        struct driver *driver = current_node->driver;
        struct track_piece *target_track = current_node->current_track;
        int steps = driver->speed;

        for (int i = 0; i < steps; i++) {
            if (target_track->next != NULL) {
                target_track = target_track->next;
            } else {
                break;
            }
        }
        append_driver(&target_track->drivers, driver);
        current_node = current_node->next;
    }

    struct driver_node *temp_node;
    current_node = move_list;
    while (current_node != NULL) {
        temp_node = current_node;
        current_node = current_node->next;
        free(temp_node);
    }
}

// Finalize a completed race
void finish_race(struct race *race) {
    struct track_piece *finish_line = NULL;
    struct track_piece *current_track = race->track;
    while (current_track != NULL) {
        if (current_track->type == FINISH_LINE) {
            finish_line = current_track;
            break;
        }
        current_track = current_track->next;
    }

    race->drivers = finish_line->drivers;
    finish_line->drivers = NULL;
    race->status = COMPLETED;

    printf("Race %s has finished!\n", race->name);
    printf("Final positions: \n");
    struct driver *current_driver = race->drivers;
    int place = 1;
    while (current_driver != NULL) {
        enum points points = place_to_points(place);
        printf("    %d. %s (%d points)\n", place, current_driver->name, points);
        place++;
        current_driver = current_driver->next;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Stage 1.2 - Command Loop Implementation (all commands)
///////////////////////////////////////////////////////////////////////////////
// Main command loop for user interaction
void command_loop(struct series *series) {
    char cmd;
    while (1) {
        printf("Enter command: ");
        if (scanf(" %c", &cmd) == EOF) {
            free_series(series);
            break;
        }

        switch (cmd) {
            case '?':
                print_usage();
                break;

            case 'a': {
                char sub_cmd;
                scanf(" %c", &sub_cmd);
                if (sub_cmd == 'r') {
                    add_race(series);
                } else if (sub_cmd == 't') {
                    add_track_piece(series);
                } else if (sub_cmd == 'd') {
                    add_driver(series);
                } else {
                    printf("ERROR: Invalid command.\n");
                }
                break;
            }

            case '*':
                print_series(series);
                break;

            // 修改后的 command_loop 函数中的插入命令处理部分
            case 'i': {
                char sub_cmd;
                scanf(" %c", &sub_cmd);
                int n;
                scanf("%d", &n);

                // 先读取所有参数，再进行错误检查
                if (sub_cmd == 't') {
                    char race_name[MAX_SIZE];
                    scan_name(race_name);
                    enum piece_type type = scan_piece_type();

                    if (n < 1) {
                        printf("ERROR: Position must be at least 1.\n");
                        break;
                    }

                    if (type == INVALID) {
                        printf("ERROR: Invalid track piece type.\n");
                        break;
                    }

                    struct race *race = find_race(series, race_name);
                    if (race == NULL) {
                        printf("ERROR: No race with name %s.\n", race_name);
                        break;
                    }

                    if (race->status != NOT_STARTED) {
                        printf("ERROR: Race %s has already started.\n", race_name);
                        break;
                    }

                    struct track_piece *new_track = create_track(type);
                    insert_track(race, new_track, n);
                    printf("Track Piece: '%s' inserted!\n", type_to_string(type));
                } else if (sub_cmd == 'd') {
                    char race_name[MAX_SIZE], driver_name[MAX_SIZE];
                    scan_name(race_name);
                    scan_name(driver_name);
                    int speed;
                    scanf("%d", &speed);

                    if (n < 1) {
                        printf("ERROR: Position must be at least 1.\n");
                        break;
                    }

                    struct race *race = find_race(series, race_name);
                    if (race == NULL) {
                        printf("ERROR: No race with name %s.\n", race_name);
                        break;
                    }

                    if (race->status != NOT_STARTED) {
                        printf("ERROR: Race %s has already started.\n", race_name);
                        break;
                    }

                    if (driver_exists_in_race(race, driver_name)) {
                        printf("ERROR: Driver with name %s already exists in race.\n", driver_name);
                        break;
                    }

                    if (speed < MIN_SPEED || speed > MAX_SPEED) {
                        printf("ERROR: Driver speed must be within 1 and 3.\n");
                        break;
                    }

                    if (driver_count(race->drivers) >= MAX_DRIVERS) {
                        printf("ERROR: Maximum of 8 drivers per race.\n");
                        break;
                    }

                    struct driver *new_driver = create_driver(driver_name, speed);
                    insert_driver(race, new_driver, n);
                    printf("Driver: '%s' inserted!\n", driver_name);
                } else {
                    printf("ERROR: Invalid command.\n");
                }
                break;
            }

            case 'c': {
                char existing_name[MAX_SIZE], new_name[MAX_SIZE];
                scan_name(existing_name);
                scan_name(new_name);

                struct race *existing_race = find_race(series, existing_name);
                if (existing_race == NULL) {
                    printf("ERROR: No race with name %s.\n", existing_name);
                    break;
                }
                if (find_race(series, new_name) != NULL) {
                    printf("ERROR: Race %s already exists.\n", new_name);
                    break;
                }
                if (existing_race->track == NULL) {
                    printf("ERROR: No track pieces in race to be copied.\n");
                    break;
                }

                struct race *new_race = create_race(new_name, existing_race->location);
                struct track_piece *current_track = existing_race->track;
                while (current_track != NULL) {
                    struct track_piece *copied_track = create_track(current_track->type);
                    append_track(new_race, copied_track);
                    current_track = current_track->next;
                }
                append_race(series, new_race);
                break;
            }

            case 'v': {
                char race_name[MAX_SIZE];
                scan_name(race_name);
                struct race *race = find_race(series, race_name);

                if (race == NULL) {
                    printf("ERROR: No race with name %s.\n", race_name);
                    break;
                }
                if (race->status != NOT_STARTED) {
                    printf("ERROR: Race %s has already started.\n", race_name);
                    break;
                }

                struct track_piece *current = race->track;
                while (current != NULL && current->next != NULL) {
                    if ((current->type == RIGHT_TURN && current->next->type == RIGHT_TURN) ||
                        (current->type == LEFT_TURN && current->next->type == LEFT_TURN)) {
                        struct track_piece *straight = create_track(STRAIGHT);
                        straight->next = current->next;
                        current->next = straight;
                    }
                    if (current->type == JUMP && current->next->type != STRAIGHT) {
                        struct track_piece *straight = create_track(STRAIGHT);
                        straight->next = current->next;
                        current->next = straight;
                    }
                    current = current->next;
                }
                printf("Race %s validated!\n", race_name);
                break;
            }

            case 's': {
                char race_name[MAX_SIZE];
                scan_name(race_name);
                struct race *race = find_race(series, race_name);

                if (race == NULL) {
                    printf("ERROR: No race with name %s.\n", race_name);
                    break;
                }
                if (race->status != NOT_STARTED) {
                    printf("ERROR: Race %s has already started.\n", race_name);
                    break;
                }
                if (race->track == NULL) {
                    printf("ERROR: No track pieces in race %s.\n", race_name);
                    break;
                }
                if (race->drivers == NULL) {
                    printf("ERROR: No drivers in race %s.\n", race_name);
                    break;
                }

                struct track_piece *start_line = create_track(START_LINE);
                start_line->next = race->track;
                race->track = start_line;

                struct track_piece *finish_line = create_track(FINISH_LINE);
                append_track(race, finish_line);

                start_line->drivers = race->drivers;
                race->drivers = NULL;
                race->status = ACTIVE;
                printf("Race %s started!\n", race_name);
                break;
            }

            case 'm': {
                char driver_name[MAX_SIZE], current_race_name[MAX_SIZE], new_race_name[MAX_SIZE];
                scan_name(driver_name);
                scan_name(current_race_name);
                scan_name(new_race_name);

                struct race *current_race = find_race(series, current_race_name);
                if (current_race == NULL) {
                    printf("ERROR: No race with name %s.\n", current_race_name);
                    break;
                }
                if (current_race->status != NOT_STARTED) {
                    printf("ERROR: Race %s has already started.\n", current_race_name);
                    break;
                }

                struct race *new_race = find_race(series, new_race_name);
                if (new_race == NULL) {
                    printf("ERROR: No race with name %s.\n", new_race_name);
                    break;
                }
                if (new_race->status != NOT_STARTED) {
                    printf("ERROR: Race %s has already started.\n", new_race_name);
                    break;
                }

                if (!driver_exists_in_race(current_race, driver_name)) {
                    printf("ERROR: Driver %s not found in race %s.\n", driver_name, current_race_name);
                    break;
                }
                if (driver_exists_in_race(new_race, driver_name)) {
                    printf("ERROR: Driver %s already in race %s.\n", driver_name, new_race_name);
                    break;
                }
                if (driver_count(new_race->drivers) >= MAX_DRIVERS) {
                    printf("ERROR: Race %s already at maximum driver limit.\n", new_race_name);
                    break;
                }

                struct driver *moved_driver = remove_driver_from_list(&current_race->drivers, driver_name);
                append_driver(&new_race->drivers, moved_driver);
                printf("Driver: '%s' moved to race '%s'!\n", driver_name, new_race_name);
                break;
            }

            case 'q':
                free_series(series);
                return;

            case 'r': {
                char driver_name[MAX_SIZE];
                scan_name(driver_name);
                int found = 0;

                struct race *current_race = series->races;
                while (current_race != NULL) {
                    if (current_race->status == NOT_STARTED) {
                        struct driver *removed = remove_driver_from_list(&current_race->drivers, driver_name);
                        if (removed != NULL) {
                            free(removed);
                            found = 1;
                        }
                    } else if (current_race->status == ACTIVE) {
                        struct track_piece *current_track = current_race->track;
                        while (current_track != NULL) {
                            struct driver *removed = remove_driver_from_list(&current_track->drivers, driver_name);
                            if (removed != NULL) {
                                free(removed);
                                found = 1;
                            }
                            current_track = current_track->next;
                        }
                    }
                    current_race = current_race->next;
                }

                if (!found) {
                    printf("ERROR: Driver %s not found in the series.\n", driver_name);
                } else {
                    printf("Driver %s removed from the racing series!\n", driver_name);
                }
                break;
            }

            case 't': {
                char race_name[MAX_SIZE];
                scan_name(race_name);
                int turns;
                scanf("%d", &turns);

                struct race *race = find_race(series, race_name);
                if (race == NULL) {
                    printf("ERROR: No race with name %s.\n", race_name);
                    break;
                }
                if (race->status != ACTIVE) {
                    printf("ERROR: Race %s is not active.\n", race_name);
                    break;
                }
                if (turns < 1) {
                    printf("ERROR: Turns must be a positive integer.\n");
                    break;
                }

                for (int i = 0; i < turns; i++) {
                    if (is_race_finished(race)) break;
                    play_single_turn(race);
                    if (is_race_finished(race)) {
                        finish_race(race);
                        break;
                    }
                }
                break;
            }

            case 'e': {
                char race_name[MAX_SIZE];
                scan_name(race_name);
                struct race *race = find_race(series, race_name);

                if (race == NULL) {
                    printf("ERROR: No race with name %s.\n", race_name);
                    break;
                }
                if (race->status != ACTIVE) {
                    printf("ERROR: Race %s is not active.\n", race_name);
                    break;
                }

                while (!is_race_finished(race)) {
                    play_single_turn(race);
                }
                finish_race(race);
                break;
            }

            default:
                printf("ERROR: Invalid command.\n");
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Provided Function Definitions (DO NOT MODIFY)
///////////////////////////////////////////////////////////////////////////////
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

void scan_name(char name[MAX_SIZE]) {
    scan_token(name, MAX_SIZE);
}

enum location scan_location(void) {
    char type[MAX_SIZE];
    scan_token(type, MAX_SIZE);
    return string_to_location(type);
}

enum piece_type scan_piece_type(void) {
    char type[MAX_SIZE];
    scan_token(type, MAX_SIZE);
    return string_to_type(type);
}

enum points place_to_points(int place) {
    if (place == 1) return FIRST;
    if (place == 2) return SECOND;
    if (place == 3) return THIRD;
    if (place == 4) return FOURTH;
    if (place == 5) return FIFTH;
    if (place == 6) return SIXTH;
    if (place == 7) return SEVENTH;
    return EIGHTH;
}

enum location string_to_location(char *type_str) {
    int len = strlen(type_str);
    if (strncasecmp(type_str, "ALBERT_PARK", len) == 0) return ALBERT_PARK;
    if (strncasecmp(type_str, "MARINA_BAY", len) == 0) return MARINA_BAY;
    if (strncasecmp(type_str, "MONZA", len) == 0) return MONZA;
    if (strncasecmp(type_str, "SUZUKA", len) == 0) return SUZUKA;
    return ZANDVOORT;
}

char *location_to_string(enum location location) {
    if (location == ALBERT_PARK) return "ALBERT_PARK";
    if (location == MARINA_BAY) return "MARINA_BAY";
    if (location == MONZA) return "MONZA";
    if (location == SUZUKA) return "SUZUKA";
    return "ZANDVOORT";
}

enum piece_type string_to_type(char *type_str) {
    int len = strlen(type_str);
    if (strncasecmp(type_str, "START_LINE", len) == 0) return START_LINE;
    if (strncasecmp(type_str, "FINISH_LINE", len) == 0) return FINISH_LINE;
    if (strncasecmp(type_str, "STRAIGHT", len) == 0) return STRAIGHT;
    if (strncasecmp(type_str, "LEFT_TURN", len) == 0) return LEFT_TURN;
    if (strncasecmp(type_str, "RIGHT_TURN", len) == 0) return RIGHT_TURN;
    if (strncasecmp(type_str, "JUMP", len) == 0) return JUMP;
    if (strncasecmp(type_str, "MYSTERY_BOX", len) == 0) return MYSTERY_BOX;
    return INVALID;
}

char *type_to_string(enum piece_type type) {
    if (type == START_LINE) return "START_LINE";
    if (type == FINISH_LINE) return "FINISH_LINE";
    if (type == STRAIGHT) return "STRAIGHT";
    if (type == LEFT_TURN) return "LEFT_TURN";
    if (type == RIGHT_TURN) return "RIGHT_TURN";
    if (type == JUMP) return "JUMP";
    if (type == MYSTERY_BOX) return "MYSTERY_BOX";
    return "INVALID";
}

char *status_to_string(enum race_status status) {
    if (status == NOT_STARTED) return "NOT_STARTED";
    if (status == ACTIVE) return "ACTIVE";
    return "COMPLETED";
}

int scan_token(char *buffer, int buffer_size) {
    if (buffer_size == 0) return 0;
    char c;
    int i = 0;
    int num_scanned = 0;
    scanf(" ");
    while (i < buffer_size - 1 && (num_scanned = scanf("%c", &c)) == 1 &&
           !isspace(c)) {
        buffer[i++] = c;
    }
    if (i > 0) buffer[i] = '\0';
    return num_scanned;
}
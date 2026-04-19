// Assignment 2 26T1 COMP1511: CS Karting
// cs_karting.h
//
// Written by <YOUR-NAME>, <YOUR-ZID>
// on <TODAYS-DATE>
//
// CS Karting racing series management system header file
////////////////////////////////////////////////////////////////////////////////
// Provided Constants
////////////////////////////////////////////////////////////////////////////////
#define MAX_SIZE 100

////////////////////////////////////////////////////////////////////////////////
// Your constants
////////////////////////////////////////////////////////////////////////////////
#define MAX_DRIVERS 8
#define MIN_SPEED 1
#define MAX_SPEED 3

////////////////////////////////////////////////////////////////////////////////
// Provided Enums
////////////////////////////////////////////////////////////////////////////////
enum mystery_box {
    MUSHROOM,
    BANANA_PEEL,
    STAR,
    LIGHTNING,
    BLUE_SHELL,
    BULLET,
    NO_MYSTERY_BOX
};

enum piece_type {
    START_LINE,
    FINISH_LINE,
    STRAIGHT,
    LEFT_TURN,
    RIGHT_TURN,
    JUMP,
    MYSTERY_BOX,
    INVALID
};

enum location {
    ALBERT_PARK,
    MARINA_BAY,
    MONZA,
    SUZUKA,
    ZANDVOORT
};

enum race_status {
    NOT_STARTED,
    ACTIVE,
    COMPLETED
};

enum points {
    FIRST = 15,
    SECOND = 10,
    THIRD = 7,
    FOURTH = 5,
    FIFTH = 4,
    SIXTH = 3,
    SEVENTH = 2,
    EIGHTH = 1
};

////////////////////////////////////////////////////////////////////////////////
// Provided Structs
////////////////////////////////////////////////////////////////////////////////
struct driver {
    char name[MAX_SIZE];
    int speed;
    struct driver *next;
};

struct track_piece {
    enum piece_type type;
    struct driver *drivers;
    struct track_piece *next;
};

struct race {
    char name[MAX_SIZE];
    enum location location;
    enum race_status status;
    struct track_piece *track;
    struct driver *drivers;
    struct race *next;
};

struct series {
    char name[MAX_SIZE];
    struct race *races;
};


struct driver_rank;
////////////////////////////////////////////////////////////////////////////////
// Provided function prototypes
////////////////////////////////////////////////////////////////////////////////
struct series *create_series(char name[MAX_SIZE]);
struct race *create_race(char name[MAX_SIZE], enum location location);
struct track_piece *create_track(enum piece_type type);
struct driver *create_driver(char name[MAX_SIZE], int speed);
void command_loop(struct series *series);
void add_race(struct series *series);
void add_track_piece(struct series *series);
void add_driver(struct series *series);

////////////////////////////////////////////////////////////////////////////////
// Your function prototypes
struct driver_rank *calculate_driver_ranks(struct race *race);
int find_driver_rank(struct driver_rank *ranks, char *name);
void free_driver_ranks(struct driver_rank *ranks);
////////////////////////////////////////////////////////////////////////////////
// Core functionality helpers
struct race *find_race(struct series *series, char *name);
int track_length(struct track_piece *track);
int driver_count(struct driver *drivers);
int driver_exists_in_race(struct race *race, char *name);

// Linked list append/insert helpers
void append_race(struct series *series, struct race *new_race);
void append_track(struct race *race, struct track_piece *new_track);
void append_driver(struct driver **head, struct driver *new_driver);
void insert_track(struct race *race, struct track_piece *new_track, int n);
void insert_driver(struct race *race, struct driver *new_driver, int n);

// Memory management functions
void free_series(struct series *series);
void free_race(struct race *race);
void free_track(struct track_piece *track);
void free_drivers(struct driver *drivers);
struct driver *remove_driver_from_list(struct driver **head, char *name);

// Race simulation helpers
int is_race_finished(struct race *race);
void play_single_turn(struct race *race);
void finish_race(struct race *race);

////////////////////////////////////////////////////////////////////////////////
// Helper function prototypes
////////////////////////////////////////////////////////////////////////////////
void print_usage();
void print_welcome_banner();
void print_series(struct series *series);
void print_track(struct track_piece *track, struct driver_rank *ranks);
void print_drivers(struct race *race);
void scan_name(char name[MAX_SIZE]);
enum location scan_location(void);
enum piece_type scan_piece_type(void);
enum location string_to_location(char *type_str);
char *location_to_string(enum location location);
enum piece_type string_to_type(char *type_str);
char *type_to_string(enum piece_type type);
enum points place_to_points(int place);
char *status_to_string(enum race_status status);
int scan_token(char *buffer, int buffer_size);
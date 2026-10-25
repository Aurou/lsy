// cs_submarine.c
// Description: CS Submarine, Stages 1.1-3.5.

// Provided Libraries
#include <stdio.h>

// Provided constants
#define ROWS 10
#define COLS 10

#define NO_SUBMARINE -1

// Additional game constants
#define MAX_OXYGEN 100
#define NORMAL_OXYGEN_COST 5
#define AIR_REFILL 20
#define INITIAL_TORPEDOES 4
#define REVEAL_RANGE 2
#define MAX_PUSHES 5
#define CURRENT_ROTATION_PERIOD 3
#define MAX_ENTITIES 100
#define HISTORY_SIZE 257

#define ADD_TREASURE 't'
#define ADD_AIR 'o'
#define ADD_MINE 'm'
#define ADD_EXIT 'x'
#define ADD_ANGLERFISH 'a'
#define ADD_JELLYFISH 'j'
#define ADD_CURRENT 'c'
#define ADD_SEAWEED 's'
#define END_SETUP 'e'
#define HORIZONTAL 'h'
#define VERTICAL 'v'

#define MOVE_UP 'w'
#define MOVE_DOWN 's'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define QUIT_GAME 'q'
#define PRINT_STATUS 'p'
#define PRINT_MAP 'm'
#define TOGGLE_VISION 'v'
#define UNDO_TURNS 'u'



// Provided Enums
// Enum for features on the game board
enum entity {
    OCEAN,
    SEAWEED,
    TREASURE,
    EXIT_LOCKED,
    EXIT_UNLOCKED,
    AIR_POCKET,
    JELLYFISH,
    ANGLERFISH,
    MINE,
    HIDDEN,
    SEEKING_MINE,
    CURRENT_UP,
    CURRENT_DOWN,
    CURRENT_LEFT,
    CURRENT_RIGHT,
    TORPEDO
};

// Directions are shared by patrols and currents.
enum direction {
    NO_DIRECTION,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum game_result {
    RUNNING,
    WON,
    OUT_OF_OXYGEN,
    SUNK
};


// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
    enum direction current;
    enum direction jellyfish_direction;
    int anglerfish_revealed;
};

// Complete state is copied into the undo history after each turn.
struct position {
    int row;
    int col;
};

struct submarine {
    struct position position;
    int oxygen;
    int treasure_collected;
    int moves;
    int torpedoes;
};

struct game_state {
    struct tile board[ROWS][COLS];
    struct submarine submarine;
    enum game_result result;
    enum entity cause;
};

struct jellyfish_move {
    struct position start;
    struct position destination;
    enum direction direction;
    int moved;
};


// Provided Function Prototypes
void print_welcome(void);
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int submarine_row, 
    int submarine_col
);
void print_submarine_stats(
    int oxygen,
    int treasure_collected,
    int moves,
    int torpedoes
);
void print_map_statistics(
    int num_treasure,
    int num_air_pockets,
    int num_seaweed,
    int num_mines,
    int num_jellyfish,
    int num_anglerfish,
    int num_exits
);
void print_board_line(void);
void print_tile_spacer(void);
void print_board_header(void);
char *get_name_from_type(enum entity entity_type);
void print_win_message(void);
void print_loss_message(enum entity entity_type);
void print_oxygen_loss_message(void);

// Game helper prototypes
void initialise_game(struct game_state *game);
int setup_game(struct game_state *game);
void read_placement(struct tile board[ROWS][COLS], char command);
int valid_placement(
    struct tile board[ROWS][COLS], struct position position
);
void place_entity(
    struct tile board[ROWS][COLS],
    struct position position,
    char command,
    char direction
);
void place_seaweed(
    struct tile board[ROWS][COLS],
    struct position start,
    char direction,
    int length
);
int spawn_submarine(struct game_state *game);
void play_game(struct game_state *game);
int take_turn(struct game_state *game, char command, int deep_sea);
void push_submarine(struct game_state *game);
void collect_tile(struct game_state *game);
void check_submarine(struct game_state *game);
void unlock_exits(struct tile board[ROWS][COLS]);
void undo_turns(
    struct game_state *game, struct game_state history[HISTORY_SIZE]
);
void move_jellyfish(struct game_state *game);
int plan_jellyfish(
    struct tile board[ROWS][COLS],
    struct jellyfish_move plans[MAX_ENTITIES],
    int requests[ROWS][COLS]
);
void resolve_conflicts(
    struct jellyfish_move plans[MAX_ENTITIES],
    int count,
    int requests[ROWS][COLS]
);
void apply_jellyfish_moves(
    struct tile board[ROWS][COLS],
    struct jellyfish_move plans[MAX_ENTITIES],
    int count
);
void carry_jellyfish(
    struct game_state *game,
    struct jellyfish_move plans[MAX_ENTITIES],
    int count
);
void push_jellyfish(struct game_state *game, struct position position);
int jellyfish_hits_submarine(struct game_state *game);
void rotate_currents(struct tile board[ROWS][COLS]);
void print_game(struct game_state *game, int deep_sea);
void reveal_anglerfish(
    struct tile board[ROWS][COLS],
    struct position position,
    struct position submarine
);
enum entity visible_entity(
    struct tile tile,
    struct position position,
    struct position submarine,
    int deep_sea
);
enum entity current_entity(enum direction direction);
void print_statistics(struct tile board[ROWS][COLS]);
void print_status(struct submarine submarine);
void print_vision_message(int deep_sea);
void print_result(struct game_state *game);
int on_board(struct position position);
struct position next_position(
    struct position position,
    enum direction direction
);
enum direction command_direction(char command);
enum direction reverse_direction(enum direction direction);
int submarine_blocked(
    struct tile board[ROWS][COLS],
    struct position position
);
int jellyfish_can_enter(
    struct tile board[ROWS][COLS],
    struct position position
);
int is_hazard(enum entity entity);
int count_entity(struct tile board[ROWS][COLS], enum entity entity);
int absolute(int number);
int column_distance(int first, int second);


int main(void) {
    print_welcome();
    struct game_state game;
    initialise_game(&game);
    if (setup_game(&game)) {
        play_game(&game);
    }
    return 0;
}

// =============================================================================
// Game setup
// =============================================================================

// Create a fully initialised game, including fields used by undo snapshots.
void initialise_game(struct game_state *game) {
    initialise_board(game->board);
    game->submarine.position.row = NO_SUBMARINE;
    game->submarine.position.col = NO_SUBMARINE;
    game->submarine.oxygen = MAX_OXYGEN;
    game->submarine.treasure_collected = 0;
    game->submarine.moves = 0;
    game->submarine.torpedoes = INITIAL_TORPEDOES;
    game->result = RUNNING;
    game->cause = OCEAN;
}

// Read setup until a valid end command, or until input ends.
int setup_game(struct game_state *game) {
    printf("============== Setup Phase ==============\n");
    printf("Enter setup commands:\n");
    char command;
    while (scanf(" %c", &command) == 1) {
        if (command == END_SETUP) {
            if (count_entity(game->board, EXIT_LOCKED) > 0) {
                return spawn_submarine(game);
            }
            printf("Cannot end setup until an EXIT_LOCKED has been placed\n");
        } else {
            read_placement(game->board, command);
        }
    }
    print_game(game, 0);
    return 0;
}

// Read the arguments belonging to one setup command.
void read_placement(struct tile board[ROWS][COLS], char command) {
    struct position position;
    char direction = HORIZONTAL;
    int length = 1;
    if (command == ADD_JELLYFISH || command == ADD_CURRENT) {
        if (scanf(" %c %d %d", &direction,
            &position.row, &position.col) != 3) {
            return;
        }
    } else if (command == ADD_SEAWEED) {
        if (scanf("%d %d %c %d", &position.row, &position.col,
            &direction, &length) != 4) {
            return;
        }
    } else if (scanf("%d %d", &position.row, &position.col) != 2) {
        return;
    }
    if (command == ADD_SEAWEED) {
        place_seaweed(board, position, direction, length);
    } else if (valid_placement(board, position)) {
        place_entity(board, position, command, direction);
    }
}

// Validate a single placement without changing the board.
int valid_placement(struct tile board[ROWS][COLS], struct position position) {
    if (!on_board(position)) {
        printf("Invalid location: position is not on the board!\n");
        return 0;
    }
    if (board[position.row][position.col].entity != OCEAN ||
        board[position.row][position.col].current != NO_DIRECTION) {
        printf("Invalid location: tile is occupied!\n");
        return 0;
    }
    return 1;
}

// Store a single entity, or a current in its separate persistent layer.
void place_entity(
    struct tile board[ROWS][COLS],
    struct position position,
    char command,
    char direction
) {
    struct tile *tile = &board[position.row][position.col];
    if (command == ADD_TREASURE) {
        tile->entity = TREASURE;
    } else if (command == ADD_AIR) {
        tile->entity = AIR_POCKET;
    } else if (command == ADD_MINE) {
        tile->entity = MINE;
    } else if (command == ADD_EXIT) {
        tile->entity = EXIT_LOCKED;
    } else if (command == ADD_ANGLERFISH) {
        tile->entity = ANGLERFISH;
    } else if (command == ADD_CURRENT) {
        tile->current = command_direction(direction);
    } else if (command == ADD_JELLYFISH) {
        tile->entity = JELLYFISH;
        tile->jellyfish_direction = RIGHT;
        if (direction == VERTICAL) {
            tile->jellyfish_direction = UP;
        }
    }
}

// Check the entire seaweed run before writing any of its tiles.
void place_seaweed(
    struct tile board[ROWS][COLS],
    struct position start,
    char direction,
    int length
) {
    struct position end = start;
    if (direction == HORIZONTAL) {
        if (!on_board(start) || length > COLS - start.col) {
            printf("Invalid location: position is not on the board!\n");
            return;
        }
        end.col += length - 1;
    } else {
        if (!on_board(start) || length > start.row + 1) {
            printf("Invalid location: position is not on the board!\n");
            return;
        }
        end.row -= length - 1;
    }
    for (int row = end.row; row <= start.row; row++) {
        for (int col = start.col; col <= end.col; col++) {
            if (!valid_placement(board, (struct position) {row, col})) {
                return;
            }
        }
    }
    for (int row = end.row; row <= start.row; row++) {
        for (int col = start.col; col <= end.col; col++) {
            board[row][col].entity = SEAWEED;
        }
    }
}

// Prompt repeatedly until the submarine can start on plain ocean.
int spawn_submarine(struct game_state *game) {
    struct position position;
    printf("Enter the Submarine's starting position: ");
    while (scanf("%d %d", &position.row, &position.col) == 2) {
        if (on_board(position) &&
            game->board[position.row][position.col].entity == OCEAN &&
            game->board[position.row][position.col].current == NO_DIRECTION) {
            game->submarine.position = position;
            print_game(game, 0);
            printf("============= Gameplay Phase ============\n");
            return 1;
        }
        printf("The Submarine cannot start here!\n");
        printf("Enter the Submarine's starting position: ");
    }
    return 0;
}

// =============================================================================
// Gameplay and turn resolution
// =============================================================================

// Keep deep-sea mode outside snapshots: undo deliberately does not change it.
void play_game(struct game_state *game) {
    struct game_state history[HISTORY_SIZE];
    history[0] = *game;
    int deep_sea = 0;
    char command;
    while (game->result == RUNNING && scanf(" %c", &command) == 1) {
        if (command == QUIT_GAME) {
            printf("============= Quitting Game =============\n");
            return;
        } else if (command == PRINT_STATUS) {
            print_status(game->submarine);
        } else if (command == PRINT_MAP) {
            print_statistics(game->board);
        } else if (command == TOGGLE_VISION) {
            deep_sea = !deep_sea;
            print_vision_message(deep_sea);
            print_game(game, deep_sea);
        } else if (command == UNDO_TURNS) {
            undo_turns(game, history);
            print_game(game, deep_sea);
        } else if (take_turn(game, command, deep_sea)) {
            print_game(game, deep_sea);
            history[game->submarine.moves] = *game;
        }
    }
    if (game->result != RUNNING) {
        print_result(game);
    }
}

// Validate the voluntary move, then perform the complete turn in spec order.
int take_turn(struct game_state *game, char command, int deep_sea) {
    struct position destination = next_position(
        game->submarine.position, command_direction(command));
    if (submarine_blocked(game->board, destination)) {
        printf("The Submarine cannot move there!\n");
        return 0;
    }
    game->submarine.position = destination;
    game->submarine.moves++;
    int oxygen_cost = NORMAL_OXYGEN_COST;
    if (deep_sea) {
        oxygen_cost = destination.row + 1;
    }
    game->submarine.oxygen -= oxygen_cost;
    push_submarine(game);
    if (game->result == RUNNING) {
        collect_tile(game);
        unlock_exits(game->board);
        check_submarine(game);
    }
    if (game->result == RUNNING) {
        move_jellyfish(game);
    }
    if (game->submarine.moves % CURRENT_ROTATION_PERIOD == 0) {
        rotate_currents(game->board);
    }
    return 1;
}

// Follow a chain of currents, stopping on blockage, a hazard, or five pushes.
void push_submarine(struct game_state *game) {
    struct position position = game->submarine.position;
    if (is_hazard(game->board[position.row][position.col].entity)) {
        return;
    }
    for (int push = 0; push < MAX_PUSHES; push++) {
        enum direction direction =
            game->board[position.row][position.col].current;
        if (direction == NO_DIRECTION) {
            return;
        }
        struct position destination = next_position(position, direction);
        if (submarine_blocked(game->board, destination)) {
            return;
        }
        position = destination;
        game->submarine.position = position;
        enum entity entity = game->board[position.row][position.col].entity;
        if (is_hazard(entity)) {
            game->result = SUNK;
            game->cause = entity;
            return;
        }
    }
}

// Items are collected after currents, before checking oxygen or victory.
void collect_tile(struct game_state *game) {
    struct position position = game->submarine.position;
    struct tile *tile = &game->board[position.row][position.col];
    if (tile->entity == TREASURE) {
        game->submarine.treasure_collected++;
        tile->entity = OCEAN;
    } else if (tile->entity == AIR_POCKET) {
        game->submarine.oxygen += AIR_REFILL;
        if (game->submarine.oxygen > MAX_OXYGEN) {
            game->submarine.oxygen = MAX_OXYGEN;
        }
        tile->entity = OCEAN;
    }
}

// A win takes precedence over oxygen loss; oxygen precedes direct hazards.
void check_submarine(struct game_state *game) {
    struct position position = game->submarine.position;
    enum entity entity = game->board[position.row][position.col].entity;
    if (entity == EXIT_UNLOCKED) {
        game->result = WON;
    } else if (game->submarine.oxygen <= 0) {
        game->result = OUT_OF_OXYGEN;
    } else if (is_hazard(entity)) {
        game->result = SUNK;
        game->cause = entity;
    }
}

// Unlock all exits together, including on turn one of a treasure-free map.
void unlock_exits(struct tile board[ROWS][COLS]) {
    if (count_entity(board, TREASURE) != 0) {
        return;
    }
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == EXIT_LOCKED) {
                board[row][col].entity = EXIT_UNLOCKED;
            }
        }
    }
}

// Restore the earlier branch of history; subsequent turns overwrite its future.
void undo_turns(
    struct game_state *game, struct game_state history[HISTORY_SIZE]
) {
    int turns;
    if (scanf("%d", &turns) != 1) {
        return;
    }
    int target = game->submarine.moves - turns;
    if (target < 0) {
        target = 0;
    }
    *game = history[target];
}

// =============================================================================
// Jellyfish: plan together, move together, then ride currents in row order
// =============================================================================

// Resolve all swimming before any ride, so row traversal never biases a swim.
void move_jellyfish(struct game_state *game) {
    struct jellyfish_move plans[MAX_ENTITIES];
    int requests[ROWS][COLS] = {0};
    int count = plan_jellyfish(game->board, plans, requests);
    resolve_conflicts(plans, count, requests);
    apply_jellyfish_moves(game->board, plans, count);
    // Record a collision, but finish every other jellyfish's current ride.
    jellyfish_hits_submarine(game);
    carry_jellyfish(game, plans, count);
}

// Each plan reads only the original board, including other jellyfish.
int plan_jellyfish(
    struct tile board[ROWS][COLS],
    struct jellyfish_move plans[MAX_ENTITIES],
    int requests[ROWS][COLS]
) {
    int count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == JELLYFISH) {
                struct jellyfish_move plan;
                plan.start = (struct position) {row, col};
                plan.direction = board[row][col].jellyfish_direction;
                plan.destination = next_position(plan.start, plan.direction);
                plan.moved = jellyfish_can_enter(board, plan.destination);
                if (plan.moved) {
                    requests[plan.destination.row][plan.destination.col]++;
                } else {
                    plan.destination = plan.start;
                    plan.direction = reverse_direction(plan.direction);
                }
                plans[count] = plan;
                count++;
            }
        }
    }
    return count;
}

// All contenders for the same destination reverse and remain in place.
void resolve_conflicts(
    struct jellyfish_move plans[MAX_ENTITIES],
    int count,
    int requests[ROWS][COLS]
) {
    for (int i = 0; i < count; i++) {
        struct position destination = plans[i].destination;
        if (plans[i].moved && requests[destination.row][destination.col] > 1) {
            plans[i].destination = plans[i].start;
            plans[i].direction = reverse_direction(plans[i].direction);
            plans[i].moved = 0;
        }
    }
}

// Clear sources first, and retain currents beneath every source/destination.
void apply_jellyfish_moves(
    struct tile board[ROWS][COLS],
    struct jellyfish_move plans[MAX_ENTITIES],
    int count
) {
    for (int i = 0; i < count; i++) {
        struct position start = plans[i].start;
        board[start.row][start.col].entity = OCEAN;
        board[start.row][start.col].jellyfish_direction = NO_DIRECTION;
    }
    for (int i = 0; i < count; i++) {
        struct position destination = plans[i].destination;
        board[destination.row][destination.col].entity = JELLYFISH;
        board[destination.row][destination.col].jellyfish_direction =
            plans[i].direction;
    }
}

// Capture eligible arrival positions before rides alter any of those positions.
void carry_jellyfish(
    struct game_state *game,
    struct jellyfish_move plans[MAX_ENTITIES],
    int count
) {
    int arrivals[ROWS][COLS] = {0};
    for (int i = 0; i < count; i++) {
        struct position position = plans[i].destination;
        if (plans[i].moved) {
            arrivals[position.row][position.col] = 1;
        }
    }
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (arrivals[row][col]) {
                push_jellyfish(game, (struct position) {row, col});
            }
        }
    }
}

// A ride changes position, never the jellyfish's own patrol direction.
void push_jellyfish(struct game_state *game, struct position position) {
    struct position submarine = game->submarine.position;
    if (position.row == submarine.row && position.col == submarine.col) {
        return;
    }
    for (int push = 0; push < MAX_PUSHES; push++) {
        enum direction current =
            game->board[position.row][position.col].current;
        if (current == NO_DIRECTION) {
            return;
        }
        struct position destination = next_position(position, current);
        if (!jellyfish_can_enter(game->board, destination)) {
            return;
        }
        enum direction patrol =
            game->board[position.row][position.col].jellyfish_direction;
        game->board[position.row][position.col].entity = OCEAN;
        game->board[position.row][position.col].jellyfish_direction =
            NO_DIRECTION;
        game->board[destination.row][destination.col].entity = JELLYFISH;
        game->board[destination.row][destination.col].jellyfish_direction =
            patrol;
        position = destination;
        if (position.row == submarine.row && position.col == submarine.col) {
            jellyfish_hits_submarine(game);
            return;
        }
    }
}

// Detect contact during swimming or a ride, even if a later push could leave.
int jellyfish_hits_submarine(struct game_state *game) {
    struct position position = game->submarine.position;
    if (game->board[position.row][position.col].entity == JELLYFISH) {
        game->result = SUNK;
        game->cause = JELLYFISH;
        return 1;
    }
    return 0;
}

// Currents rotate even on a turn that has just ended the game.
void rotate_currents(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            enum direction direction = board[row][col].current;
            if (direction == UP) {
                board[row][col].current = RIGHT;
            } else if (direction == RIGHT) {
                board[row][col].current = DOWN;
            } else if (direction == DOWN) {
                board[row][col].current = LEFT;
            } else if (direction == LEFT) {
                board[row][col].current = UP;
            }
        }
    }
}

// =============================================================================
// Display: visibility affects a temporary board, never the real map
// =============================================================================

// Reveal nearby anglerfish persistently, and construct this frame's display.
void print_game(struct game_state *game, int deep_sea) {
    struct tile display[ROWS][COLS];
    struct position submarine = game->submarine.position;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            struct position position = {row, col};
            reveal_anglerfish(game->board, position, submarine);
            display[row][col] = game->board[row][col];
            display[row][col].entity = visible_entity(
                game->board[row][col], position, submarine, deep_sea);
        }
    }
    print_board(display, submarine.row, submarine.col);
}

// Anglerfish use a square reveal range with the shorter column distance.
void reveal_anglerfish(
    struct tile board[ROWS][COLS],
    struct position position,
    struct position submarine
) {
    if (submarine.row == NO_SUBMARINE) {
        return;
    }
    if (board[position.row][position.col].entity == ANGLERFISH &&
        absolute(position.row - submarine.row) <= REVEAL_RANGE &&
        column_distance(position.col, submarine.col) <= REVEAL_RANGE) {
        board[position.row][position.col].anglerfish_revealed = 1;
    }
}

// Squared distances avoid rounding and do not require the maths library.
enum entity visible_entity(
    struct tile tile,
    struct position position,
    struct position submarine,
    int deep_sea
) {
    if (deep_sea && submarine.row != NO_SUBMARINE) {
        int row_distance = position.row - submarine.row;
        int col_distance = column_distance(position.col, submarine.col);
        int radius = ROWS - submarine.row;
        if (row_distance * row_distance + col_distance * col_distance >
            radius * radius) {
            return HIDDEN;
        }
    }
    if (tile.entity == ANGLERFISH && !tile.anglerfish_revealed) {
        return OCEAN;
    }
    if (tile.entity == OCEAN) {
        return current_entity(tile.current);
    }
    return tile.entity;
}

// Translate the independent current layer to the supplied rendering symbols.
enum entity current_entity(enum direction direction) {
    if (direction == UP) {
        return CURRENT_UP;
    } else if (direction == DOWN) {
        return CURRENT_DOWN;
    } else if (direction == LEFT) {
        return CURRENT_LEFT;
    } else if (direction == RIGHT) {
        return CURRENT_RIGHT;
    }
    return OCEAN;
}

// Report real entities, including hidden anglerfish and jellyfish on currents.
void print_statistics(struct tile board[ROWS][COLS]) {
    print_map_statistics(
        count_entity(board, TREASURE),
        count_entity(board, AIR_POCKET),
        count_entity(board, SEAWEED),
        count_entity(board, MINE),
        count_entity(board, JELLYFISH),
        count_entity(board, ANGLERFISH),
        count_entity(board, EXIT_LOCKED) + count_entity(board, EXIT_UNLOCKED)
    );
}

void print_status(struct submarine submarine) {
    print_submarine_stats(submarine.oxygen, submarine.treasure_collected,
        submarine.moves, submarine.torpedoes);
}

void print_vision_message(int deep_sea) {
    if (deep_sea) {
        printf("Deep sea mode enabled!\n");
    } else {
        printf("Deep sea mode disabled!\n");
    }
}

// The final board was already printed by the gameplay loop.
void print_result(struct game_state *game) {
    if (game->result == WON) {
        print_win_message();
    } else if (game->result == OUT_OF_OXYGEN) {
        print_oxygen_loss_message();
    } else if (game->result == SUNK) {
        print_loss_message(game->cause);
    }
    print_status(game->submarine);
}

// =============================================================================
// Shared board and direction helpers
// =============================================================================

int on_board(struct position position) {
    return position.row >= 0 && position.row < ROWS &&
        position.col >= 0 && position.col < COLS;
}

// Only horizontal motion wraps; callers validate a prospective row.
struct position next_position(
    struct position position,
    enum direction direction
) {
    if (direction == UP) {
        position.row--;
    } else if (direction == DOWN) {
        position.row++;
    } else if (direction == LEFT) {
        position.col = (position.col + COLS - 1) % COLS;
    } else if (direction == RIGHT) {
        position.col = (position.col + 1) % COLS;
    }
    return position;
}

enum direction command_direction(char command) {
    if (command == MOVE_UP) {
        return UP;
    } else if (command == MOVE_DOWN) {
        return DOWN;
    } else if (command == MOVE_LEFT) {
        return LEFT;
    } else if (command == MOVE_RIGHT) {
        return RIGHT;
    }
    return NO_DIRECTION;
}

enum direction reverse_direction(enum direction direction) {
    if (direction == UP) {
        return DOWN;
    } else if (direction == DOWN) {
        return UP;
    } else if (direction == LEFT) {
        return RIGHT;
    } else if (direction == RIGHT) {
        return LEFT;
    }
    return NO_DIRECTION;
}

int submarine_blocked(
    struct tile board[ROWS][COLS],
    struct position position
) {
    if (!on_board(position)) {
        return 1;
    }
    enum entity entity = board[position.row][position.col].entity;
    return entity == SEAWEED || entity == EXIT_LOCKED;
}

int jellyfish_can_enter(
    struct tile board[ROWS][COLS],
    struct position position
) {
    return on_board(position) &&
        board[position.row][position.col].entity == OCEAN;
}

int is_hazard(enum entity entity) {
    return entity == MINE || entity == ANGLERFISH || entity == JELLYFISH;
}

int count_entity(struct tile board[ROWS][COLS], enum entity entity) {
    int count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == entity) {
                count++;
            }
        }
    }
    return count;
}

int absolute(int number) {
    if (number < 0) {
        return -number;
    }
    return number;
}

int column_distance(int first, int second) {
    int distance = absolute(first - second);
    if (distance > COLS - distance) {
        distance = COLS - distance;
    }
    return distance;
}


// =============================================================================
// EDITABLE Provided Functions
// =============================================================================

// Given a 2D board array:
// Initialise all tile entities to OCEAN.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = OCEAN;
            board[row][col].current = NO_DIRECTION;
            board[row][col].jellyfish_direction = NO_DIRECTION;
            board[row][col].anglerfish_revealed = 0;
        }
    }
}

// Prints the game board, showing the submarine's position and entities.
void print_board(
    struct tile board[ROWS][COLS], 
    int submarine_row, 
    int submarine_col
) {
    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        print_tile_spacer();
        for (int col = 0; col < COLS; col++) {
            printf("|");
            struct tile tile = board[row][col];
            if (row == submarine_row && col == submarine_col) {
                printf("[o>");
            } else if (tile.entity == SEAWEED) {
                printf(")|(");
            } else if (tile.entity == TREASURE) {
                printf("[$]");
            } else if (tile.entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (tile.entity == EXIT_UNLOCKED) {
                printf("[^]");
            } else if (tile.entity == AIR_POCKET) {
                printf("oOo");
            } else if (tile.entity == JELLYFISH) {
                printf("(~)");
            } else if (tile.entity == ANGLERFISH) {
                printf("><>");
            } else if (tile.entity == MINE) {
                printf("(*)");
            } else if (tile.entity == HIDDEN) {
                printf("###");
            } else if (tile.entity == SEEKING_MINE) {
                printf("{*}");
            } else if (tile.entity == CURRENT_UP) {
                printf("^^^");
            } else if (tile.entity == CURRENT_DOWN) {
                printf("vvv");
            } else if (tile.entity == CURRENT_LEFT) {
                printf("<<<");
            } else if (tile.entity == CURRENT_RIGHT) {
                printf(">>>");
            } else if (tile.entity == TORPEDO) {
                printf("-*-");
            } else {
                printf("   ");
            }
        }
        printf("|\n");
    }
    print_board_line();
    return;
}

// Print out the submarine's stats
void print_submarine_stats(
    int oxygen, 
    int treasure_collected, 
    int moves, 
    int torpedoes
) {
    printf("+=======================================+\n");
    printf("|        >> SUBMARINE STATUS <<         |\n");
    printf("+=======================================+\n");
    printf("|  Oxygen     : %-23d |\n", oxygen);
    printf("|  Treasure   : %-23d |\n", treasure_collected);
    printf("|  Moves      : %-23d |\n", moves);
    printf("|  Torpedoes  : %-23d |\n", torpedoes);
    printf("+=======================================+\n");
    return;
}

// Prints how much of each entity type is left on the board
void print_map_statistics(
    int num_treasure, 
    int num_air_pockets, 
    int num_seaweed, 
    int num_mines, 
    int num_jellyfish, 
    int num_anglerfish, 
    int num_exits
) {
    printf("+=======================================+\n");
    printf("|         >> MAP STATISTICS <<          |\n");
    printf("+=======================================+\n");
    printf("|  Treasure     : %-21d |\n", num_treasure);
    printf("|  Air Pockets  : %-21d |\n", num_air_pockets);
    printf("|  Seaweed      : %-21d |\n", num_seaweed);
    printf("+---------------------------------------+\n");
    printf("|  Mines        : %-21d |\n", num_mines);
    printf("|  Jellyfish    : %-21d |\n", num_jellyfish);
    printf("|  Anglerfish   : %-21d |\n", num_anglerfish);
    printf("+---------------------------------------+\n");
    printf("|  Exits        : %-21d |\n", num_exits);
    printf("+=======================================+\n");
    return;
}

// =============================================================================
// NON-EDITABLE Provided Functions
// (Recommended to NOT EDIT the functions below this line!!)
// =============================================================================

// Prints the game win message
void print_win_message(void) {
    printf("+=======================================+\n");
    printf("|  *** ESCAPED WITH THE TREASURE! ***   |\n");
    printf("+=======================================+\n");
}

// Prints the game loss message given an entity type
void print_loss_message(enum entity entity_type) {
    printf("+=======================================+\n");
    printf("        >> Sunk by a %s! <<\n"
        , get_name_from_type(entity_type));
    printf("+=======================================+\n");
}

// Prints the game loss message for running out of oxygen
void print_oxygen_loss_message(void) {
    printf("+=======================================+\n");
    printf("        >> Out of oxygen! <<\n");
    printf("+=======================================+\n");
}

// Prints the welcome banner.
void print_welcome(void) {
    printf("=========================================\n");
    printf("    o                               o    \n");
    printf("   o o    |===================|    o o   \n");
    printf("  o   o   |    Welcome to     |   o   o  \n");
    printf("   o o    |   CS SUBMARINE    |    o o   \n");
    printf("    o     |===================|     o    \n");
    printf("  ~~~~~                           ~~~~~  \n");
    printf("=========================================\n\n");
}

// Helper function for print_board().
void print_board_header(void) {
    printf("|***  >> C S   S U B M A R I N E <<  ***|\n");
}

// Helper function for print_board().
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("===+");
    }
    printf("\n");
}

// Helper function for print_board().
void print_tile_spacer(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Returns the name of an entity given its type
char *get_name_from_type(enum entity entity_type) {
    if (entity_type == JELLYFISH) {
        return "Jellyfish";
    } else if (entity_type == ANGLERFISH) {
        return "Anglerfish";
    } else if (entity_type == MINE) {
        return "Mine";
    } else if (entity_type == SEEKING_MINE) {
        return "Seeking Mine";
    } else if (entity_type == TORPEDO) {
        return "Torpedo";
    } else {
        return "Invalid";
    }
}

// cs_chicken.c
// Written by <INSERT YOUR FULL NAME> <INSERT YOUR ZID> on <INSERT DATE>
//
// Description: COMP1511 26T1 Assignment 1 - CS Chicken Game (Stage 1-3 Complete, 样例一逻辑修正)
// Provided Libraries
#include <stdio.h>
#include <string.h>

// Provided constants
#define COLS 10
#define ROWS 10
#define INVALID_ROW -1
#define INVALID_COL -1
#define INITIAL_POINTS 0
#define DEFAULT_POINT_TARGET 20
// Stage 3 Additional Constants
#define MAX_TUNNELS 10
#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_DOWN 2
#define DIR_RIGHT 3

// Provided Enums
enum entity {
    EMPTY,
    COIN,
    TREE,
    ROAD,
    CAR_FACING_RIGHT,
    CAR_FACING_LEFT,
    HEADLIGHTS,
    KANGAROO,
    WOMBAT_TUNNEL,
    BABY_CHICKEN
};

// Stage 3 Additional Enums
typedef enum {
    STATIC_MODE,
    DRIVING_MODE,
    SCROLLING_MODE
} GameMode;

// Represents a tile on the board
struct tile {
    enum entity entity;
};

// Car info struct for sorting movement priority
struct car_info {
    int row;
    int col;
    enum entity direction;
};

// Provided Function Prototypes
void print_welcome(void);
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int curr_score,
    int target_score,
    int tunnel_id_map[ROWS][COLS]
);
void print_board_line(void);
void print_board_footer(int curr_score, int target_score);
void print_game_statistics(
    int turns_taken,
    int step_count,
    int coins,
    int score
);
void print_game_won(void);
void print_game_lost(void);

// Stage 1-3 Custom Function Prototypes
int is_position_valid(int row, int col);
int is_tile_occupied(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int row,
    int col
);
int is_move_valid(struct tile board[ROWS][COLS], int new_row, int new_col);
void process_car_turn(struct tile board[ROWS][COLS]);
void scroll_board(
    struct tile board[ROWS][COLS],
    int original_coins[ROWS][COLS],
    int tunnel_id_map[ROWS][COLS]
);
void find_tunnel_exit(
    int tunnel_id,
    int curr_row,
    int curr_col,
    int tunnel_id_map[ROWS][COLS],
    int *exit_row,
    int *exit_col
);
int check_car_collision(struct tile board[ROWS][COLS], int player_row, int player_col);
void trigger_game_loss(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int curr_score,
    int target_score,
    int turns_taken,
    int step_count,
    int coins_collected,
    int tunnel_id_map[ROWS][COLS]
);

// Direction delta arrays (w, a, s, d)
const int dr[] = {-1, 0, 1, 0};
const int dc[] = {0, -1, 0, 1};

int main(void) {
    print_welcome();
    struct tile board[ROWS][COLS];
    initialise_board(board);

    // Core Game Variables
    int player_row, player_col;
    int curr_score = INITIAL_POINTS;
    int target_score = DEFAULT_POINT_TARGET;
    int turns_taken = 0;
    int step_count = 0;
    int coins_collected = 0;
    GameMode game_mode = STATIC_MODE;

    // Stage 3.3 Scrolling Variables
    int original_coins[ROWS][COLS] = {0};

    // Stage 3.4 Wombat Tunnel Variables
    int tunnel_id_map[ROWS][COLS];
    int tunnel_count = 0;
    memset(tunnel_id_map, -1, sizeof(tunnel_id_map));

    // ==============================================
    // Stage 1: Setup Phase
    // ==============================================
    printf("============== Setup Phase ==============\n");

    // Stage 1.1 & 1.2: Spawn and validate player starting position
    do {
        printf("Enter the starting position: ");
        scanf("%d %d", &player_row, &player_col);
        if (!is_position_valid(player_row, player_col)) {
            printf("Penny the Chicken cannot start here!\n");
        }
    } while (!is_position_valid(player_row, player_col));
    print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);

    // Stage 1.3-1.5 & Stage 2.4 & Stage 3 Setup Commands
    printf("Enter setup commands:\n");
    char cmd;
    while (scanf(" %c", &cmd) != EOF) {
        // Stage 2 & 3: End Setup Phase Commands
        if (cmd == 'e' || cmd == 'd' || cmd == 's') {
            if (cmd == 'd') {
                game_mode = DRIVING_MODE;
            } else if (cmd == 's') {
                game_mode = SCROLLING_MODE;
            } else {
                game_mode = STATIC_MODE;
            }
            break;
        }

        // Stage 1.3: COIN & TREE Commands
        if (cmd == 'c' || cmd == 't') {
            int row, col;
            scanf("%d %d", &row, &col);

            if (!is_position_valid(row, col)) {
                printf("Invalid location: position is not on map!\n");
                continue;
            }
            if (is_tile_occupied(board, player_row, player_col, row, col)) {
                printf("Invalid location: tile is occupied!\n");
                continue;
            }

            if (cmd == 'c') {
                board[row][col].entity = COIN;
                original_coins[row][col] = 1;
            } else {
                board[row][col].entity = TREE;
            }
        }
        // Stage 1.4: ROAD Command
        else if (cmd == 'r') {
            int row;
            scanf("%d", &row);

            if (!is_position_valid(row, 0)) {
                printf("Invalid location: position is not on map!\n");
                continue;
            }

            int has_tree = 0;
            int has_invalid = 0;
            for (int col = 0; col < COLS; col++) {
                if (row == player_row && col == player_col) {
                    has_invalid = 1;
                    break;
                }
                if (board[row][col].entity == TREE) {
                    has_tree = 1;
                } else if (board[row][col].entity != EMPTY) {
                    has_invalid = 1;
                    break;
                }
            }

            if (has_invalid) {
                printf("Invalid location: road cannot be built.\n");
                continue;
            }
            if (has_tree) {
                printf("Deforesting.\n");
            }

            for (int col = 0; col < COLS; col++) {
                board[row][col].entity = ROAD;
                original_coins[row][col] = 0;
            }
        }
        // Stage 1.4: CAR Command
        else if (cmd == 'v') {
            int row, col;
            char dir;
            scanf("%d %d %c", &row, &col, &dir);

            if (!is_position_valid(row, col)) {
                printf("Invalid location: position is not on map!\n");
                continue;
            }
            if (board[row][col].entity != ROAD && board[row][col].entity != HEADLIGHTS) {
                printf("Invalid location: car must be on a road!\n");
                continue;
            }

            enum entity car_type;
            int headlight_col;
            if (dir == 'r') {
                car_type = CAR_FACING_RIGHT;
                headlight_col = col + 1;
            } else {
                car_type = CAR_FACING_LEFT;
                headlight_col = col - 1;
            }

            // Place the car on the board
            board[row][col].entity = car_type;

            // 严格检查：车灯位置只要是车辆，就不生成车灯
            if (is_position_valid(row, headlight_col)) {
                enum entity headlight_tile = board[row][headlight_col].entity;
                int is_car_present = (headlight_tile == CAR_FACING_LEFT || headlight_tile == CAR_FACING_RIGHT);
                if (!is_car_present) {
                    board[row][headlight_col].entity = HEADLIGHTS;
                }
            }
        }
        // Stage 2.4: Target Score Command
        else if (cmd == 'x') {
            int new_target;
            scanf("%d", &new_target);
            if (new_target < 1 || new_target > 99) {
                printf("Target must be between 1 and 99 inclusive.\n");
            } else {
                target_score = new_target;
            }
        }
        // Stage 3.4: Wombat Tunnel Command
        else if (cmd == 'w') {
            char sub_cmd;
            scanf(" %c", &sub_cmd);
            if (sub_cmd == 't') {
                int row1, col1, row2, col2;
                scanf("%d %d %d %d", &row1, &col1, &row2, &col2);

                // Check tunnel limit
                if (tunnel_count >= MAX_TUNNELS) {
                    printf("Invalid feature: too many tunnels!\n");
                    continue;
                }
                // Check positions are valid
                if (!is_position_valid(row1, col1) || !is_position_valid(row2, col2)) {
                    printf("Invalid location: wombat couldn't dig a tunnel here!\n");
                    continue;
                }
                // Check positions are not occupied
                if (is_tile_occupied(board, player_row, player_col, row1, col1) ||
                    is_tile_occupied(board, player_row, player_col, row2, col2)) {
                    printf("Invalid location: wombat couldn't dig a tunnel here!\n");
                    continue;
                }

                // Create tunnel
                board[row1][col1].entity = WOMBAT_TUNNEL;
                board[row2][col2].entity = WOMBAT_TUNNEL;
                tunnel_id_map[row1][col1] = tunnel_count;
                tunnel_id_map[row2][col2] = tunnel_count;
                tunnel_count++;
            }
        }
    }

    // ==============================================
    // End Setup Phase, Enter Gameplay Phase
    // ==============================================
    print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);
    printf("============ Gameplay Phase =============\n");

    // ==============================================
    // Stage 2 & 3: Gameplay Loop
    // ==============================================
    char gameplay_cmd;
    while (scanf(" %c", &gameplay_cmd) != EOF) {
        // Handle quit command
        if (gameplay_cmd == 'q') {
            printf("============= Quitting Game =============\n");
            return 0;
        }

        // Handle statistics print command (does not count as turn)
        if (gameplay_cmd == 'p') {
            print_game_statistics(turns_taken, step_count, coins_collected, curr_score);
            continue;
        }

        // ==============================================
        // Player Turn Processing
        // ==============================================
        int prev_player_row = player_row; // 移动前的行号，用于滚动判断
        int prev_player_col = player_col; // 移动前的列号，用于回退
        int move_dir = -1;
        int is_rest = 0;
        int moved_up = 0;
        int attempted_up_from_row0 = 0;

        // Parse movement command
        if (gameplay_cmd == 'w') {
            move_dir = DIR_UP;
            moved_up = 1;
            if (player_row == 0) {
                attempted_up_from_row0 = 1;
            }
        } else if (gameplay_cmd == 'a') {
            move_dir = DIR_LEFT;
        } else if (gameplay_cmd == 's') {
            move_dir = DIR_DOWN;
        } else if (gameplay_cmd == 'd') {
            move_dir = DIR_RIGHT;
        } else if (gameplay_cmd == 'R') {
            is_rest = 1;
        } else {
            // Ignore unrecognized commands
            continue;
        }

        // Increment turn count (all w/a/s/d/R count as a turn)
        turns_taken++;
        int move_successful = 0;

        // Process movement if not resting
        if (!is_rest) {
            // [核心修复] 完全重写移动逻辑，严格按照样例一流程
            if (attempted_up_from_row0 && game_mode == SCROLLING_MODE) {
                // 情况1：滚动模式下从第0行向上移动
                if (board[9][player_col].entity != TREE) {
                    // 1. 先执行滚动
                    scroll_board(board, original_coins, tunnel_id_map);
                    move_successful = 1;
                    step_count++;

                    // 2. 滚动后玩家位置不变，仍在(0, col)
                    // 3. 检查滚动后的(0, col)是否是隧道
                    if (board[player_row][player_col].entity == WOMBAT_TUNNEL) {
                        int tid = tunnel_id_map[player_row][player_col];
                        int exit_r = -1, exit_c = -1;
                        find_tunnel_exit(tid, player_row, player_col, tunnel_id_map, &exit_r, &exit_c);

                        // 4. 计算落地位置：隧道出口 + 移动方向（上方一格）
                        int land_r = exit_r + dr[move_dir];
                        int land_c = exit_c + dc[move_dir];

                        if (is_position_valid(land_r, land_c) && board[land_r][land_c].entity != TREE) {
                            // 5. 有效传送，移动到落地位置
                            player_row = land_r;
                            player_col = land_c;
                        }
                        // 如果传送无效，玩家留在(0, col)
                    }

                    // 6. 处理金币收集
                    if (board[player_row][player_col].entity == COIN) {
                        board[player_row][player_col].entity = EMPTY;
                        curr_score += 5;
                        coins_collected++;
                    }
                }
            } else {
                // 情况2：正常移动逻辑（非第0行向上或非滚动模式）
                int new_row = player_row + dr[move_dir];
                int new_col = player_col + dc[move_dir];

                if (is_move_valid(board, new_row, new_col)) {
                    move_successful = 1;
                    player_row = new_row;
                    player_col = new_col;
                    step_count++;

                    // 处理隧道传送
                    if (board[player_row][player_col].entity == WOMBAT_TUNNEL) {
                        int tid = tunnel_id_map[player_row][player_col];
                        int exit_row, exit_col;
                        find_tunnel_exit(tid, player_row, player_col, tunnel_id_map, &exit_row, &exit_col);

                        // 计算落地位置：隧道出口 + 移动方向
                        int land_row = exit_row + dr[move_dir];
                        int land_col = exit_col + dc[move_dir];

                        if (!is_position_valid(land_row, land_col) || board[land_row][land_col].entity == TREE) {
                            // 落地无效，回退
                            move_successful = 0;
                            player_row = prev_player_row;
                            player_col = prev_player_col;
                            step_count--;
                        } else {
                            // 有效传送
                            player_row = land_row;
                            player_col = land_col;
                        }
                    }

                    // 处理金币收集
                    if (board[player_row][player_col].entity == COIN) {
                        board[player_row][player_col].entity = EMPTY;
                        curr_score += 5;
                        coins_collected++;
                    }
                }
            }
        }

        // ==============================================
        // Post-Player-Turn Checks
        // ==============================================
        // 胜利条件检查
        if (curr_score >= target_score) {
            print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);
            print_game_statistics(turns_taken, step_count, coins_collected, curr_score);
            print_game_won();
            return 0;
        }

        // 玩家移动后碰撞检查
        if (check_car_collision(board, player_row, player_col)) {
            trigger_game_loss(
                board, player_row, player_col, curr_score, target_score,
                turns_taken, step_count, coins_collected, tunnel_id_map
            );
            return 0;
        }

        // ==============================================
        // Car Turn (Driving & Scrolling Modes)
        // ==============================================
        if (game_mode == DRIVING_MODE || game_mode == SCROLLING_MODE) {
            process_car_turn(board);

            // 汽车移动后碰撞检查
            if (check_car_collision(board, player_row, player_col)) {
                trigger_game_loss(
                    board, player_row, player_col, curr_score, target_score,
                    turns_taken, step_count, coins_collected, tunnel_id_map
                );
                return 0;
            }
        }

        // ==============================================
        // 滚动处理 (Scrolling Mode)
        // ==============================================
        if (game_mode == SCROLLING_MODE) {
            int scroll_triggered = 0;

            // 正常滚动触发：移动成功、方向向上、且移动前在0-6行（排除第0行向上的特殊情况）
            if (moved_up && move_successful && !attempted_up_from_row0 && prev_player_row <= 6) {
                scroll_board(board, original_coins, tunnel_id_map);
                scroll_triggered = 1;
                // 所有行下移，玩家位置同步下移
                player_row += 1;
            }

            // 滚动后检查
            if (scroll_triggered) {
                // 滚动后碰撞检查
                if (check_car_collision(board, player_row, player_col)) {
                    trigger_game_loss(
                        board, player_row, player_col, curr_score, target_score,
                        turns_taken, step_count, coins_collected, tunnel_id_map
                    );
                    return 0;
                }
                // 滚动后胜利检查
                if (curr_score >= target_score) {
                    print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);
                    print_game_statistics(turns_taken, step_count, coins_collected, curr_score);
                    print_game_won();
                    return 0;
                }
            }
        }

        // 回合结束，打印更新后的棋盘
        print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);
    }

    return 0;
}

// ==============================================
// Custom Helper Functions Implementation
// ==============================================

// Checks if a given (row, col) is within the board boundaries
int is_position_valid(int row, int col) {
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS);
}

// Checks if a given (row, col) is occupied by the player or a non-EMPTY entity
int is_tile_occupied(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int row,
    int col
) {
    if (row == player_row && col == player_col) {
        return 1;
    }
    if (board[row][col].entity != EMPTY) {
        return 1;
    }
    return 0;
}

// Checks if a player move to (new_row, new_col) is valid
int is_move_valid(struct tile board[ROWS][COLS], int new_row, int new_col) {
    if (!is_position_valid(new_row, new_col)) {
        return 0;
    }
    if (board[new_row][new_col].entity == TREE) {
        return 0;
    }
    return 1;
}

// Checks if player is on the same tile as a car
int check_car_collision(struct tile board[ROWS][COLS], int player_row, int player_col) {
    enum entity e = board[player_row][player_col].entity;
    return (e == CAR_FACING_LEFT || e == CAR_FACING_RIGHT);
}

// Triggers the game loss sequence
void trigger_game_loss(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int curr_score,
    int target_score,
    int turns_taken,
    int step_count,
    int coins_collected,
    int tunnel_id_map[ROWS][COLS]
) {
    print_board(board, player_row, player_col, curr_score, target_score, tunnel_id_map);
    print_game_statistics(turns_taken, step_count, coins_collected, curr_score);
    print_game_lost();
}

// Stage 3.2: Process car movement for Car Turn
void process_car_turn(struct tile board[ROWS][COLS]) {
    struct car_info cars[ROWS * COLS];
    int car_count = 0;

    // Step 1: Collect all cars on the board
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            enum entity e = board[row][col].entity;
            if (e == CAR_FACING_LEFT || e == CAR_FACING_RIGHT) {
                cars[car_count].row = row;
                cars[car_count].col = col;
                cars[car_count].direction = e;
                car_count++;
            }
        }
    }

    // Step 2: Sort cars by movement priority
    // Right-facing cars: higher column first (move right to left)
    // Left-facing cars: lower column first (move left to right)
    for (int i = 0; i < car_count; i++) {
        for (int j = i + 1; j < car_count; j++) {
            int swap = 0;
            if (cars[i].direction == CAR_FACING_RIGHT && cars[j].direction == CAR_FACING_RIGHT) {
                if (cars[i].col < cars[j].col) swap = 1;
            } else if (cars[i].direction == CAR_FACING_LEFT && cars[j].direction == CAR_FACING_LEFT) {
                if (cars[i].col > cars[j].col) swap = 1;
            }
            if (swap) {
                struct car_info temp = cars[i];
                cars[i] = cars[j];
                cars[j] = temp;
            }
        }
    }

    // Step 3: Clear all old headlights (convert to road)
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == HEADLIGHTS) {
                board[row][col].entity = ROAD;
            }
        }
    }

    // Step 4: Process each car's movement
    for (int i = 0; i < car_count; i++) {
        int row = cars[i].row;
        int col = cars[i].col;
        enum entity dir = cars[i].direction;

        // Skip if car was already moved/overwritten by another car
        if (board[row][col].entity != dir) {
            continue;
        }

        // Calculate destination
        int dest_col = col;
        if (dir == CAR_FACING_RIGHT) {
            dest_col = col + 1;
        } else {
            dest_col = col - 1;
        }

        // Check if destination is valid
        int dest_valid = 0;
        if (is_position_valid(row, dest_col)) {
            enum entity dest_e = board[row][dest_col].entity;
            if (dest_e == ROAD) {
                dest_valid = 1;
            }
        }

        if (dest_valid) {
            // Move car to destination
            board[row][dest_col].entity = dir;
            board[row][col].entity = ROAD;
            // Update car position for headlight generation
            cars[i].col = dest_col;
        } else {
            // Reverse car direction
            if (dir == CAR_FACING_RIGHT) {
                board[row][col].entity = CAR_FACING_LEFT;
                cars[i].direction = CAR_FACING_LEFT;
            } else {
                board[row][col].entity = CAR_FACING_RIGHT;
                cars[i].direction = CAR_FACING_RIGHT;
            }
        }
    }

    // Step 5: Generate new headlights for all cars
    for (int i = 0; i < car_count; i++) {
        int row = cars[i].row;
        int col = cars[i].col;
        enum entity dir = cars[i].direction;

        // Skip if car was overwritten
        if (board[row][col].entity != dir) {
            continue;
        }

        int headlight_col = col;
        if (dir == CAR_FACING_RIGHT) {
            headlight_col = col + 1;
        } else {
            headlight_col = col - 1;
        }

        // 严格检查：车灯位置只要是车辆，就不生成车灯
        if (is_position_valid(row, headlight_col)) {
            enum entity e = board[row][headlight_col].entity;
            if (e != CAR_FACING_LEFT && e != CAR_FACING_RIGHT) {
                board[row][headlight_col].entity = HEADLIGHTS;
            }
        }
    }
}

// Stage 3.3: Scroll the board and restore coins
void scroll_board(
    struct tile board[ROWS][COLS],
    int original_coins[ROWS][COLS],
    int tunnel_id_map[ROWS][COLS]
) {
    // Save the bottom row (row9)
    struct tile temp_board_row[COLS];
    int temp_coins_row[COLS];
    int temp_tunnel_row[COLS];
    for (int col = 0; col < COLS; col++) {
        temp_board_row[col] = board[9][col];
        temp_coins_row[col] = original_coins[9][col];
        temp_tunnel_row[col] = tunnel_id_map[9][col];
    }

    // Shift all rows down by 1
    for (int row = 8; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            board[row + 1][col] = board[row][col];
            original_coins[row + 1][col] = original_coins[row][col];
            tunnel_id_map[row + 1][col] = tunnel_id_map[row][col];
        }
    }

    // Move saved bottom row to top (row0)
    for (int col = 0; col < COLS; col++) {
        board[0][col] = temp_board_row[col];
        original_coins[0][col] = temp_coins_row[col];
        tunnel_id_map[0][col] = temp_tunnel_row[col];
    }

    // Restore collected coins on the new top row
    for (int col = 0; col < COLS; col++) {
        if (original_coins[0][col] == 1 && board[0][col].entity == EMPTY) {
            board[0][col].entity = COIN;
        }
    }
}

// Stage 3.4: Find the other exit of a tunnel
void find_tunnel_exit(
    int tunnel_id,
    int curr_row,
    int curr_col,
    int tunnel_id_map[ROWS][COLS],
    int *exit_row,
    int *exit_col
) {
    *exit_row = -1;
    *exit_col = -1;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (tunnel_id_map[row][col] == tunnel_id && !(row == curr_row && col == curr_col)) {
                *exit_row = row;
                *exit_col = col;
                return;
            }
        }
    }
}

// ==============================================
// Modified Provided Helper Functions
// ==============================================

// Updated print_board for Stage 3.1 (shocked chicken) and 3.4 (tunnel display)
void print_board(
    struct tile board[ROWS][COLS],
    int player_row,
    int player_col,
    int curr_score,
    int target_score,
    int tunnel_id_map[ROWS][COLS]
) {
    print_board_line();
    printf("|          C S   C H I C K E N          |\n");
    print_board_line();

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            printf("|");
            if (row == player_row && col == player_col) {
                // Shocked chicken if on headlights OR on a car
                enum entity e = board[row][col].entity;
                if (e == HEADLIGHTS || e == CAR_FACING_LEFT || e == CAR_FACING_RIGHT) {
                    printf("0_0");
                } else {
                    printf("^_^");
                }
            } else if (board[row][col].entity == EMPTY) {
                printf("   ");
            } else if (board[row][col].entity == COIN) {
                printf(" c ");
            } else if (board[row][col].entity == TREE) {
                printf(" T ");
            } else if (board[row][col].entity == ROAD) {
                printf("___");
            } else if (board[row][col].entity == CAR_FACING_RIGHT) {
                printf("[_0");
            } else if (board[row][col].entity == CAR_FACING_LEFT) {
                printf("0_]");
            } else if (board[row][col].entity == HEADLIGHTS) {
                printf("###");
            } else if (board[row][col].entity == WOMBAT_TUNNEL) {
                // Stage 3.4: Display tunnel ID
                printf("(%d)", tunnel_id_map[row][col]);
            } else {
                printf("   ");
            }
        }
        printf("|\n");
        print_board_line();
    }
    print_board_footer(curr_score, target_score);
    printf("\n");
}

// ==============================================
// Unmodified Provided Helper Functions
// ==============================================
void print_welcome(void) {
    printf(
        "_________________________________________   \n"
        "   \\\\                               \\\\  \n"
        "   (o>   !!    Welcome to    !!     (o>     \n"
        "\\\\_//)         CS CHICKEN        \\\\_//) \n"
        " \\_/_)                            \\_/_)   \n"
        "  _|_                              _|_      \n"
        "_________________________________________   \n\n"
    );
}

void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = EMPTY;
        }
    }
}

void print_game_statistics(
    int turns_taken,
    int step_count,
    int coins,
    int score
) {
    printf(
        "============ Game Statistics ============\n"
        "          Turns taken: %d\n"
        "           Step count: %d\n"
        "      Coins Collected: %d\n"
        "                Score: %d\n",
        turns_taken, step_count, coins, score
    );
}

void print_game_won(void) {
    printf(
        "_________________________________________   \n"
        "       Penny the Chicken is happy!          \n"
        "   \\\\                               \\\\  \n"
        "   (o>   !!    Thank  you    !!     (o>     \n"
        "\\\\_//)        for playing!       \\\\_//) \n"
        " \\_/_)                            \\_/_)   \n"
        "  _|_                              _|_      \n"
        "_________________________________________   \n\n"
    );
}

void print_game_lost(void) {
    printf(
        "_________________________________________   \n"
        "   \\\\                               \\\\  \n"
        "   (x>    !!      Game      !!      (x>     \n"
        "\\\\_//)             Over          \\\\_//) \n"
        " \\_/_)                            \\_/_)   \n"
        "  _|_                              _|_      \n"
        "_________________________________________   \n\n"
    );
}

void print_board_footer(int curr_score, int target_score) {
    printf("Score: %-3d                     ", curr_score);
    printf("Target: %-2d", target_score);
}

void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}
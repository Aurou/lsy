// Assignment 2 26T3 COMP1511: CS Alchemy
// cs_alchemy.c
//
// Written by <YOUR-NAME>, <YOUR-ZID>
// on <TODAYS-DATE>
//
// Linked-list potion simulator, implementing Stages 1, 2 and 3.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

#include "cs_alchemy.h"

// Stage 1.1
// Function to create the Apothecary
// Params:
//      name - the name of the apothecary
// Returns: a pointer to the apothecary
struct apothecary *create_apothecary(char name[MAX_SIZE]) {

    struct apothecary *apothecary = malloc(sizeof(struct apothecary));
    strcpy(apothecary->name, name);
    apothecary->wizards = NULL;
    apothecary->cauldrons = NULL;
    return apothecary;
}

// Stage 1.1
// Function to create a wizard
// Params:
//      name - the name of the wizard
//      rank - the rank the wizard has attained
// Returns: a pointer to the wizard
struct wizard *create_wizard(char name[MAX_SIZE], enum wizard_rank rank) {

    struct wizard *wizard = malloc(sizeof(struct wizard));
    strcpy(wizard->name, name);
    wizard->rank = rank;
    wizard->potions = NULL;
    wizard->next = NULL;
    return wizard;
}

// Stage 1.1
// Function to create a cauldron
// Returns: a pointer to the cauldron
struct cauldron *create_cauldron(void) {
    struct cauldron *cauldron = malloc(sizeof(struct cauldron));
    cauldron->assigned_wizard_name[0] = '\0';
    cauldron->status = READY;
    cauldron->brew_potency = 0;
    cauldron->required_rank = NOVICE;
    cauldron->turns_remaining = 0;
    cauldron->ingredients = NULL;
    cauldron->next = NULL;
    return cauldron;
}

// Stage 1.1
// Function to create an ingredient
// Params:
//      type - the ingredient_type of the ingredient
//      effect - the effect_type of the ingredient
//      potency - the ingredients numerical strength
// Returns: a pointer to the ingredient
struct ingredient *create_ingredient(
    enum ingredient_type type,
    enum ingredient_effect effect,
    int potency
) {

    struct ingredient *ingredient = malloc(sizeof(struct ingredient));
    ingredient->type = type;
    ingredient->effect = effect;
    ingredient->potency = potency;
    ingredient->next = NULL;
    return ingredient;
}

// Stage 1.1
// Function to create a potion
// Params:
//      name - the name of potion
// Returns: a pointer to the potion
struct potion *create_potion(char name[MAX_SIZE]) {

    struct potion *potion = malloc(sizeof(struct potion));
    strcpy(potion->name, name);
    potion->brew_potency = 0;
    potion->ingredients = NULL;
    potion->next = NULL;
    return potion;
}

// Stage 1.2
// Function to run the main command loop for the program
// Params:
//      apothecary - a pointer to the apothecary
// Returns: None
void command_loop(struct apothecary *apothecary) {

    char command;
    printf("Enter command: ");
    while (scanf(" %c", &command) == 1 && command != 'q') {
        execute_command(apothecary, command);
        printf("Enter command: ");
    }
    free_apothecary(apothecary);
}

// Stage 1.3
// Function to add a wizard
// Params:
//      apothecary - a pointer to the apothecary
// Returns: None
void add_wizard(struct apothecary *apothecary) {

    char name[MAX_SIZE];
    scan_name(name);
    enum wizard_rank rank = scan_wizard_rank();
    if (rank == INVALID_RANK) {
        printf("Error: Invalid wizard rank.\n");
        return;
    }
    if (find_wizard(apothecary, name) != NULL) {
        printf("Error: Wizard %s already exists.\n", name);
        return;
    }
    struct wizard *wizard = create_wizard(name, rank);
    wizard->next = apothecary->wizards;
    apothecary->wizards = wizard;
    printf("Wizard: '%s' added!\n", name);
}

// Stage 1.3
// Function to add a cauldron, assigned to an existing wizard
// Params:
//      apothecary - a pointer to the apothecary
// Returns: None
void add_cauldron(struct apothecary *apothecary) {

    char name[MAX_SIZE];
    scan_name(name);
    enum wizard_rank rank = scan_wizard_rank();
    struct wizard *wizard = find_wizard(apothecary, name);
    if (rank == INVALID_RANK) {
        printf("Error: Invalid wizard rank.\n");
        return;
    }
    if (wizard == NULL) {
        printf("Error: No wizard with name %s.\n", name);
        return;
    }
    if (wizard->rank < rank) {
        printf("Error: Wizard %s is not of a high enough rank.\n", name);
        return;
    }
    struct cauldron *cauldron = create_cauldron();
    strcpy(cauldron->assigned_wizard_name, name);
    cauldron->required_rank = rank;
    if (apothecary->cauldrons == NULL) {
        apothecary->cauldrons = cauldron;
    } else {
        struct cauldron *tail = apothecary->cauldrons;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = cauldron;
    }
    printf("Cauldron: assigned to '%s'!\n", name);
}

// Stage 1.3
// Function to add an ingredient to a cauldron
// Params:
//      apothecary - a pointer to the apothecary
// Returns: None
void add_ingredient(struct apothecary *apothecary) {

    int number;
    scanf("%d", &number);
    enum ingredient_type type = scan_ingredient_type();
    enum ingredient_effect effect = scan_ingredient_effect();
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL || !can_add_ingredients(cauldron)) {
        return;
    }
    if (!valid_ingredient(type, effect)) {
        return;
    }
    struct ingredient *ingredient = create_ingredient(
        type, effect, starting_potency(type));
    append_ingredient(cauldron, ingredient);
    cauldron->brew_potency += ingredient->potency;
    printf("Ingredient: %s %s added to cauldron %d!\n",
        ingredient_type_to_string(type), ingredient_effect_to_string(effect),
        number);
}

// Stage 1.4
// Function to print out the apothecary
// Params:
//      apothecary - a pointer to the alchemist's apothecary
// Returns: None
void print_apothecary(struct apothecary *apothecary) {

    printf("=-=-=-=-=-=-=-=[Apothecary: %s]=-=-=-=-=-=-=-=\n",
        apothecary->name);
    printf("Wizards:\n");
    if (apothecary->wizards == NULL) {
        printf("  No wizards yet!\n");
    }
    struct wizard *wizard = apothecary->wizards;
    while (wizard != NULL) {
        printf("  %s - %s\n", wizard->name,
            wizard_rank_to_string(wizard->rank));
        print_potion_list(wizard->potions);
        wizard = wizard->next;
    }
    printf("\nCauldrons:\n");
    if (apothecary->cauldrons == NULL) {
        printf("  No cauldrons yet!\n\n");
    }
    struct cauldron *cauldron = apothecary->cauldrons;
    int number = 1;
    while (cauldron != NULL) {
        print_cauldron(cauldron, number);
        cauldron = cauldron->next;
        number++;
    }
    if (apothecary->cauldrons != NULL) {
        printf("\n");
    }
}


// Add your function definitions below this line

// Dispatch a complete command, leaving the next command unread.
void execute_command(struct apothecary *apothecary, char command) {
    if (command == '?') {
        print_usage();
    } else if (command == 'a') {
        add_command(apothecary);
    } else if (command == '*') {
        print_apothecary(apothecary);
    } else if (command == 'c') {
        inspect_cauldron(apothecary);
    } else if (command == 'i') {
        insert_ingredient(apothecary);
    } else if (command == 'b') {
        bottle_potion(apothecary);
    } else if (command == 'o') {
        reorder_cauldron(apothecary);
    } else if (command == 'r') {
        remove_command(apothecary);
    } else if (command == 'l') {
        light_cauldron(apothecary);
    } else if (command == 's') {
        simmer_cauldron(apothecary);
    } else {
        printf("ERROR: Invalid command.\n");
    }
}

// Read the kind of object to add.
void add_command(struct apothecary *apothecary) {
    char kind;
    scanf(" %c", &kind);
    if (kind == 'w') {
        add_wizard(apothecary);
    } else if (kind == 'c') {
        add_cauldron(apothecary);
    } else if (kind == 'i') {
        add_ingredient(apothecary);
    } else {
        printf("ERROR: Invalid command.\n");
    }
}

// Read the kind of object to remove.
void remove_command(struct apothecary *apothecary) {
    char kind;
    scanf(" %c", &kind);
    if (kind == 'p') {
        remove_potion(apothecary);
    } else if (kind == 'c') {
        remove_cauldron(apothecary);
    } else if (kind == 'w') {
        remove_wizard(apothecary);
    } else {
        printf("ERROR: Invalid command.\n");
    }
}

// Return the wizard with this exact name, or NULL.
struct wizard *find_wizard(struct apothecary *apothecary, char *name) {
    struct wizard *wizard = apothecary->wizards;
    while (wizard != NULL && strcmp(wizard->name, name) != 0) {
        wizard = wizard->next;
    }
    return wizard;
}

// Find a cauldron by its current 1-based position; report invalid positions.
struct cauldron *find_cauldron(struct apothecary *apothecary, int number) {
    struct cauldron *cauldron = apothecary->cauldrons;
    int position = 1;
    while (cauldron != NULL && position < number) {
        cauldron = cauldron->next;
        position++;
    }
    if (number < 1 || cauldron == NULL) {
        printf("Error: Cauldron %d does not exist.\n", number);
        return NULL;
    }
    return cauldron;
}

// Count the nodes in an ingredient list.
int ingredient_count(struct ingredient *head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

// Count the nodes in a potion list.
int potion_count(struct potion *head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

// Get the initial potency associated with a valid ingredient type.
int starting_potency(enum ingredient_type type) {
    if (type == MUSHROOM || type == CRYSTAL) {
        return STRONG_POTENCY;
    }
    return LIGHT_POTENCY;
}

// Return whether an ingredient type prefers the TOP region.
int prefers_top(enum ingredient_type type) {
    return type == CRYSTAL || type == FLOWER;
}

// Compare the preferred region with a 1-based depth in the current list.
int is_misplaced(struct ingredient *ingredient, int depth, int count) {
    int in_top = depth <= count / 2;
    return in_top != prefers_top(ingredient->type);
}

// Check ingredient enums in the required error order.
int valid_ingredient(enum ingredient_type type, enum ingredient_effect effect) {
    if (type == INVALID_TYPE) {
        printf("Error: Invalid ingredient type.\n");
        return 0;
    }
    if (effect == INVALID_EFFECT) {
        printf("Error: Invalid ingredient effect.\n");
        return 0;
    }
    return 1;
}

// Ingredients may only be added before brewing starts.
int can_add_ingredients(struct cauldron *cauldron) {
    if (cauldron->status != READY) {
        printf("Error: Ingredients can only be added to a cauldron "
            "before brewing.\n");
        return 0;
    }
    return 1;
}

// Bottling, reordering and removal must wait until simmering has ended.
int not_simmering(struct cauldron *cauldron, int number) {
    if (cauldron->status == SIMMERING) {
        printf("Error: Cauldron %d is still simmering.\n", number);
        return 0;
    }
    return 1;
}

// Append an existing ingredient node to a cauldron.
void append_ingredient(
    struct cauldron *cauldron,
    struct ingredient *ingredient
) {
    if (cauldron->ingredients == NULL) {
        cauldron->ingredients = ingredient;
    } else {
        struct ingredient *tail = cauldron->ingredients;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = ingredient;
    }
}

// Append an existing potion node to a wizard's list.
void append_potion(struct wizard *wizard, struct potion *potion) {
    if (wizard->potions == NULL) {
        wizard->potions = potion;
    } else {
        struct potion *tail = wizard->potions;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = potion;
    }
}

// Stage 2.1: inspect the selected cauldron.
void inspect_cauldron(struct apothecary *apothecary) {
    int number;
    scanf("%d", &number);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron != NULL) {
        print_inspection(cauldron, number);
    }
}

// Display regions and calculate placement without modifying the list.
void print_inspection(struct cauldron *cauldron, int number) {
    int count = ingredient_count(cauldron->ingredients);
    int misplaced = 0;
    printf("=-=-= Cauldron %d =-=-= [%s]\n", number,
        brew_status_to_string(cauldron->status));
    printf("  Wizard: %s | Required rank: %s | Brew potency: %d\n\n",
        cauldron->assigned_wizard_name,
        wizard_rank_to_string(cauldron->required_rank), cauldron->brew_potency);
    printf("  ------------------ TOP ------------------\n");
    if (count == 0) {
        printf("    No ingredients yet\n");
    }
    struct ingredient *ingredient = cauldron->ingredients;
    int depth = 1;
    while (ingredient != NULL) {
        if (depth == count / 2 + 1) {
            printf("  ----------------- BOTTOM ----------------\n");
        }
        printf("  %2d. %s ::::: %s (%d)", depth,
            ingredient_type_to_string(ingredient->type),
            ingredient_effect_to_string(ingredient->effect),
            ingredient->potency);
        if (is_misplaced(ingredient, depth, count)) {
            printf("   < misplaced");
            misplaced++;
        }
        printf("\n");
        ingredient = ingredient->next;
        depth++;
    }
    if (count == 0) {
        printf("  ----------------- BOTTOM ----------------\n");
        printf("    No ingredients yet\n");
    }
    printf("  -----------------------------------------\n");
    printf("\n  %d of %d ingredients are misplaced.\n\n", misplaced, count);
}

// Stage 2.2: validate the position before allocating an ingredient.
void insert_ingredient(struct apothecary *apothecary) {
    int number;
    int depth;
    scanf("%d %d", &number, &depth);
    enum ingredient_type type = scan_ingredient_type();
    enum ingredient_effect effect = scan_ingredient_effect();
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL || !can_add_ingredients(cauldron)) {
        return;
    }
    if (depth < 1 || depth > ingredient_count(cauldron->ingredients) + 1) {
        printf("Error: Invalid depth.\n");
        return;
    }
    if (!valid_ingredient(type, effect)) {
        return;
    }
    struct ingredient *ingredient = create_ingredient(
        type, effect, starting_potency(type));
    insert_at_depth(cauldron, ingredient, depth);
    cauldron->brew_potency += ingredient->potency;
    printf("Ingredient: %s %s inserted into cauldron %d at depth %d!\n",
        ingredient_type_to_string(type), ingredient_effect_to_string(effect),
        number, depth);
}

// Insert a node at an already validated 1-based depth.
void insert_at_depth(struct cauldron *cauldron, struct ingredient *ingredient,
    int depth) {
    if (depth == 1) {
        ingredient->next = cauldron->ingredients;
        cauldron->ingredients = ingredient;
    } else {
        struct ingredient *previous = cauldron->ingredients;
        int position = 1;
        while (position < depth - 1) {
            previous = previous->next;
            position++;
        }
        ingredient->next = previous->next;
        previous->next = ingredient;
    }
}

// Stage 2.3: transfer ownership of the entire ingredient list into a potion.
void bottle_potion(struct apothecary *apothecary) {
    int number;
    char name[MAX_SIZE];
    scanf("%d", &number);
    scan_name(name);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL || !not_simmering(cauldron, number)) {
        return;
    }
    if (cauldron->ingredients == NULL) {
        printf("Error: Cauldron %d is empty.\n", number);
        return;
    }
    struct potion *potion = create_potion(name);
    potion->ingredients = cauldron->ingredients;
    potion->brew_potency = cauldron->brew_potency;
    cauldron->ingredients = NULL;
    cauldron->brew_potency = 0;
    cauldron->status = READY;
    cauldron->turns_remaining = 0;
    struct wizard *wizard = find_wizard(apothecary,
        cauldron->assigned_wizard_name);
    append_potion(wizard, potion);
    printf("Potion: '%s' bottled by %s!\n", name, wizard->name);
}

// Stage 2.4: reorder a cauldron while preserving its potency.
void reorder_cauldron(struct apothecary *apothecary) {
    int number;
    scanf("%d", &number);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL || !not_simmering(cauldron, number)) {
        return;
    }
    cauldron->ingredients = reorder_ingredients(cauldron->ingredients);
    printf("Cauldron %d reordered!\n", number);
}

// Split into two stable lists, then join them without allocating new nodes.
struct ingredient *reorder_ingredients(struct ingredient *head) {
    struct ingredient *top_head = NULL;
    struct ingredient *top_tail = NULL;
    struct ingredient *bottom_head = NULL;
    struct ingredient *bottom_tail = NULL;
    while (head != NULL) {
        struct ingredient *next = head->next;
        head->next = NULL;
        if (prefers_top(head->type)) {
            if (top_head == NULL) {
                top_head = head;
            } else {
                top_tail->next = head;
            }
            top_tail = head;
        } else {
            if (bottom_head == NULL) {
                bottom_head = head;
            } else {
                bottom_tail->next = head;
            }
            bottom_tail = head;
        }
        head = next;
    }
    if (top_head == NULL) {
        return bottom_head;
    }
    top_tail->next = bottom_head;
    return top_head;
}

// Free every ingredient in a list.
void free_ingredients(struct ingredient *head) {
    while (head != NULL) {
        struct ingredient *next = head->next;
        free(head);
        head = next;
    }
}

// Free every potion and the ingredient list owned by each potion.
void free_potions(struct potion *head) {
    while (head != NULL) {
        struct potion *next = head->next;
        free_ingredients(head->ingredients);
        free(head);
        head = next;
    }
}

// Free one cauldron and its ingredients, leaving its successor untouched.
void free_cauldron(struct cauldron *cauldron) {
    free_ingredients(cauldron->ingredients);
    free(cauldron);
}

// Stage 3.1: release all allocated objects on either q or EOF.
void free_apothecary(struct apothecary *apothecary) {
    struct wizard *wizard = apothecary->wizards;
    while (wizard != NULL) {
        struct wizard *next = wizard->next;
        free_potions(wizard->potions);
        free(wizard);
        wizard = next;
    }
    struct cauldron *cauldron = apothecary->cauldrons;
    while (cauldron != NULL) {
        struct cauldron *next = cauldron->next;
        free_cauldron(cauldron);
        cauldron = next;
    }
    free(apothecary);
}

// Stage 3.2: find and discard a potion from any wizard.
void remove_potion(struct apothecary *apothecary) {
    char name[MAX_SIZE];
    scan_name(name);
    struct wizard *wizard = apothecary->wizards;
    while (wizard != NULL) {
        if (remove_wizard_potion(wizard, name)) {
            printf("Potion: '%s' discarded!\n", name);
            return;
        }
        wizard = wizard->next;
    }
    printf("Error: No potion with name %s.\n", name);
}

// Remove a named potion from one wizard, returning whether it was found.
int remove_wizard_potion(struct wizard *wizard, char *name) {
    struct potion *potion = wizard->potions;
    struct potion *previous = NULL;
    while (potion != NULL && strcmp(potion->name, name) != 0) {
        previous = potion;
        potion = potion->next;
    }
    if (potion == NULL) {
        return 0;
    }
    if (previous == NULL) {
        wizard->potions = potion->next;
    } else {
        previous->next = potion->next;
    }
    free_ingredients(potion->ingredients);
    free(potion);
    return 1;
}

// Stage 3.2: discard one cauldron, updating subsequent cauldron numbers.
void remove_cauldron(struct apothecary *apothecary) {
    int number;
    scanf("%d", &number);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL || !not_simmering(cauldron, number)) {
        return;
    }
    unlink_cauldron(apothecary, cauldron);
    free_cauldron(cauldron);
    printf("Cauldron %d removed!\n", number);
}

// Detach a known cauldron without freeing it.
void unlink_cauldron(struct apothecary *apothecary, struct cauldron *cauldron) {
    if (apothecary->cauldrons == cauldron) {
        apothecary->cauldrons = cauldron->next;
    } else {
        struct cauldron *previous = apothecary->cauldrons;
        while (previous->next != cauldron) {
            previous = previous->next;
        }
        previous->next = cauldron->next;
    }
}

// Stage 3.3: check all errors before redistributing or deleting any data.
void remove_wizard(struct apothecary *apothecary) {
    char name[MAX_SIZE];
    scan_name(name);
    struct wizard *wizard = find_wizard(apothecary, name);
    if (wizard == NULL) {
        printf("Error: No wizard with name %s.\n", name);
        return;
    }
    if (has_simmering_cauldron(apothecary, name)) {
        printf("Error: %s has a cauldron currently simmering.\n", name);
        return;
    }
    if (wizard->potions != NULL && apothecary->wizards == wizard &&
        wizard->next == NULL) {
        printf("Error: Potions could not be redistributed to other wizards.\n");
        return;
    }
    redistribute_potions(apothecary, wizard);
    remove_wizard_cauldrons(apothecary, name);
    unlink_wizard(apothecary, wizard);
    free(wizard);
    printf("Wizard: '%s' has left the apothecary!\n", name);
}

// Check all cauldrons assigned to one wizard.
int has_simmering_cauldron(struct apothecary *apothecary, char *name) {
    struct cauldron *cauldron = apothecary->cauldrons;
    while (cauldron != NULL) {
        if (strcmp(cauldron->assigned_wizard_name, name) == 0 &&
            cauldron->status == SIMMERING) {
            return 1;
        }
        cauldron = cauldron->next;
    }
    return 0;
}

// Move each head potion to the current least-loaded remaining wizard.
void redistribute_potions(struct apothecary *apothecary, struct wizard *owner) {
    while (owner->potions != NULL) {
        struct wizard *recipient = fewest_potions(apothecary, owner);
        struct potion *potion = owner->potions;
        owner->potions = potion->next;
        potion->next = NULL;
        append_potion(recipient, potion);
    }
}

// Keep the first candidate on ties, so list-head proximity wins.
struct wizard *fewest_potions(struct apothecary *apothecary,
    struct wizard *departing) {
    struct wizard *best = NULL;
    int best_count = 0;
    struct wizard *wizard = apothecary->wizards;
    while (wizard != NULL) {
        if (wizard != departing) {
            int count = potion_count(wizard->potions);
            if (best == NULL || count < best_count) {
                best = wizard;
                best_count = count;
            }
        }
        wizard = wizard->next;
    }
    return best;
}

// Delete every cauldron belonging to a departing wizard in one traversal.
void remove_wizard_cauldrons(struct apothecary *apothecary, char *name) {
    struct cauldron *cauldron = apothecary->cauldrons;
    struct cauldron *previous = NULL;
    while (cauldron != NULL) {
        struct cauldron *next = cauldron->next;
        if (strcmp(cauldron->assigned_wizard_name, name) == 0) {
            if (previous == NULL) {
                apothecary->cauldrons = next;
            } else {
                previous->next = next;
            }
            free_cauldron(cauldron);
        } else {
            previous = cauldron;
        }
        cauldron = next;
    }
}

// Detach an existing wizard without touching their data.
void unlink_wizard(struct apothecary *apothecary, struct wizard *wizard) {
    if (apothecary->wizards == wizard) {
        apothecary->wizards = wizard->next;
    } else {
        struct wizard *previous = apothecary->wizards;
        while (previous->next != wizard) {
            previous = previous->next;
        }
        previous->next = wizard->next;
    }
}

// Stage 3.4: start a brew only after all preconditions pass.
void light_cauldron(struct apothecary *apothecary) {
    int number;
    int turns;
    scanf("%d %d", &number, &turns);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL) {
        return;
    }
    if (cauldron->status != READY) {
        printf("Error: Cauldron %d is not ready to be lit.\n", number);
        return;
    }
    if (cauldron->ingredients == NULL) {
        printf("Error: Cauldron %d is empty.\n", number);
        return;
    }
    if (turns < 1 || turns > MAX_BREW_TURNS) {
        printf("Error: Brew turns must be between 1 and 10.\n");
        return;
    }
    cauldron->status = SIMMERING;
    cauldron->turns_remaining = turns;
    printf("Cauldron %d lit for %d turns!\n", number, turns);
}

// Stage 3.4: simmer once and report the remaining turns.
void simmer_cauldron(struct apothecary *apothecary) {
    int number;
    scanf("%d", &number);
    struct cauldron *cauldron = find_cauldron(apothecary, number);
    if (cauldron == NULL) {
        return;
    }
    if (cauldron->status != SIMMERING) {
        printf("Error: Cauldron %d is not simmering.\n", number);
        return;
    }
    simmer_one_turn(cauldron);
    if (cauldron->status == BREWED) {
        printf("Cauldron %d has finished brewing!\n", number);
    } else {
        printf("Cauldron %d simmered, %d turns remaining!\n", number,
            cauldron->turns_remaining);
    }
}

// Compute placement using the list at the start of the turn, then remove nodes.
void simmer_one_turn(struct cauldron *cauldron) {
    int count = ingredient_count(cauldron->ingredients);
    struct ingredient *ingredient = cauldron->ingredients;
    int depth = 1;
    while (ingredient != NULL) {
        ingredient->potency -= POTENCY_DECAY;
        if (ingredient->potency < 0) {
            ingredient->potency = 0;
        }
        if (!is_misplaced(ingredient, depth, count)) {
            cauldron->brew_potency += BREW_GAIN;
        }
        ingredient = ingredient->next;
        depth++;
    }
    remove_spent_ingredients(cauldron);
    cauldron->turns_remaining--;
    if (cauldron->turns_remaining == 0) {
        cauldron->status = BREWED;
    }
}

// Free all ingredients whose potency has reached zero.
void remove_spent_ingredients(struct cauldron *cauldron) {
    struct ingredient *ingredient = cauldron->ingredients;
    struct ingredient *previous = NULL;
    while (ingredient != NULL) {
        struct ingredient *next = ingredient->next;
        if (ingredient->potency == 0) {
            if (previous == NULL) {
                cauldron->ingredients = next;
            } else {
                previous->next = next;
            }
            free(ingredient);
        } else {
            previous = ingredient;
        }
        ingredient = next;
    }
}


////////////////////////////////////////////////////////////////////////////////
// Provided function definitions
////////////////////////////////////////////////////////////////////////////////

// These are provided for you to use throughout the assignment.

// Function to print the program usage information
// Params: None
// Returns: None
void print_usage() {

    printf(
       "===========================[ Usage Info ]==========================\n"
       "                                                                   \n"
       "  ?                                                                \n"
       "    Show this help information.                                    \n"

       "  a w [wizard_name] [rank]                                         \n"
       "    Add a new wizard with the given rank.                          \n"
       "  a c [wizard_name] [required_rank]                                \n"
       "    Add a new cauldron, assigned to the given wizard.              \n"
       "  a i [cauldron_number] [type] [effect]                            \n"
       "    Add a new ingredient to the given cauldron.                    \n"

       "  c [cauldron_number]                                              \n"
       "    Inspect a single cauldron.                                     \n"
       "  i [cauldron_number] [depth] [type] [effect]                      \n"
       "    Insert an ingredient at a given depth.                         \n"
       "  b [cauldron_number] [potion_name]                                \n"
       "    Bottle a cauldron into a potion.                               \n"
       "  o [cauldron_number]                                              \n"
       "    Reorder the ingredients in a cauldron.                         \n"
       "  r p [potion_name]                                                \n"
       "    Remove a potion.                                               \n"
       "  r c [cauldron_number]                                            \n"
       "    Remove a cauldron.                                             \n"
       "  r w [wizard_name]                                                \n"
       "    Remove a wizard and redistribute their potions.                \n"
       "  l [cauldron_number] [turns]                                      \n"
       "    Light a cauldron to simmer for a number of turns.              \n"
       "  s [cauldron_number]                                              \n"
       "    Simmer a cauldron for one turn.                                \n"
       "  p [cauldron_number] [turns]                                      \n"
       "    Peek at what the next simmer turns would do.                   \n"
       "  d [wizard_a] [wizard_b]                                          \n"
       "    Duel two wizards with their potions.                           \n"

       "  *                                                                \n"
       "    Print the apothecary.                                          \n"

       "  q                                                                \n"
       "    Quits the apothecary.                                          \n"
       "                                                                   \n"
       "===================================================================\n"
    );

}

// Function to print the welcome banner
// Params: None
// Returns: None
void print_welcome_banner() {

    printf(
    "                  Welcome to....\n"
    "                .       o      .       o\n"
    "      o    .        O       .       O       .    o\n"
    "       T H E   P O T I O N   A P O T H E C A R Y\n"
    "     .   o  .   O   .   o   .   O   .   o   .   O\n"
    " (o) (O)  (o) (O)  (o)  (O) (o)  (O) (o)  (O) (o) (O)\n"
    "oOo(_)oOo(_)oOoOo(_)oOo(_)oOo(_)oOoOo(_)oOo(_)oOo(_)oO\n"
    "~o~^~O~o~^~o~O~^~o~^~O~o~^~o~^~O~o~^~o~O~^~o~^~O~o~^~o\n\n"
    );

}

// Scan in a name string into the provided buffer, placing
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

// Scans a string and converts it to an enum ingredient_effect.
//
// Returns:
//      The corresponding ingredient effect, if the string was valid,
//      Otherwise, returns INVALID_EFFECT.
//
// Usage:
// ```
//      enum ingredient_effect effect = scan_ingredient_effect();
// ```
//
enum ingredient_effect scan_ingredient_effect(void) {
    char effect[MAX_SIZE];
    scan_token(effect, MAX_SIZE);
    return string_to_ingredient_effect(effect);
}

// Scans a string and converts it to an enum ingredient_type.
//
// Returns:
//      The corresponding ingredient type, if the string was valid,
//      Otherwise, returns INVALID_TYPE.
//
// Usage:
// ```
//      enum ingredient_type type = scan_ingredient_type();
// ```
//
enum ingredient_type scan_ingredient_type(void) {
    char type[MAX_SIZE];
    scan_token(type, MAX_SIZE);
    return string_to_ingredient_type(type);
}

// Scans a string and converts it to an enum wizard_rank.
//
// Returns:
//      The corresponding wizard rank, if the string was valid,
//      Otherwise, returns INVALID_RANK.
//
// Usage:
// ```
//      enum wizard_rank rank = scan_wizard_rank();
// ```
//
enum wizard_rank scan_wizard_rank(void) {
    char rank[MAX_SIZE];
    scan_token(rank, MAX_SIZE);
    return string_to_wizard_rank(rank);
}

// Converts an enum ingredient_effect to a string, 
// e.g. ENERGETIC -> "ENERGETIC".
//
// Returns:
//      The effect as a string, or "INVALID_EFFECT" if it is not valid.
char *ingredient_effect_to_string(enum ingredient_effect effect) {

    if (effect == ENERGETIC) {
        return "ENERGETIC";
    }
    if (effect == SLOWING) {
        return "SLOWING";
    }
    if (effect == CHILLING) {
        return "CHILLING";
    }
    if (effect == SCORCHING) {
        return "SCORCHING";
    }
    if (effect == FLYING) {
        return "FLYING";
    }
    if (effect == SINKING) {
        return "SINKING";
    }
    if (effect == INERT) {
        return "INERT";
    }

    return "INVALID_EFFECT";
}

// Converts an enum ingredient_type to a string, e.g. MUSHROOM -> "MUSHROOM".
//
// Returns:
//      The type as a string, or "INVALID_TYPE" if it is not valid.
char *ingredient_type_to_string(enum ingredient_type type) {

    if (type == MUSHROOM) {
        return "MUSHROOM";
    }
    if (type == CRYSTAL) {
        return "CRYSTAL";
    }
    if (type == ROOT) {
        return "ROOT";
    }
    if (type == FLOWER) {
        return "FLOWER";
    }

    return "INVALID_TYPE";
}

// Converts an enum wizard_rank to a string, e.g. NOVICE -> "NOVICE".
//
// Returns:
//      The rank as a string, or "INVALID_RANK" if it is not valid.
char *wizard_rank_to_string(enum wizard_rank rank) {
    if (rank == NOVICE) {
        return "NOVICE";
    }
    if (rank == APPRENTICE) {
        return "APPRENTICE";
    }
    if (rank == MASTER) {
        return "MASTER";
    }

    return "INVALID_RANK";
}

// Converts an enum brew_status to a string, e.g. READY -> "READY".
//
// Returns:
//      The status as a string, or "INVALID_STATUS" if it is not valid.
char *brew_status_to_string(enum brew_status status) {
    if (status == READY) {
        return "READY";
    }
    if (status == SIMMERING) {
        return "SIMMERING";
    }
    if (status == BREWED) {
        return "BREWED";
    }

    return "INVALID_STATUS";
}

// Helper function to print out the potion information
// PARAMS:
//      potion - a pointer to a single potion
// RETURNS: None
void print_potion_list(struct potion *potion) {
    if (potion == NULL) {
        printf("    No potions at the moment!\n\n");
        return;
    }
    while (potion != NULL) {
        printf("    %s [Potency: %d]\n", potion->name, potion->brew_potency);
        printf("      Ingredients: ");
        print_ingredients_horizontally(potion->ingredients);
        printf("\n\n");
        potion = potion->next;
    }
}

// Helper function to print out the cauldron information (including the
// ingredients it holds)
// PARAMS:
//      cauldron - a pointer to a single cauldron
//      index - the 1-indexed position of the cauldron
// RETURNS: None
void print_cauldron(struct cauldron *cauldron, int index) {
    printf("  %d. |%s| [%s]\n", index, cauldron->assigned_wizard_name,
        brew_status_to_string(cauldron->status));
    printf("  Required rank: %s\n",
        wizard_rank_to_string(cauldron->required_rank));
    printf("  Brew potency: %d\n", cauldron->brew_potency);

    printf("\n  ==================================================\n");
    if (cauldron->ingredients == NULL) {
        printf("    No ingredients yet\n");
    } else {
        struct ingredient *ingredient = cauldron->ingredients;
        while (ingredient != NULL) {
            // You might not have seen something like %8s before: this pads
            // the string with spaces if it is less than 8 characters long,
            // making our ingredients appear right-aligned when printed!
            printf("    %8s ::::: %s (%d)\n",
                ingredient_type_to_string(ingredient->type),
                ingredient_effect_to_string(ingredient->effect),
                ingredient->potency);
            ingredient = ingredient->next;
        }
    }

    printf("  ==================================================\n\n");

}

// Helper function to print out a list of ingredients, separated by commas
// PARAMS:
//      head - a pointer to a list of ingredients
// RETURNS: None
void print_ingredients_horizontally(struct ingredient *head) {
    if (head == NULL) {
        return;
    }
    struct ingredient *curr = head;
    while (curr->next != NULL) {
        printf("%s %s, ", ingredient_type_to_string(curr->type),
            ingredient_effect_to_string(curr->effect));
        curr = curr->next;
    }
    printf("%s %s", ingredient_type_to_string(curr->type),
        ingredient_effect_to_string(curr->effect));
}


////////////////////////////////////////////////////////////////////////////////
// Additional provided function definitions
////////////////////////////////////////////////////////////////////////////////

// You don't need to use any of these, or understand how they work!
// We use them to implement some of the provided functions above.

enum ingredient_effect string_to_ingredient_effect(char *effect_str) {
    int len = strlen(effect_str);

    if (strncasecmp(effect_str, "ENERGETIC", len) == 0) {
        return ENERGETIC;
    }
    if (strncasecmp(effect_str, "SLOWING", len) == 0) {
        return SLOWING;
    }
    if (strncasecmp(effect_str, "CHILLING", len) == 0) {
        return CHILLING;
    }
    if (strncasecmp(effect_str, "SCORCHING", len) == 0) {
        return SCORCHING;
    }
    if (strncasecmp(effect_str, "FLYING", len) == 0) {
        return FLYING;
    }
    if (strncasecmp(effect_str, "SINKING", len) == 0) {
        return SINKING;
    }
    if (strncasecmp(effect_str, "INERT", len) == 0) {
        return INERT;
    }

    return INVALID_EFFECT;
}

enum ingredient_type string_to_ingredient_type(char *type_str) {
    int len = strlen(type_str);

    if (strncasecmp(type_str, "MUSHROOM", len) == 0) {
        return MUSHROOM;
    }
    if (strncasecmp(type_str, "CRYSTAL", len) == 0) {
        return CRYSTAL;
    }
    if (strncasecmp(type_str, "ROOT", len) == 0) {
        return ROOT;
    }
    if (strncasecmp(type_str, "FLOWER", len) == 0) {
        return FLOWER;
    }

    return INVALID_TYPE;
}

enum wizard_rank string_to_wizard_rank(char *rank_str) {
    int len = strlen(rank_str);

    if (strncasecmp(rank_str, "NOVICE", len) == 0) {
        return NOVICE;
    }
    if (strncasecmp(rank_str, "APPRENTICE", len) == 0) {
        return APPRENTICE;
    }
    if (strncasecmp(rank_str, "MASTER", len) == 0) {
        return MASTER;
    }

    return INVALID_RANK;
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

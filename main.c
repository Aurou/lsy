// Assignment 2 26T3 COMP1511: CS Alchemy
// main.c
//
// Written by <YOUR-NAME>, <YOUR-ZID>
// on <TODAYS-DATE>
//
// DESCRIPTION OF YOUR PROGRAM HERE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cs_alchemy.h"

int main(void) {
    print_welcome_banner();
    
    printf("Enter the name of the apothecary: ");
    char apothecary_name[MAX_SIZE];
    scan_name(apothecary_name);
    struct apothecary *my_apothecary = create_apothecary(apothecary_name);

    command_loop(my_apothecary);

    printf("\nThank you for brewing potions!\n");
    return 0;
}

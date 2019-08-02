// Headers
#include <stdio.h>
#include <ctype.h>  // parse strings, isdigit,
#include <string.h> // strlen
#include <stdlib.h> // random number generator functions
#include <time.h>   // time function to seed the random number generator function

// Constants
#define INVENTORY_ROWS 4    // Max 26
#define INVENTORY_COLUMNS 3 // Max 100
#define MAX_STR_LENGTH 20
#define MAX_ITEM_NAMES 24
#define MAX_ITEM_QUANTITY 12

// Printing Constants
#define PM_CORNER "#"
#define PM_HLINE "-"
#define PM_VLINE "|"
#define PM_SPACE " "

// Structures
typedef struct{
    int price_cent;
    int quantity;
    char *name;
} S_Item;

// Function Declare
int parse_int(char **s);
void parse_dollars_cents(char **s, int *dollar, int *cent);
int Change_Cal(int amount, int denominator); //
void Print_Machine(S_Item Inventory[INVENTORY_ROWS][INVENTORY_COLUMNS]);  // Prints whole the Vending Machine
static void _PM_Blank_Line();  // Prints a blank line in the Vending Machine
static void _PM_Full_Line();   // Prints a full line in the Vending Machine
static void _PM_Center_Print(char *String, int Width); // prints a string centered
static void _PM_Start_1();
static void _PM_Cap_1();
static void _PM_Start_2();
static void _PM_Cap_2();

// Main
int main() {
//// Create Inventory

    // Initialize Inventory
    S_Item Inventory[INVENTORY_ROWS][INVENTORY_COLUMNS];

    // Possible Items in the Vending Machine
    char *item_names[] = {"Milk Duds", "M&M's", "Snickers", "Twix", "Reeses", "Butterfingers",
                          "Milky Way", "Baby Ruth",
                          "Gum", "Doritos", "Cheetos", "Potato Chips", "Oreos",
                          "Grandma's Cookies", "Rice Crispy Treats",
                          "Peanuts", "Corn Nuts", "Trail Mix", "Honey Buns", "Pop Tarts", "Cheez-its",
                          "Granola Bars", "Beef Jerky", "Pretzels"};

    // Initialize Rand
    srand(time(NULL));

    // Select Items (Randomly)
    for(int i = 0; i < INVENTORY_ROWS; i++){
        for(int j = 0; j < INVENTORY_COLUMNS; j++){
            Inventory[i][j].name = item_names[rand() % MAX_ITEM_NAMES];
            Inventory[i][j].price_cent = 25*(rand() % 5) + 75; // Min $0.75 Max $1.75
            Inventory[i][j].quantity = rand() % MAX_ITEM_QUANTITY;
        }
    }

//// Show Vending Machine
    Print_Machine(Inventory);

//// Ask for user choice

    // Ask user for choice
    printf("What do you want to buy?\n");

    // Flags for while loop
    int flag_letter = 0;
    int flag_number = 0;
    int row_index;
    int column_index;
    int try_counter = 0;


    // Handles most sensible inputs.
    do {    // Try until user makes a choice or tries to many times
        char user_input[MAX_STR_LENGTH];
        char *p = user_input;

        // Capture user's input as a string
        gets(user_input);

        // Parse input
        while (*p) {
            if (isalpha(*p) && 0 == flag_letter) { // Find letter
                row_index = toupper(p[0]) - 'A';

                if(INVENTORY_ROWS > row_index && 0 <= row_index){
                    flag_letter = 1;
                }

                p++;
            } else if ((isdigit(*p) || isdigit(*(p + 1))) && 0 == flag_number) { // Find number
                column_index = strtol(p, &p, 10);

                if(INVENTORY_COLUMNS > column_index && 0 <= column_index){
                    flag_number = 1;
                }

            } else {
                p++;
            }
        }

        // Was the right info parsed from the user input?
        if (1 == flag_letter && 1 == flag_number){
            if (0 == Inventory[row_index][column_index].quantity) {
                printf("That item is not in stock.\n");
                flag_letter = 0;
                flag_number = 0;
            }
        } else {
            flag_letter = 0;
            flag_number = 0;
        }

        try_counter++;

    }while( 4 > try_counter && (1 != flag_letter && 1 != flag_number) );

    if(1 != flag_letter && 1 != flag_number){
        printf("The vending machine could not understand your input.\n");
        return 0;
    }

    printf("\n");

//// Ask user for money
    int user_money_cent = 0;
    int flag = 0;

    // ask user for money
    printf("Insert money (X.XX) to purchase a %s.\n", Inventory[row_index][column_index].name);

    do{
        char user_input[MAX_STR_LENGTH];
        char *p = user_input;
        int temp_dollar = 0;
        int temp_cent = 0;

        scanf("%s", user_input);
        printf("\n");

        parse_dollars_cents(&p, &temp_dollar, &temp_cent);
        // Note that large values of cent or dollars create errors if the user's input is greater than the max size of int.

        while(99 < temp_cent){
            temp_cent = temp_cent - (temp_cent % 10);
            temp_cent = temp_cent / 10;
        }

        user_money_cent = user_money_cent + 100*temp_dollar + temp_cent;

        if(Inventory[row_index][column_index].price_cent > user_money_cent){
            printf("Balance: $%.2f. Not enough money was given, add more.\n", (double) user_money_cent/100);
            flag = 0;
        } else {
            flag = 1;
        }

    } while( 1 != flag);

    printf("\n");

//// Deal Item, Change, Update Quantity

    // figure out how much change to give
    int user_change_cent = user_money_cent - Inventory[row_index][column_index].price_cent;

    // calculate change
    int temp_change_cent = user_change_cent;
    int quarter_num;
    int dime_num;
    int nickel_num;
    int penny_num;

    if(0 != user_change_cent){
        quarter_num = Change_Cal(temp_change_cent, 25);
        temp_change_cent = temp_change_cent - 25*quarter_num;
        dime_num = Change_Cal(temp_change_cent, 10);
        temp_change_cent = temp_change_cent - 10*dime_num;
        nickel_num = Change_Cal(temp_change_cent, 5);
        temp_change_cent = temp_change_cent - 5*nickel_num;
        penny_num = Change_Cal(temp_change_cent, 1);
    }

    // print messages
    printf("The machine drops a %s ", Inventory[row_index][column_index].name);

    if(0 != user_change_cent){
        printf("and gives you %d quarters, %d  dimes, %d nickels, and %d pennies making a total of $%.2f.\n", quarter_num,
               dime_num, nickel_num, penny_num, (float) user_change_cent/100 );
    } else{
        printf("\n");
    }

    printf("\n");

    // Update Vending machine inventory
    Inventory[row_index][column_index].quantity--;


//// Show Updated Vending Machine
    //Print_Machine(Inventory);
    return 0;
}

//// Functions
int parse_int(char **s) {
    char sign = **s;
    if ('-' == sign || '+' == sign || '$' == sign) {
        (*s)++;
    }
    int result = 0;
    unsigned char ch;
    while (isdigit(ch = **s)) {
        (*s)++;
        // By accumulating the int on the negative side, we can parse INT_MIN
        result = result * 10 + '0' - ch;
    }

    if (sign != '-') result = -result;
    return result;
}

void parse_dollars_cents(char **s, int *dollar, int *cent) {
    *dollar = parse_int(s);
    if ('.' == **s) {
        (*s)++;
        *cent = parse_int(s);
    } else {
        *cent = 0;
    }
}

int Change_Cal(int amount, int denominator){

    int count = 0;

    while(0 <= amount - denominator){

        amount = amount - denominator;
        count++;
    }

    return count;
}

void Print_Machine(S_Item Inventory[INVENTORY_ROWS][INVENTORY_COLUMNS]){

    //// Header
    _PM_Full_Line();
    _PM_Blank_Line();
    _PM_Start_1();
    _PM_Center_Print("Josh's Vending Machine", INVENTORY_COLUMNS*MAX_STR_LENGTH);
    _PM_Cap_1();
    _PM_Blank_Line();
    _PM_Full_Line();

    //// Inventory
    for(int i = 0; i < INVENTORY_ROWS; i++){
        // Print Blank Line
        _PM_Blank_Line();

        // Print Item Lookup
        _PM_Start_1();
        for(int j = 0; j < INVENTORY_COLUMNS; j++){
            char temp[MAX_STR_LENGTH];
            sprintf(temp, "%c%0.2d", (char) 'A' + i, j);
            _PM_Center_Print(temp, MAX_STR_LENGTH);
        }
        _PM_Cap_1();

        // Print Item Names
        _PM_Start_1();
        for(int j = 0; j < INVENTORY_COLUMNS; j++){
            _PM_Center_Print(Inventory[i][j].name, MAX_STR_LENGTH);
        }
        _PM_Cap_1();

        // Print Item Quantity
        _PM_Start_1();
        for(int j = 0; j < INVENTORY_COLUMNS; j++){
            char temp[MAX_STR_LENGTH];
            sprintf(temp, "Supply: %d", Inventory[i][j].quantity);
            _PM_Center_Print(temp, MAX_STR_LENGTH);
        }
        _PM_Cap_1();

        // Print Item Price
        _PM_Start_1();
        for(int j = 0; j < INVENTORY_COLUMNS; j++){
            char temp[MAX_STR_LENGTH];
            sprintf(temp, "$%.2f", (float) Inventory[i][j].price_cent/100);
            _PM_Center_Print(temp, MAX_STR_LENGTH);
        }
        _PM_Cap_1();
    }

    //// Footer
    _PM_Blank_Line();
    _PM_Full_Line();

    // Blank Line
    printf("\n");
}

//// Print Machine Function
static void _PM_Start_1(){
    printf("%s%s", PM_VLINE, PM_SPACE);
}

static void _PM_Cap_1(){
    printf("%s%s\n", PM_SPACE, PM_VLINE);
}

static void _PM_Start_2(){
    printf("%s%s", PM_CORNER, PM_HLINE);
}

static void _PM_Cap_2(){
    printf("%s%s\n", PM_HLINE, PM_CORNER);
}

static void _PM_Blank_Line(){
    _PM_Start_1();
    for(int i = 0; i < INVENTORY_COLUMNS*MAX_STR_LENGTH; i++){
        printf("%s", PM_SPACE);
    }
    _PM_Cap_1();
};

static void _PM_Full_Line(){
    _PM_Start_2();
    for(int i = 0; i < INVENTORY_COLUMNS*MAX_STR_LENGTH; i++){
        printf("%s", PM_HLINE);
    }
    _PM_Cap_2();
}

static void _PM_Center_Print(char *String, int Width){
    int str_length = strlen(String);
    int i;

    for(i = 0; i <=(Width - str_length)/2; i++){
        printf("%s", PM_SPACE);
    }
    printf("%s", String);
    i += str_length;
    for(; i < Width; i++){
        printf("%s", PM_SPACE);
    }
}
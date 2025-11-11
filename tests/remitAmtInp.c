
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../isNum.h"



double amountAdd = 0;
double senderBalance = 1000;
double receiverBalance = 1002;

char amount[100] = "";
char *endPtr;

int main() {
    for (int i=0; i<20; i++) {
        while (amountAdd <= 0 || senderBalance < amountAdd*(1+0.03) || !isNum(amount)) {
            printf("Enter Amount to Transfer: RM ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountAdd = strtod(amount, &endPtr);

            if (!isNum(amount)) printf("Invalid input. Please enter a numeric value.\n\n");
            else if (endPtr == amount || *endPtr != '\0') printf("Invalid input. Please enter a numeric value.\n\n");
            else if (senderBalance < amountAdd) printf("Error - Insufficient Balance.\n\n");
            else if (amountAdd == 0) printf("Error - Add Amount Must Be Greater than 0.\n\n");
        }

        amountAdd = 0;
        amount[0] = '\0';
    }
}
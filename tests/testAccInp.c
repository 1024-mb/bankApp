#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// allows efficient testing of individual deposit amount input
bool isNum(char inString[]) {
    int maxIndex = strlen(inString);
    char current;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if (!('0' <= current && current <= '9') && !(current == '.')) {
            return false;
        }
    }

    return true;
}

int main () {
    double amountAdd;
    char amount[1024];
    char *endPtr;

    for (int i =0; i< 20; i++) {

        while (amountAdd <= 0 || amountAdd > 50000 || !isNum(amount)) {
            printf("Enter Amount to deposit: RM ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountAdd = strtod(amount, &endPtr);


            if (!isNum(amount)) {printf("Invalid input. Please enter a numeric value.\n\n");}
            else if (amountAdd > 50000) printf("Deposit Amount Cannot Be Greater than RM 50,000\n\n");
            else if (amountAdd<=0) printf("Please Enter a Value Above 0.\n\n");

        }
        amountAdd = 0;
        amount[0] = '\0';
    }
}



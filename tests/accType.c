#include <stdio.h>
#include <string.h>
#include <strings.h>

int main() {
    char accType[8];
    do {
        printf("Enter Account Type (S for Savings, C for Current):  ");
        scanf("%s", accType);

        if (strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) printf("Enter C or S \n\n");

    } while ((strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) || strlen(accType) == 0);

    return 0;
}
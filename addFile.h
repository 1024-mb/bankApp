#include <stdio.h>
#include <string.h>
#include <time.h>
// writes the data to the file corresponding to the account number
// used for updating the balance during deposit/withdrawal
void addFile(char accNo[], char name[], char ID[], char accType[], float balance, char PIN[]) {
    char finalFile[1024] = "./database/";
    strcat(finalFile, accNo);
    FILE *pfile = fopen(finalFile, "w");

    if (!(pfile == NULL)) {
        char logs[2048];
        time_t currentTime;
        time(&currentTime);


        snprintf(logs,sizeof(logs), "NAME:%s,ID:%s,NUMBER:%s,TYPE:%s,BALANCE:%.2f,PIN:%s",
            name, ID, accNo, accType, balance, PIN);

        fprintf(pfile, "%s" ,logs);
        fclose(pfile);

        FILE *index = fopen("./database/index.txt", "a");
        fprintf(index, "%s\n", accNo);
        fclose(index);
    }
}
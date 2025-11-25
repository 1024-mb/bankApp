void displayInfo() {
    char buffer[1024] = "";
    int count=0;

    FILE *countFile = fopen("./database/index.txt","ab+");
    while (fgets(buffer, sizeof(buffer), countFile) != NULL) {
        count++;
    }
    fclose(countFile);

    time_t currentTime;
    time(&currentTime);
    char* time = ctime(&currentTime);
    printf("\nSession Start: %s", time);
    printf("Accounts Loaded: %d\n", count);
}
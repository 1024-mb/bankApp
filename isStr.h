// Checks if the string is entirely alphabetic / has a space (for name)
bool isStr(char inString[]) {
    int maxIndex = strlen(inString);
    char current;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if (!('a' <= current && current <= 'z') && !('A' <= current && current <= 'Z') && current != ' ') {
            return false;
        }
    }

    return true;
}

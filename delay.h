#include <unistd.h>

// creates a delay (useful for ensuring the user can keep up with output to the terminal
void delay(double number_of_seconds){
    sleep(number_of_seconds);
}
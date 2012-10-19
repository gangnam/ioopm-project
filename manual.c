#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int typeReader(char* input) {
    int multiply = 0;
    int result = 0;
    int i = 0;

    while (input[i]) {
        if (isdigit(input[i])) {
            multiply *= 10;
            multiply += (input[i] - 48);
            }
        else {
            if (multiply == 0) {
                multiply = 1;
                }
            if  (input[i] == '*') {
                result += (multiply * sizeof(void*));
                multiply = 0;
                }
            else if (input[i] == 'i') {
                result += multiply * sizeof(int);
                multiply = 0;
                }
            else if (input[i] == 'l') {
                result += multiply * sizeof(long);
                multiply = 0;
                }
            else if (input[i] == 'f') {
                result += multiply * sizeof(float);
                multiply = 0;
                }
            else if (input[i] == 'd') {
                result += multiply * sizeof(double);
                multiply = 0;
                }
            else {
                result += (multiply * sizeof(char));
                multiply = 0;
                }
            }
        i++;
        }
    return result;
    }

int main() {
    char *test = "3**5d\0";
    int p = typeReader(test);
    printf("%d\n", p);
    return 0;
    }

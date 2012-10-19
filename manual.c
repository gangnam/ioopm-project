#ifndef __manual__
#define __manual__

#include <ctype.h>
#include <stdio.h>
#include "imalloc.h"
#include "priv_imalloc.h"

int typeReader(char* input) {
    int multiply = 0, result = 0, i = 0, size;
    while (input[i]) {
        if (isdigit(input[i])) {
            multiply *= 10;
            multiply += (input[i] - 48);
            }
        else {
            if (multiply == 0) {
                multiply = 1;
                }

            switch (input[i]) {

                case '*' : {
                    size = sizeof(void*);
                    break;
                    }
                case 'i' : {
                    size = sizeof(int);
                    break;
                    }
                case 'l' : {
                    size = sizeof(long);
                    break;
                    }
                case 'f' : {
                    size = sizeof(float);
                    break;
                    }
                case 'd': {
                    size = sizeof(double);
                    break;
                    }
                default: {
                    size = sizeof(char);
                    break;
                    }
                }

            result += multiply * size;
            multiply = 0;

            }
        i++;
        }
    return result;
    }

unsigned int avail(Memory mem) {
    Chunk c = mem->data;
    int avail = 0;
    for (; c; c = c->next) {
        if (c->free) {
            avail += c->size;
            }
        }
    return avail;
    }

int main() {
    int b = typeReader("20i\0");
    printf("answer: %d\n",b);
    return 0;
    }

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include "info.h"
#include "functions.h"


int main(int argc, char* argv[]) {

    int file;

    if (argc == 2) {

        if (strcmp(argv[1], "--version") == 0) {
            printVersion();
            return 0;
        }

        else if (strcmp(argv[1], "--help") == 0) {
            printHelp();
            return 0;
        }

        else if (strcmp(argv[1], "cpu03.bmp") == 0) {
            file = open(argv[1], O_RDONLY);
            if (file < 0) {
                fprintf(stderr, "Nem lehet megnyitni a fájlt.\n");
                return 1;
            }

        }

        else {
            fprintf(stderr, "A parancs nem található. Kérem adjon meg más parancsot.\n");
            return 2;
        }
    }

    else if (argc == 1) {
        file = BrowseForOpen();
        if (file < 0) {
            fprintf(stderr, "Nem lehet megnyitni a fájlt.\n");
            return 1;
        }
    }
    else {
        printf("Túl sok parancsot adott meg!\n");
        return 3;
    }

    signal(SIGALRM, WhatToDo);
    signal(SIGINT, WhatToDo);
    alarm(2);

    int encNum;                                            // a kódolt karakterek száma
    char *Pixels = ReadPixels(file, &encNum);              // a beolvasott pixeleket tartalmazó tömb
    char* Dec = Unwrap(Pixels, encNum);                    // a dekódolt karaktereket tartalmazó tömb

    alarm(0);

    /*******Ha a konzolra szeretnénk kiiratni******
    for (int i = 0; i < encNum; i++) {
        printf("%c", Dec[i]);
    }
    */

    char *neptunID = "ZP5J12";
    int ret = Post(neptunID, Dec, encNum);

    if (ret == 0) {
        printf("The message has been successfully sent!\n");
    }
    else {
        fprintf(stderr, "Cannot send the message!\n");
        return 4;
    }

    free(Dec);
    close(file);

    return 0;
}
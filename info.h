#include <stdio.h>

void printVersion() {
    printf("Version: 1.6.1\nCreated: 2021.03.10\nCreated By: Gyurcsák Dániel\nLast modified: 2021.04.23\n\n    © Copyright\nAll Rights Reserved ®\n");
}

void printHelp() {
    printf(" ||-------------------------------HELP-------------------------------||\n\n");
    printf(" *  A program verziószámának megtekíntéséhez futtatáskor adja meg a 'Programnév' --version parancsot.\n");
    printf(" *  Ha képfájlból szeretne szöveget dekódolni akkor futtatáskor adja meg a 'Programnév' képfájlnév parancsot.\n");
    printf(" *  Abban az esetben ha nem ad meg semmit a 'Programnév' után, akkor tallózhat a mappák és a fájlok között.\n");
    printf(" *  Több parancs esetén a program nem fog lefutni.\n");
    printf(" *  További információkért a program működésével kapcsolatban, tekintse meg a dokumentációt.\n");
}
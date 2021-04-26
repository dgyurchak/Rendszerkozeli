#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omp.h>
#include <signal.h>
#include <sys/wait.h>

/*
char* TestArray(int *NumCh) {

    srand(time(NULL));
    char abc[3] = {'a', 'b', 'c'};
    *NumCh = sizeof(abc);
    int n = *NumCh * 3;
    char* Enc = (char*)malloc(n * sizeof(char));
    if (Enc == NULL) {
        fprintf(stderr, "A memóriafoglalás sikertelen.\t(TestArray)\n");
        exit(1);
    }

    int j = 2;
    for (int i = n-1; i >= 0; i--) {
        Enc[i] = rand() % 120 + 5;
        if (i % 3 == 0) {
            Enc[i] = Enc[i] << 2;
            Enc[i] = Enc[i] | abc[j];
            if (j > 0) j--;
        }
        else {
            Enc[i] = Enc[i] << 3;
            Enc[i] = Enc[i] | abc[j];
            abc[j] = abc[j] >> 3;
        }
    }
    return Enc;
}
*/

char* Unwrap(char* Pbuff, int NumCh) {

    char* Dec = (char*)malloc(NumCh * sizeof(char));   //a kikódolt karaktereket tartalmazó tömb
    if (Dec == NULL) {
        fprintf(stderr, "A memóriafoglalás sikertelen.\t(Unwrap)\n");
        exit(1);
    }

    int i;
    #pragma omp parallel for
    for(i = 0; i < NumCh; i++)
    {
        Dec[i] = (Pbuff[(i*3)] & 0b00000011)<<6 | (Pbuff[(i*3)+1] & 0b00000111)<<3 | Pbuff[(i*3)+2] & 0b00000111;
    }
    free(Pbuff);
    return Dec;

}


char* ReadPixels(int f, int* NumCh) {

    int size;
    int start;
    lseek(f, 2, SEEK_SET);
    read(f, &size, sizeof(int));    //beolvassuk a teljes fájl méretét
    lseek(f, 6, SEEK_SET);
    read(f, NumCh, sizeof(int));   //beolvassuk a kódolt karakterek számát
    lseek(f, 10, SEEK_SET);
    read(f, &start, sizeof(int));   //beolvassuk honnan kezdődik  a pixel array

    char* pixelArray = (char*)malloc((size - start)* sizeof(char));
    if (pixelArray == NULL) {
        fprintf(stderr, "A memóriafoglalás sikertelen.\t(ReadPixels)\n");
        exit(1);
    }

    for (int i = 0; i < size-start; i++) {
        lseek(f, i+start, SEEK_SET);
        read(f, &pixelArray[i], sizeof(char));
    }

    return pixelArray;

}


int BrowseForOpen() {

    DIR* dir;
    struct stat inode;
    bool set = true;
    char name[30];

    dir = opendir("."); //".", ".."
    struct dirent *entry; //d_name, d_ino
    while((entry = readdir(dir)) != NULL){      // a könyvtár tartalmának kiírása
        printf("%s \t", (*entry).d_name);
    }
    printf("\n");

    while(set) {
        scanf("%s", name);                         // a könyvtár nevének bekérése a felhasználótól

        if(stat(name, &inode) == -1){
            fprintf(stderr, "A megadott fájl vagy könyvtár nem található. Kérem adjon meg más nevet.\n");
        }
        else if (inode.st_mode & __S_IFREG) {
            set = false;
            break;
        }
        else if(inode.st_mode & __S_IFDIR) {
            chdir(name);
            dir = opendir(".");
            while((entry = readdir(dir)) != NULL){      // a könyvtár tartalmának kiírása
                printf("%s \t", (*entry).d_name);
            }
            printf("\n");
        }
    }

    int file = open(name, O_RDONLY);
    closedir(dir);
    return file;

}


int Post(char *neptunID, char *message, int NumCh) {

    char on = 1;
    int port = 80;                                                  // a szerver portszáma
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("193.6.135.162");            // a szerver ip cime

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        fprintf(stderr, "Could not create socket\n");
        return 2;
    }

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on));

    unsigned int server_size = sizeof(server);
    connect(server_socket, (struct sockaddr *)&server, server_size);

    char buffer[1024];
    int content_length = NumCh + 27;                    // meghatározzuk a küldendő szöveg méretét ami a dekódolt karakterek + a többi karakter

    sprintf(buffer, "POST /~vargai/post.php HTTP/1.1\r\nHost: irh.inf.unideb.hu\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nNeptunID=%s&PostedText=%s\r\n", content_length, neptunID, message);
    int recSize;
    int sentSize;

    sentSize = send(server_socket, buffer, sizeof(buffer), 0);              // elküldjük a szervernek az üzenetet
    recSize = recv(server_socket, buffer, 1024, 0);                         // várjuk a válasz tartalmát a szervertől

    if (strstr(buffer, "The message has been received.")) {
        close(server_socket);
        return 0;
    }
    else {
        close(server_socket);
        return 1;
    }

}


void WhatToDo(int sig) {

    if (sig == SIGALRM) {
        fprintf(stderr, "A program túl sokáig futott!\n");
        exit(1);
    }
    else if (sig == SIGINT) {
        pid_t pid;
        pid = fork();
        if (pid == 0) {     //child
            printf("\nJelenleg a < ctrl+c > billentyűkombináció nem állítja le a programot! (Haha)\n");
            raise(SIGKILL);
        }
    }

}
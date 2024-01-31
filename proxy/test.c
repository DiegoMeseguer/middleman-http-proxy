#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calc_filesize(const char* fileName);

int main() {

    /*
    printf("Hello\n");
    char s[20] = "hello";

    char a[] = "hello";
    printf("%lu\n", sizeof(a));
    printf("%lu\n", strlen(a));

    //Another example
    int size;
    size = calc_filesize("index.html");
    printf("The size of the file is: %d bytes\n", size);

    */

   char buffer[500];
   bzero(buffer, sizeof(buffer));
   snprintf(buffer, sizeof(buffer), "The quick brown fox\n");

    printf("%lu\n", sizeof(buffer));
    printf("%lu\n", strlen(buffer));

    return 0;
}

int calc_filesize(const char* fileName) {

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error when trying to open file\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);       // seek to the end of the file
    long fileSize = ftell(file);    // the ftell() function obtains the 
                                    // current value of the file position 
                                    // indicator for the stream pointed to 
                                    // by stream.

    fclose(file);

    return fileSize;
}


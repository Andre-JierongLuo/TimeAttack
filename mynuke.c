#include <stdio.h>
#include <unistd.h>

#define LAUNCH_CODE "OMDNJXUOWKNDHZJA"
#define CODE_LEN 16

int my_strncmp(char* s1, char* s2, int num) {
    unsigned char* us1 = (unsigned char*) s1;
    unsigned char* us2 = (unsigned char*) s2;

    for (int i = 0; i < num; ++i) {
        usleep(10000);  // Make things a little easier.
        if (us1[i] < us2[i])
            return -1;
        else if (us1[i] > us2[i])
            return 1;
        else if (!us1[i])
            return 0;
    }
    return 0;
}

int main(int argc, char** argv) {
    char input[CODE_LEN + 2];  // \n\0

    printf("ENTER LAUNCH CODE: ");
    if (fgets(input, CODE_LEN + 1, stdin) == NULL) return 1;

    if (my_strncmp(LAUNCH_CODE, input, CODE_LEN) == 0) {
        printf("LAUNCHING NUKES!\n");
        return 0;
    }
    printf("Access Denied.\n");
    return 1;
}

#include <stdio.h>
#include <string.h>

int check_login(void) {
    int authenticated = 0;
    char password[16];

    printf("password      is at %p\n", (void*)password);
    printf("authenticated is at %p\n", (void*)&authenticated);

    printf("Password: ");
    scanf("%s", password);  // no width limit

    if (strcmp(password, "hunter2") == 0) authenticated = 1;

    return authenticated;
}

int main(void) {
    if (check_login())
        printf("logged in\n");
    else
        printf("access denied\n");

    return 0;
}

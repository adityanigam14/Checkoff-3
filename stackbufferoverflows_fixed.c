#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <input string>\n", argv[0]);
        return 1;
    }

    int is_admin = 0;
    char buf[16];

    printf("buf      is at %p\n", (void*)buf);
    printf("is_admin is at %p\n", (void*)&is_admin);

    snprintf(buf, sizeof buf, "%s", argv[1]);

    printf("is_admin = %d\n", is_admin);
    return 0;
}

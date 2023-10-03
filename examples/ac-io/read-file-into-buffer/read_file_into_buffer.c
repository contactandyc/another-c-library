#include "another-c-library/ac_io.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s <input>\n", argv[0]);
        return -1;
    }

    size_t length;
    char *buffer = ac_io_read_file(&length, argv[1]);
    if(buffer) {
        printf( "%s", buffer );
        ac_free(buffer);
    }
    return 0;
}

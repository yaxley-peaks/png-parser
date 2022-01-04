//a png parser.
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNG_SIG_CAP 8
const uint8_t exp_sig[PNG_SIG_CAP] = {137, 80, 78, 71, 13, 10, 26, 10};

void read_bytes_or_panic(FILE *file, void *buf, size_t buf_cap) {
    size_t n = fread(buf, buf_cap, 1, file);
    if (n != 1) {
        if (ferror(file)) {
            fprintf(stderr, "ERROR: Could not read PNG header: %s\n", strerror(errno));
            exit(1);
        } else if (feof(file)) {
            fprintf(stderr, "ERROR: Could not read PNG header: Reached end of file");
        } else {
            assert(0 && "unreacheable");
        }
    }
}
void print_bytes(uint8_t *buf, size_t buf_cap) {
    for (size_t i = 0; i < buf_cap; i++) {
        printf("%u ", buf[i]);
    }
    printf("\n");
}

void reverse_bytes(void *buf0, size_t buf_cap) {
    uint8_t* buf = buf0;
    for (size_t i = 0; i < buf_cap / 2; ++i) {
        uint8_t t = buf[i];
        buf[i] = buf[buf_cap - i - 1];
        buf[buf_cap - i - 1] = t;
    }
}
int main(int argc, char **argv) {
    (void)argc;
    assert(*argv != NULL);
    char *program = *argv++;
    if (*argv == NULL) {
        fprintf(stderr, "Usage: %s <input.png>\n", program);
        fprintf(stderr, "ERROR: no input file provided\n");
        exit(1);
    }
    char *input_file_path = *argv++;
    printf("inspected file is %s\n", input_file_path);
    FILE *input_file = fopen(input_file_path, "rb");

    if (input_file == NULL) {
        fprintf(stderr, "ERROR: could not open file %s: %s\n", input_file_path, strerror(errno));
        exit(1);
    }

    //first 8 bytes in a png is a signature.
    //will always be 137 80 78 71 13 10 26 10
    uint8_t sig[PNG_SIG_CAP];
    read_bytes_or_panic(input_file, sig, PNG_SIG_CAP);
    if (memcmp(sig, exp_sig, PNG_SIG_CAP) != 0) {
        fprintf(stderr, "ERROR: Possible file corruption\n");
        exit(1);
    }

    print_bytes(sig, PNG_SIG_CAP);

    //32 bit int for chunk size
    //the bytes are reversed
    uint32_t chunk_sz;
    read_bytes_or_panic(input_file, &chunk_sz, sizeof(chunk_sz));
    reverse_bytes(&chunk_sz, sizeof(chunk_sz));
    printf("chunk size: %u\n", chunk_sz);
    fclose(input_file);
    return 0;
}
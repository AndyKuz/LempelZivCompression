#include "word.h"
#include "trie.h"
#include "io.h"
#include "code.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int bit_length(uint16_t code) {
    int num_bits = 0;

    uint16_t temp_code = 0;
    temp_code |= code;

    while (temp_code != 0) {
        temp_code = temp_code >> 1;
        num_bits += 1;
    }

    return num_bits;
}

int main(int argc, char *argv[]) {
    int opt = 0;

    bool print_output = false;
    int infile_int = STDIN_FILENO;
    int outfile_int = STDOUT_FILENO;

    while ((opt = getopt(argc, argv, "i:o:v")) != -1) {
        switch (opt) {
        case 'i':
            infile_int = open(optarg, O_RDONLY);
            if (infile_int == -1) {
                fprintf(stderr, "Error opening infile\n");
                exit(0);
            }
            break;
        case 'o':
            outfile_int = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            if (outfile_int == -1) {
                fprintf(stderr, "Error opening outfile\n");
                exit(0);
            }
            break;
        case 'v': print_output = true; break;
        }
    }
    FileHeader header;

    read_header(infile_int, &header);

    // following code is based of sudo code provided in asgn6.pdf
    WordTable *table = wt_create();
    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;
    uint16_t next_code = START_CODE;

    while (read_pair(infile_int, &curr_code, &curr_sym, bit_length(next_code))) {
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        write_word(outfile_int, table[next_code]);
        next_code = next_code + 1;

        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    flush_words(outfile_int);

    if (print_output == true) {

        uint64_t compressed_size = total_bits / 8;
        uint64_t uncompressed_size = total_syms;

        double space_saving
            = 100.00 * (1.00 - ((double) compressed_size / (double) uncompressed_size));

        fprintf(stderr, "Compressed file size: %" PRId64 "bytes\n", compressed_size);
        fprintf(stderr, "Uncompressed file size: %" PRId64 "bytes\n", uncompressed_size);
        fprintf(stderr, "Space saving: %.2f%%\n", space_saving);
    }

    close(infile_int);
    close(outfile_int);
}

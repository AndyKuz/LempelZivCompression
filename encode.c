#include "word.h"
#include "trie.h"
#include "io.h"
#include "code.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

    struct stat filestat;
    FileHeader header;
    memset(&header, 0, sizeof(FileHeader));

    header.magic = MAGIC;

    fstat(infile_int, &filestat);
    header.protection = filestat.st_mode;

    write_header(outfile_int, &header);

    // the following code is based of sudo code provided in asgn6.pdf
    TrieNode *root = trie_create();
    TrieNode *curr_node = root;
    TrieNode *prev_node = NULL;
    uint8_t curr_sym = 0;
    uint8_t prev_sym = 0;
    uint16_t next_code = START_CODE;

    while (read_sym(infile_int, &curr_sym)) {
        TrieNode *next_node = trie_step(curr_node, curr_sym);
        if (next_node != NULL) {
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            write_pair(outfile_int, curr_node->code, curr_sym, bit_length(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code = next_code + 1;
        }

        if (next_code == MAX_CODE) {
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }
        prev_sym = curr_sym;
    }
    if (curr_node != root) {
        write_pair(outfile_int, prev_node->code, prev_sym, bit_length(next_code));
        next_code = (next_code + 1) % MAX_CODE;
    }
    write_pair(outfile_int, STOP_CODE, 0, bit_length(next_code));
    flush_pairs(outfile_int);

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

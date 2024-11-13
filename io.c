#include "io.h"
#include "endian.h"

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

uint64_t total_syms = 0;
uint64_t total_bits = 0;

// used by read_pair, write_pair, and flush_pairs
uint8_t buf_one[BLOCK] = { 0 };
static int bits_index_one = 0;
static int buf_one_filled = 0;

// used by read_sym, write_word, and flush_words
uint8_t buf_two[BLOCK] = { 0 };
static int bits_index_two = 0;
static int buf_two_filled = 0;

int read_bytes(int infile, uint8_t *buf, int to_read) {
    // continously call read(to_read - already_read)
    // read() returning 0 indicates EOF
    int already_read = 0;
    ssize_t curr_read = 0;

    do {
        curr_read = read(infile, buf + already_read, to_read - already_read);
        already_read += curr_read;
        total_syms += curr_read;
    } while (curr_read != 0);

    return already_read;
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    //write bytes from buf to outfile
    //call write() until curr_written == 0
    int already_written = 0;
    ssize_t curr_written = 0;

    do {
        curr_written = write(outfile, buf + already_written, to_write - already_written);
        already_written += curr_written;
        total_bits += curr_written * 8;
    } while (curr_written != 0);

    return already_written;
}

void read_header(int infile, FileHeader *header) {
    //read header from infile -> header
    //use sizeof()
    //headers are little-endian so swap bytes if system is big-endian
    //if magic number is incorrect print error
    read_bytes(infile, (uint8_t *) header, sizeof(header));
    if (big_endian()) {
        swap32(header->magic);
        swap16(header->protection);
    }
    assert(header->magic == MAGIC);
}

void write_header(int outfile, FileHeader *header) {
    write_bytes(outfile, (uint8_t *) header, sizeof(header)); // write bytes to outfile from header
    if (big_endian()) { // checks if big endian if so swaps to little endian
        swap32(header->magic);
        swap16(header->protection);
    }
    assert(header->magic == MAGIC);
}

bool read_sym(int infile, uint8_t *sym) {
    int bytes_read = 0;

    *sym = 0;
    if (bits_index_two / 8 >= buf_two_filled) {
        bytes_read = read_bytes(infile, buf_two + (bits_index_two / 8), BLOCK);

        buf_two_filled += bytes_read;
        if (bytes_read == 0) {
            return false;
        }
    }
    int read_byte = buf_two[bits_index_two / 8];
    *sym = read_byte;
    bits_index_two += 8;
    return true;
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    for (int i = 0; i < bitlen; i++) {
        int lsb = (code >> i) & 1;
        buf_one[bits_index_one / 8] = buf_one[bits_index_one / 8] | (lsb << (bits_index_one % 8));

        bits_index_one++;
        buf_one_filled++;

        if (bits_index_one / 8 == BLOCK) {
            write_bytes(outfile, buf_one, BLOCK);

            memset(buf_one, 0, sizeof(buf_one));
            bits_index_one = 0;
            buf_one_filled = 0;
        }
    }
    for (int i = 0; i < 8; i++) {
        int lsb = (sym >> i) & 1;
        buf_one[bits_index_one / 8] = buf_one[bits_index_one / 8] | (lsb << (bits_index_one % 8));
        bits_index_one++;
        buf_one_filled++;

        if (bits_index_one / 8 == BLOCK) {
            write_bytes(outfile, buf_one, BLOCK);

            memset(buf_one, 0, sizeof(buf_one));
            bits_index_one = 0;
            buf_one_filled = 0;
        }
    }
}

void flush_pairs(int outfile) {
    if (bits_index_one % 8 == 0) {
        write_bytes(outfile, buf_one, bits_index_one / 8);
    } else {
        write_bytes(outfile, buf_one, bits_index_one / 8 + 1);
    }
    memset(buf_one, 0, sizeof(buf_one));

    bits_index_one = 0;
    buf_one_filled = 0;
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    int bytes_read = 0;

    *code = 0;
    for (int i = 0; i < bitlen; i++) {
        if (bits_index_one / 8 >= buf_one_filled) {
            bytes_read = read_bytes(infile, buf_one + (bits_index_one / 8), BLOCK);
            buf_one_filled += bytes_read;

            if (bytes_read == 0) {
                return false;
            }
        }

        int read_byte = buf_one[bits_index_one / 8];
        int read_bit = (read_byte >> (bits_index_one % 8)) & 1;
        *code = *code | (read_bit << i);

        bits_index_one += 1;
    }

    total_syms += 1;
    *sym = 0;
    for (int i = 0; i < 8; i++) {
        if (bits_index_one / 8 >= buf_one_filled) {
            bytes_read = read_bytes(infile, buf_one + (bits_index_one / 8), BLOCK);
            buf_one_filled += bytes_read;

            if (bytes_read == 0) {
                return false;
            }
        }
        int read_byte = buf_one[bits_index_one / 8];
        int read_bit = (read_byte >> (bits_index_one % 8)) & 1;
        *sym = *sym | (read_bit << i);

        bits_index_one += 1;
    }

    if (*code == 0) {
        return false;
    }
    return true;
}

void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; i++) {
        if (bits_index_two / 8 >= BLOCK) {
            flush_words(outfile);
        }
        buf_two[bits_index_two / 8] = w->syms[i];
        bits_index_two += 8;
        buf_two_filled += 1;
    }
}

void flush_words(int outfile) {
    write_bytes(outfile, buf_two, bits_index_two / 8);

    memset(buf_two, 0, sizeof(buf_two));

    bits_index_two = 0;
    buf_two_filled = 0;
}

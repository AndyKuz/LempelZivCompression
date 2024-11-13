# Assignment 7: Lempel-Ziv Compression and Decompression

This project implements Lempel-Ziv compression and decompression for files. Two Abstract Data Types (ADTs), Tries and Word Tables, are used to compress and decompress data efficiently. By utilizing buffers, the program can perform clean compression and decompression on any data passed to the executables, `encode` and `decode`.

## Build

To build this project, use the following commands:

- `$ make` or `$ make all`: Builds both executables, `encode` and `decode`.
- `$ make encode`: Builds only the `encode` executable.
- `$ make decode`: Builds only the `decode` executable.

## Cleaning

To delete all executables and `.o` files, run:

- `$ make clean`

## Running the Program

1. **Build both executables** (`encode` and `decode`) as described in the **Build** section.
2. **Prepare a file** with the text or data to be compressed.

### Encoding
Run the following command to compress a file:

```bash
$ ./encode -i <input_file_name> -o <output_file_name> [-v]
```

- `-i <input_file_name>`: Specifies the input file to be compressed.
- `-o <output_file_name>`: Specifies the output file where compressed data will be stored.
- `-v`: (Optional) Prints extra statistics about the compression.

### Decoding
Once you've compressed the file, run the following command to decompress it:

```bash
$ ./decode -i <output_file_name> -o <final_output_file_name> [-v]
```

- `-i <output_file_name>`: Specifies the compressed file to decode.
- `-o <final_output_file_name>`: Specifies the file where the decompressed data will be stored.
- `-v`: (Optional) Prints extra statistics about the decompression.

After these steps,`<final_output_file_name>` should contain the decompressed version of the original file.

Note: If you don’t specify a file for `-i` (input) or `-o` (output), the program will default to `stdin` for input and `stdout` for output.

## Error Information
When running `$ make scan-build` to detect errors, you may encounter the following warning:

`word.c:48:24: warning: Result of 'malloc' is converted to a pointer of type 'WordTable', which is incompatible with sizeof operand type 'Word' [unix.MallocSizeof]`

This warning occurs because a `Word Table` is essentially a list of `Words`. When initializing `wt` (a pointer to a `Word Table`), we allocate memory for a list of `Words` up to `MAX_CODE`, which is intentional and does not impact functionality.
#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname; // File name of the stego image
    FILE *fptr_stego_image;  // File pointer for the stego image
    int magic_string_len;    // Length of the magic string used for encoding
    char magic_string[10];   // Buffer to store the magic string (max length 10)

    int secret_file_extsize; // Size of the secret file extension
    char secret_file_ext[100]; // Buffer to store the secret file extension (max length 100)

    long s_file_size;        // Size of the secret file
    char s_file_data[1000];  // Buffer to store the secret file data (max length 1000)
    char output_fname[100];  // Output file name
    FILE *fptr_output;       // File pointer for the output file
} DecodeInfo;


/* Perform the decoding process */
Status do_decoding(char *argv[], DecodeInfo *decInfo);
/* Open input and output files based on DecodeInfo structure */
Status dopen_files(DecodeInfo *decInfo);
/* Extract and store the magic string from the stego image */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);
/* Extract the size of the secret file extension from the stego image */
Status decode_secret_file_ext_size(char *secret_file_ext,DecodeInfo*decInfo);
/* Extract the size of the secret file from the stego image */
Status decode_secret_file_size(long s_file_size,DecodeInfo*decInfo);
/* Extract the secret file data from the stego image */
Status decode_secret_file_data(char *s_file_data,DecodeInfo*decInfo);
/* Decode an integer from the least significant bits of an image buffer */
int decode_lsb_to_int(char *image_buffer);
/* Decode a byte from the least significant bits of an image buffer */
char decode_lsb_to_byte(char *image_buffer);
#endif

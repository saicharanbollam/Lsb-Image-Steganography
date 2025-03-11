//Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
//declaring global variables
long file_size;	// Size of the secret file
char *sec_ext;	// Extension of the secret file
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}


/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

 /* - Extracts and validates file extensions for the source image and secret file.
 * - Validates the optional output file extension and sets a default if not provided.
 * - Ensures that the source image is a BMP file and the secret file is either a .c or .txt file.
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *src_ext=strchr(argv[2], '.');	// Extract file extension of source image
    sec_ext=strchr(argv[3], '.');	// Extract file extension of secret file
    if(sec_ext == NULL|| src_ext == NULL)
    {
	    // Print usage if extensions are missing
	printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        return e_failure;
    }
     // Validate the optional output file extension
    if(argv[4] != NULL)
    {
                if(strstr(argv[4],".bmp") != NULL)
		{
			char *token=strchr(argv[4],'.');
			if(strcmp(token,".bmp")==0)
			{
				encInfo->stego_image_fname = (char *)malloc(strlen(argv[4]) + 1);
			 	strcpy(encInfo->stego_image_fname,argv[4]);
			}
			else
			{
				// Print usage if output file is not a .bmp
				printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
                        	return e_failure;
			}
		}
		else
		{
			 // Print usage if output file does not have .bmp extension
			printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
                        return e_failure;
		}

    }
    else
    {
	    // Set default output file if not specified
                printf("INFO: Output File not mentioned. Creating output.bmp as default\n");
                encInfo->stego_image_fname = "output.bmp";
    }
    if (strcmp(argv[0], "./a.out") == 0 &&
        strcmp(argv[1], "-e") == 0 &&
        strcmp(src_ext, ".bmp") == 0 &&    // Check for .bmp file extension
        (strcmp(sec_ext,".c") == 0 || strcmp(sec_ext, ".txt") == 0))  // Check for .c or .txt
    {
        return e_success;
    }
    else 
    {
	     // Print usage if arguments are invalid
	printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        return e_failure;
    }
}

  /*Opens the source image, secret file, and stego image files.
 * - Reports errors if any files cannot be opened.
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    printf("INFO: Opening required files\n");
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        printf("The Source file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened SkeletonCode/beautiful.bmp\n");

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        printf("The Secret file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened secret.txt\n");


    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        printf("The Stego image file is not opened\n");
        return e_failure;
    }
    printf("INFO: Opened steged_img.bmp\n");
    // No failure return e_success
    return e_success;
}

/* - Validates the encoding arguments and opens the necessary files.
 * - Checks if the secret file is empty and verifies if the image has sufficient capacity.
 * - Copies BMP header, encodes magic string, file extension, file size, and data into the image.
 * - Copies remaining image data and closes all files.
 */
Status do_encoding(char* argv[], EncodeInfo *encInfo)
{
    if(read_and_validate_encode_args(argv, encInfo) != e_success)
    {
        printf("file of read and validate not done sucessfully\n");
        return e_failure;
    }
    // Open files
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Failed to open files.\n");
        return e_failure;
    }
    printf("INFO: ## Encoding Procedure Started\n");
    //checking the secret file is empty or not
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    file_size = secret_file_size(encInfo);
    if(file_size!=0)
    {
	    printf("INFO: Done.Not Empty\n");
    }
    else
    {
	    printf("ERROR: %s file is empty\n",encInfo->secret_fname);
	    return e_failure;
    }

    //Check Capacity
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(check_capacity(encInfo) != e_success)
    {
	    printf("ERROR:%s does not have capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
	    return e_failure;
    }

	printf("INFO: Done. Found OK\n");

    // Copy BMP header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy BMP header.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    // Encode magic string
    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        printf("Failed to encode magic string.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    // Encode file extension size and file extension
    if (encode_secret_file_extn(sec_ext,encInfo) !=e_success)
    //if (encode_secret_file_ext_size(encInfo) != e_success)
    {
        printf("Failed to encode file extension size.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    // Encode secret file size
    if(encode_secret_file_size(file_size,encInfo) !=e_success)
    {
	    printf("Error \n");
	    return e_failure;
    }
     printf("INFO: Done\n");
    // Encode secret file data
    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Failed to encode secret file data.\n");
        return e_failure;
    }
     printf("INFO: Done\n");
    // Copy remaining image data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy remaining image data.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    printf("INFO: ## Encoding Done Successfully\n");
    return e_success;
}
// Checks if the BMP image has enough capacity to encode the secret file data
Status check_capacity(EncodeInfo *encInfo)
{
	// Get the total image capacity in bytes
	int image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	//printf("Image capacity %d\n",image_capacity);
	// Calculate the total number of bits needed for encoding:
    	// 54 bytes for the BMP header + 4 bytes for magic string length
    	// + length of the magic string + 4 bytes for file extension size
    	// + length of the file extension + 4 bytes for file size
    	// + size of the secret file * 8 bits per byte
	//encoding things
	int encoding_things = 54+(4+(strlen(MAGIC_STRING))+4+(strlen(sec_ext))+4+(file_size))*8;
	//printf("Encoding_things %d\n",encoding_things);
	
	// Check if image capacity is sufficient for encoding
	if(image_capacity > encoding_things)
		return e_success;	// Capacity is sufficient
	else
		return e_failure;	// Capacity is insufficient
	
}

// Copies the BMP header from the source image file to the destination image file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);	// Rewind source file to start
    printf("INFO: Copying Image Header\n");
    char buf[54];	// Buffer to hold BMP header (54 bytes)
    fread(buf, 54, 1, fptr_src_image);	// Read header from source
    fwrite(buf, 54, 1, fptr_dest_image); // Write header to destination
    return e_success;
}

// Encodes an integer into the least significant bits of an image buffer
Status encode_int_to_lsb(int data, char *image_buffer)
{
    int index = 0;
    for (int i = 31; i >= 0; i--)
    {
	    // Set or clear the least significant bit based on data
        if (data & (1 << i))
        {
            image_buffer[index] = image_buffer[index] | 1;
        }
        else
        {
            image_buffer[index] = image_buffer[index] & (~1);
        }
        index++;
    }
    return e_success;
}

// Encodes a byte into the least significant bits of an image buffer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int index = 0;
    for (int i = 7; i >= 0; i--)
    {
	    // Set or clear the least significant bit based on data
        if (data & (1 << i))
        {
            image_buffer[index] = image_buffer[index] | 1;
        }
        else
        {
            image_buffer[index] = image_buffer[index] & (~1);
        }
        index++;
    }
    return e_success;
}

// Encodes the magic string (a signature) into the BMP image file
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string); 
    //printf("Encoding magic string: %s\n", MAGIC_STRING);
    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(len, buf);
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);
    printf("INFO: Encoding Magic String Signature\n");
    char buffer[8];
    for (int i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

// Encodes the secret file extension into the BMP image file
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int ext_size=strlen(file_extn);
    //printf("Secret ext size: %d bytes\n", ext_size);
    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(ext_size, buf);
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);

    printf("INFO: Encoding secret.txt File Extension\n");
    char buffer[8];
    for (int i = 0; i < ext_size; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}
// Determines the size of the secret file
Status secret_file_size(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_END);	// Move to end of file
    file_size = ftell(encInfo->fptr_secret);	// Get file size
    rewind(encInfo->fptr_secret);	// Rewind file pointer to start
    return file_size;	// Return the size

}
// Encodes the secret file size into the BMP image file
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Size\n");
    //printf("Secret file size: %ld bytes\n", file_size);
    int index = 0;
    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, buf);
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}
// Encodes the data from the secret file into the BMP image file
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Data\n");
    char ch;
    char buffer[8];
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)
    {
        //printf("%c",ch);
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);

    }
    return e_success;
}
// Copies the remaining image data from the source file to the destination file
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    int c;
    while((c=fgetc(fptr_src))!=EOF)	// Read each byte from source image
    {
        fputc(c,(fptr_dest));	// Write byte to destination image
    }
        //printf("copying done\n");
    return e_success;
}



//Header files inclusion
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Function to read and validate command-line arguments for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	// Check if the file extension of argv[2] is .bmp
	char *bmp_ext = strchr(argv[2], '.');
    	if (bmp_ext == NULL || strcmp(bmp_ext, ".bmp") != 0)
    	{
	        printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
        	return e_failure;
    	}
	// If output file name is provided, extract the base name
	if ( argv[3] != NULL )
        {
		char str[100];
		strcpy(str,argv[3]);
		char *token=strtok(str,".");
                strcpy(decInfo->output_fname,token);
		
        }
        else
        {
		// Default output file name if not provided
		printf("INFO: Output File not mentioned. Creating output as default\n");
                strcpy(decInfo->output_fname,"output");
        }

	// Validate command-line arguments
	if (strcmp(argv[0], "./a.out") == 0 && strcmp(argv[1], "-d") == 0)
    	{
        	return e_success;
    	}	
    	else
    	{
	        printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
        	return e_failure;
    	}
        
}

// Function to open the required files for decoding
Status dopen_files(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started\n");
    printf("INFO: Opening required files\n");
    printf("INFO: Opened steged_img_.bmp\n");
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("The Stego image file is not opened because u not giving correct bmp file\n");
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Main function to perform the decoding process
Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
     // Validate the decoding arguments
    if(read_and_validate_decode_args(argv, decInfo) != e_success)
    {	
	printf("file of read and validate not done sucessfully\n");
	return e_failure;
    }
    // Open files
    if (dopen_files(decInfo) != e_success)
    {
        printf("Failed to open files.\n");
        return e_failure;
    }
    // Decode magic string
    if (decode_magic_string(decInfo->magic_string,decInfo) != e_success)
    {
        printf("Failed to decode magic string or magic string not equal.\n");
        return e_failure;
    }
    // Decode secret file extension
    printf("INFO: Done\n");
    if(decode_secret_file_ext_size(decInfo->secret_file_ext, decInfo)!= e_success)
    {
	    printf("Failed to decode secret file ext\n");
	    return e_failure;
    }
    printf("INFO: Done\n");
    // Decode secret file size
    if(decode_secret_file_size(decInfo->s_file_size, decInfo)!= e_success)
    {

	    printf("Failed to decode secret file size\n");
	    return e_failure;
    }
    printf("INFO: Done\n");
    // Decode secret file data
    if(decode_secret_file_data(decInfo->s_file_data, decInfo)!=e_success)
    {

	    printf("Failed to decode secret file data\n");
	    return e_failure;
     }
    printf("INFO: Done\n");
    printf("INFO: ## Decoding Done Successfully\n");
    return e_success;
}

// Function to decode the magic string from the image
Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);  
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);
    decInfo->magic_string_len = decode_lsb_to_int(buf);
    //printf("Magic length: %d\n",decInfo->magic_string_len);
    
    char buffer[8];
    for (int i = 0; i < decInfo->magic_string_len; i++) 
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->magic_string[i] = decode_lsb_to_byte(buffer);
    }
    decInfo->magic_string[decInfo->magic_string_len] = '\0';
    //printf("Magic string: %s\n",decInfo->magic_string);

    // Compare decoded magic string with expected value
    if(strcmp(MAGIC_STRING,decInfo->magic_string) != 0)
    {
	    printf("Magic string is not equal\n");
	    return e_failure;
    }
    return e_success;
}

// Function to decode least significant bits (LSB) to an integer
int decode_lsb_to_int(char *image_buffer)
{
    int result = 0;
    for (int i = 0; i < 32; i++)
    {
      result <<= 1;
      result |= image_buffer[i] & 1; 
        
    }
    return result;
}

// Function to decode least significant bits (LSB) to a byte
char decode_lsb_to_byte(char *image_buffer)
{
    char ch=0;
    for (int i = 0; i <8; i++)
    {
        ch <<= 1 ;
	ch |= image_buffer[i] & 1;   
    }
    return ch;
}
// Function to decode the secret file extension size and name
Status decode_secret_file_ext_size(char * secret_file_ext,DecodeInfo*decInfo)
{
    printf("INFO: Decoding Output File Extension\n");
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);
    decInfo->secret_file_extsize = decode_lsb_to_int(buf);
    //printf("Secret File extsize: %d\n",decInfo->secret_file_extsize);
    
    char buffer[8];
    for (int i = 0; i < decInfo->secret_file_extsize; i++) 
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->secret_file_ext[i] = decode_lsb_to_byte(buffer);
    }
    decInfo->secret_file_ext[decInfo->secret_file_extsize] = '\0';
    strcat(decInfo->output_fname,secret_file_ext);
    printf("INFO: Opened %s\n",decInfo->output_fname);
    printf("INFO: Done.Opened all required files\n");

     // Open the output file for writing
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    //printf("Secret_file_ext: %s\n",decInfo->secret_file_ext);
    return e_success;
}

//Function to decode the secret file size
Status decode_secret_file_size(long s_file_size, DecodeInfo *decInfo)
{
    printf("INFO: Decoding File Size\n");
    char buf[32];
    fread(buf, 32, 1, decInfo->fptr_stego_image);
    decInfo->s_file_size = (long)decode_lsb_to_int(buf);
    //printf("Secret_file_size : %ld\n",decInfo->s_file_size);
    return e_success;
}
// Function to decode the secret file data and write it to the output file
Status decode_secret_file_data(char *s_file_data, DecodeInfo *decInfo)
{
    printf("INFO: Decoding File Data\n");
    char buffer[8];
    for(int i = 0; i < decInfo->s_file_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->s_file_data[i] = decode_lsb_to_byte(buffer);
        putc(decInfo->s_file_data[i],decInfo->fptr_output);
    }
    decInfo->s_file_data[decInfo->s_file_size] = '\0'; 
    //printf("Secret file data: %s", decInfo->s_file_data);    
    return e_success;
}


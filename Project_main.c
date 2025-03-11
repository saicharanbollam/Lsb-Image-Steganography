/*
NAME: B SAI CHARAN REDDY
DATE: 18/09/2024
DESCRIPTION: Hiding Data in Image Using  Image Steganography
*/
//Header files
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char*argv[]) 
{
    // Check if the number of arguments is less than or equal to 1
    if(argc<=1)
    {
	     /*
             * Display usage instructions if not enough arguments are provided.
             * The program expects the user to provide valid command-line options for either encoding or decoding.
             */
	    printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
	    printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
	    return e_failure;
    }

    // Check if the user passed the "-e" flag for encoding
    if(strcmp(argv[1],"-e")==0)
    {
    	EncodeInfo encInfo;
    	encInfo.src_image_fname = argv[2];   // Assign source BMP file name
    	encInfo.secret_fname = argv[3];     // Assign secret text file name
	// Check if the necessary arguments for encoding are provided
        if(argc<4)
    	{
	    printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        	return e_failure;
    	}
	// Proceed with encoding by calling the do_encoding function
	do_encoding(argv,&encInfo);
        //encoding
    }
    // Check if the user passed the "-d" flag for decoding
    else if(strcmp(argv[1],"-d")==0)
    {
        DecodeInfo decInfo;
    	decInfo.stego_image_fname = argv[2];	// Assign stego BMP file name
	if(argc<3)
        {
	    printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
                return e_failure;
        }
	// Proceed with decoding by calling the do_decoding function
	do_decoding(argv, &decInfo);
	//decoding
    }
    else
    {
	    // If the user provides invalid options that are neither "-e" nor "-d", display an error message.
	    printf("unsupported argument\n");
	    return e_failure;
    }
     

    return 0;
}

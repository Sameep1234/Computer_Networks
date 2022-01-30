/*
    Name - Sameep Vani
    Roll No. - AU1940049
    Question - 
    1. Modify the code so that a user can supply the filename from command line using -i filename switch.
    Further, if the user does not supply the filename, your program should attempt to read sample.txt by default.
    2. Finally, modify the code to copy an input file to an output file. The user may supply the output filename using -o 
    outfile switch, else output the copy to sample_out.txt by default.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    /* Pointer to the source file */
    FILE *src;
    /* File is read one character at a time*/
    char c;
    /* Get Arguments */
    char *fileName="sample.txt", *outputFileName = "sample_output.txt";
    while((c = getopt(argc, argv, "i:o:")) != -1)
    {
        switch(c)
        {
            case 'i': /* Optional argument for input filename */
                fileName = optarg; /* Store the input filename */
                break;
            case 'o': /* Optional argument for output filename */
                outputFileName = optarg; /* Store the output filename */
                break;
            default:
                break;
        }
    }
    /* Opening source file in read mode */
    src = fopen(fileName, "r");

    /* Display error when file is not found or does not exist. */
    if (src == NULL)
    {
        printf("File not found. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    /* Read src until end-of-file char is encountered */
    /* Open Output file in write mode */
    FILE *outputFilePointer = fopen(outputFileName, "w");
    while ((c = fgetc(src)) != EOF)
    {
        /* Write individual char to the output file */
        fprintf(outputFilePointer, "%c", c);
    }

    /* Close all the files. */
    fclose(outputFilePointer);
    fclose(src);
    return 0;
}
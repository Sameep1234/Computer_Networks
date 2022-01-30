/*
    Name - Sameep Vani
    Roll No. - AU1940049
    Question - Write a C program to create and write into a text file using FILE pointer.
*/

/* Import Required header files. */
#include<stdio.h>
#include<unistd.h>

int main()
{
    /* Strings for text and file name. */
    char *textToWrite = "Hi, My Name is Sameep Vani";
    char *fileToWrite = "temp.txt";

    /* Open file in write mode */
    FILE *fp = fopen(fileToWrite, "w");

    /* Write the string in the file. */
    fprintf(fp, "%s", textToWrite);

    /* Close the files. */
    fclose(fp);
}
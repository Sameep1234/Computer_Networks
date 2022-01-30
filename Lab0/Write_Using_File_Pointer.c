/*
    Name - Sameep Vani
    Roll No. - AU1940049
    Question - Write a C program to create and write into a text file using FILE pointer.
*/

#include<stdio.h>
#include<unistd.h>

int main()
{
    char *textToWrite = "Hi, My Name is Sameep Vani";
    char *fileToWrite = "temp.txt";

    FILE *fp = fopen(fileToWrite, "w");
    char c;
    fprintf(fp, "%s", textToWrite);
    fclose(fp);
}
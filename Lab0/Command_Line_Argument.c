/*
    Name - Sameep Vani
    Roll number - AU1940049
    Question - Understand the code and complete the assignment stated in
    the skeleton code command_line_argument.c
*/

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(void);

int main(int argc, char **argv)
{
  /* Default Number Value */
  int num = 2;
  /* Default Character Value */
  char disp = '*';
  char c;

  /* Get optional arguments using getopt() function */
  while ((c = getopt(argc, argv, "n:d:")) != -1)
    switch (c) /* Switch Case for different optional arguments */
    {
      case 'n': 
        num = atoi(optarg); /* Convert Char to int as the first argument is 'n' */
        break;

      case 'd':
        disp = optarg[0]; /* Store the character to be printed */
        break;

      default:
        usage();
        exit(1);
    }
  /* Print the requirements */
  printf("number = %d, display char = %c\n", num, disp); 

  /* Princt disp, "n" number of times */
  for (int i = 0; i < num; i++)
    printf("%c\n", disp);

  return 0;
}

void usage(void)
{

  printf("Usage:\n");
  printf(" -n <repetitions>\n");
  printf(" -d <char to dispaly>\n");
  exit(8);
}
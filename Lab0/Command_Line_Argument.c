/*
    Name - Sameep Vani
    Roll number - AU1940049
    Question - Understand the code and complete the assignment stated in
    the skeleton code command_line_argument.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(void);

int main (int argc, char **argv){
  int num = 2;
  char disp = '*';
  char c;

  while ((c = getopt (argc, argv, "n:d:")) != -1)
    
    switch (c){
    
    case 'n':
      num = atoi(optarg);
      break;
      
    case 'd':
        disp = optarg[0];
        break;

    default:
      usage();
      exit (1);
      }
  
  printf ("number = %d, display char = %c\n",
          num, disp);
  
  for (int i = 0;i<num;i++)
    printf("%c\n", disp);
 
  return 0;
}

void usage(void){

  printf("Usage:\n");
  printf(" -n <repetitions>\n");
  printf(" -d <char to dispaly>\n");
  exit (8);
}
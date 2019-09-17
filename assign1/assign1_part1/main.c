/*
About this program:
- This program counts words.
- The specific words that will be counted are passed in as command-line
  arguments.
- The program reads words (one word per line) from standard input until EOF or
  an input line starting with a dot '.'
- The program prints out a summary of the number of times each word has
  appeared.
- Various command-line options alter the behavior of the program.

E.g., count the number of times 'cat', 'nap' or 'dog' appears.
> ./main cat nap dog
Given input:
 cat
 .
Expected output:
 Looking for 3 words
 Result:
 cat:1
 nap:0
 dog:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smp0_tests.h"

#define LENGTH(s) (sizeof(s) / sizeof(*s))
FILE *out;
/* Structures */
typedef struct {
  char *word;
  int counter;
} WordCountEntry;

/** 
* @brief     Searches the input stream for occurences of search words
**/
int process_stream(WordCountEntry entries[], int entry_count)
{
  short line_count = 0;
  char buffer[30];
  char* word;
  int i;
  while (fgets(buffer,30,stdin)) {
    if (*buffer == '.')
      break; 
    buffer[strlen(buffer) - 1] = '\0';//Accounting for enline character. Assumes only 1 word per line.
    /* Compare against each entry */
    word = strtok(buffer," ");
    while (word != NULL) {
      i = 0;
      while(i < entry_count) {
        if(!strcmp(entries[i].word, word))
          entries[i].counter++;
        i++;
      }
      word = strtok(NULL," ");
    }
    while (i < entry_count) {
      if (!strcmp(entries[i].word, buffer))
        entries[i].counter++;
      i++;
    }
    line_count++;
  }
  return line_count;
}


void print_result(WordCountEntry entries[], int entry_count)
{
  //FILE* out;
  fprintf(out,"Result:\n");
  while (entry_count-- > 0) {
    fprintf(out,"%s:%d\n", entries->word, entries->counter);
    entries++;//increment entries for as long as there are still entries
  }
}

/**
 * @brief   Prints how the function is used
 **/
void printHelp(const char *name)
{
  fprintf(stderr,"usage: %s [OPTION...] <word1> ... <wordN>\n\n", name);
  fprintf(stderr, "OPTION:\n");
  fprintf(stderr,"-h \t\t\t\t (For help)\n");
  fprintf(stderr,"-f, file=FILENAME\t\t (Outputs to te file FILENAME)\n");
  //fprintf(stderr,"usage: %s [-h] <word1> ... <wordN>\n", name);
}


int main(int argc, char **argv)
{
  const char *prog_name = *argv;
  char * filename;
  out = stdout;
  WordCountEntry *entries = (WordCountEntry*) malloc((argc - 1) * sizeof(WordCountEntry));
  int entryCount = 0;

  /* Entry point for the testrunner program */
  if (argc > 1 && !strcmp(argv[1], "-test")) {
    run_smp0_tests(argc - 1, argv + 1);
    free(entries);
    return EXIT_SUCCESS;
  }
  argv++; //Shift pointer to first word after program name
  while (*argv != NULL) {
    if (**argv == '-') {
      // "-" represents overide case to use specific function/test
      switch ((*argv)[1]) {
        case 'f':
          filename = *argv + 2;
          out = fopen(filename,"w");
          fprintf(stderr,"Outputting to file: %s\n",filename);
          break;
        case 'h':
          printHelp(prog_name);
          break;
        default:
          fprintf(stderr,"%s: Invalid option %s. Use -h for help.\n",
                 prog_name, *argv);
      }
    } else { //Start capturing words
      if (entryCount < argc - 1) {//No loner uses Length macro as sizeof cannot read dynamically allocated arrays
        entries[entryCount].word = *argv;
        entries[entryCount++].counter = 0;
      }
    }
    argv++;
  }
  if (entryCount == 0) {
    fprintf(stderr,"%s: Please supply at least one word. Use -h for help.\n",
           prog_name);
           free(entries);
    return EXIT_FAILURE;
  }
  if (entryCount == 1) {
    fprintf(out,"Looking for a single word\n");
  } else {
    fprintf(out,"Looking for %d words\n", entryCount);
  }

  process_stream(entries, entryCount);
  print_result(entries, entryCount);
  free(entries);
  return EXIT_SUCCESS;
}

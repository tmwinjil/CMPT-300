/* calc.c - Multithreaded calculator */

#include "calc.h"

pthread_t adderThread;
pthread_t degrouperThread;
pthread_t multiplierThread;
pthread_t readerThread;
pthread_t sentinelThread;

char buffer[BUF_SIZE];
int num_ops;


/* Utiltity functions provided for your convenience */

/* int2string converts an integer into a string and writes it in the
   passed char array s, which should be of reasonable size (e.g., 20
   characters).  */
char *int2string(int i, char *s)
{
    sprintf(s, "%d", i);
    return s;
}

/* string2int just calls atoi() */
int string2int(const char *s)
{
    return atoi(s);
}

/* isNumeric just calls isdigit() */
int isNumeric(char c)
{
    return isdigit(c);
}

/* End utility functions */


void printErrorAndExit(char *msg)
{
    msg = msg ? msg : "An unspecified error occured!";
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int timeToFinish()
{
    /* be careful: timeToFinish() also accesses buffer */
    return buffer[0] == '.';
}

/* Looks for an addition symbol "+" surrounded by two numbers, e.g. "5+6"
   and, if found, adds the two numbers and replaces the addition subexpression 
   with the result ("(5+6)*8" becomes "(11)*8")--remember, you don't have
   to worry about associativity! */
void *adder(void *arg)
{
    int bufferlen;
    int value1, value2;
    int startOffset, remainderOffset;
    int i;

    return NULL; /* remove this line */

    while (1) {
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
	    // do we have value1 already?  If not, is this a "naked" number?
	    // if we do, is the next character after it a '+'?
	    // if so, is the next one a "naked" number?

	    // once we have value1, value2 and start and end offsets of the
	    // expression in buffer, replace it with v1+v2
	}

	// something missing?
    }
}

/* Looks for a multiplication symbol "*" surrounded by two numbers, e.g.
   "5*6" and, if found, multiplies the two numbers and replaces the
   mulitplication subexpression with the result ("1+(5*6)+8" becomes
   "1+(30)+8"). */
void *multiplier(void *arg)
{
    int bufferlen;
    int value1, value2;
    int startOffset, remainderOffset;
    int i;

    return NULL; /* remove this line */

    while (1) {
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
	    // same as adder, but v1*v2
	}

	// something missing?
    }
}


/* Looks for a number immediately surrounded by parentheses [e.g.
   "(56)"] in the buffer and, if found, removes the parentheses leaving
   only the surrounded number. */
void *degrouper(void *arg)
{
    int bufferlen;
    int i;

    return NULL; /* remove this line */

    while (1) {

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
	    // check for '(' followed by a naked number followed by ')'
	    // remove ')' by shifting the tail end of the expression
	    // remove '(' by shifting the beginning of the expression
	}

	// something missing?
    }
}


/* sentinel waits for a number followed by a ; (e.g. "453;") to appear
   at the beginning of the buffer, indicating that the current
   expression has been fully reduced by the other threads and can now be
   output.  It then "dequeues" that expression (and trailing ;) so work can
   proceed on the next (if available). */
void *sentinel(void *arg)
{
    char numberBuffer[20];
    int bufferlen;
    int i;

    return NULL; /* remove this line */

    while (1) {

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
	    if (buffer[i] == ';') {
		if (i == 0) {
		    printErrorAndExit("Sentinel found empty expression!");
		} else {
		    /* null terminate the string */
		    numberBuffer[i] = '\0';
		    /* print out the number we've found */
		    fprintf(stdout, "%s\n", numberBuffer);
		    /* shift the remainder of the string to the left */
		    strcpy(buffer, &buffer[i + 1]);
		    break;
		}
	    } else if (!isNumeric(buffer[i])) {
		break;
	    } else {
		numberBuffer[i] = buffer[i];
	    }
	}

	// something missing?
    }
}

/* reader reads in lines of input from stdin and writes them to the
   buffer */
void *reader(void *arg)
{
    while (1) {
	char tBuffer[100];
	int currentlen;
	int newlen;
	int free;

	fgets(tBuffer, sizeof(tBuffer), stdin);

	/* Sychronization bugs in remainder of function need to be fixed */

	newlen = strlen(tBuffer);
	currentlen = strlen(buffer);

	/* if tBuffer comes back with a newline from fgets, remove it */
	if (tBuffer[newlen - 1] == '\n') {
	    /* shift null terminator left */
	    tBuffer[newlen - 1] = tBuffer[newlen];
	    newlen--;
	}

	/* -1 for null terminator, -1 for ; separator */
	free = sizeof(buffer) - currentlen - 2;

	while (free < newlen) {
		// spinwaiting
	}

	/* we can add another expression now */
	strcat(buffer, tBuffer);
	strcat(buffer, ";");

	/* Stop when user enters '.' */
	if (tBuffer[0] == '.') {
	    return NULL;
	}
    }
}


/* Where it all begins */
int smp3_main(int argc, char **argv)
{
    void *arg = 0;		/* dummy value */

    /* let's create our threads */
    if (pthread_create(&multiplierThread, NULL, multiplier, arg)
	|| pthread_create(&adderThread, NULL, adder, arg)
	|| pthread_create(&degrouperThread, NULL, degrouper, arg)
	|| pthread_create(&sentinelThread, NULL, sentinel, arg)
	|| pthread_create(&readerThread, NULL, reader, arg)) {
	printErrorAndExit("Failed trying to create threads");
    }

    /* you need to join one of these threads... but which one? */
    pthread_detach(multiplierThread);
    pthread_detach(adderThread);
    pthread_detach(degrouperThread);
    pthread_detach(sentinelThread);
    pthread_detach(readerThread);

    /* everything is finished, print out the number of operations performed */
    fprintf(stdout, "Performed a total of %d operations\n", num_ops);
    return EXIT_SUCCESS;
}

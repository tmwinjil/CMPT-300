/* calc.c - Multithreaded calculator */

#include "calc.h"

pthread_t adderThread;
pthread_t degrouperThread;
pthread_t multiplierThread;
pthread_t readerThread;
pthread_t sentinelThread;

//pthread_mutex_t lock;
sem_t lock;
char buffer[BUF_SIZE];
int num_ops;
char func_finished[3];
int change_made;
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

   // return NULL; /* remove this line */

    while (1) {
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen && buffer[i] != '\0'; i++) {
		if (buffer[i] == '+') {
			if(sem_wait(&lock))
					printErrorAndExit("Adder failed to lock mutex");
			if(isdigit(buffer[i - 1]) && isdigit(buffer[i + 1])) {
				startOffset = i - 1;
				remainderOffset = i + 1;
				int mult = 10;
				value1 = buffer[startOffset--] - '0';
				value2 = buffer[remainderOffset++] - '0';

				while(isdigit(buffer[startOffset])) {//Extract value1
					value1 += mult * (buffer[startOffset--] - '0');
					mult *= 10;
				}
				while(isdigit(buffer[remainderOffset])) //Extract value2
					value2 = (10 * value2) + (buffer[remainderOffset++] - '0');
				
				value1 += value2;

				while(value1 >= 1) { //Copy number from rightmost digit(result will always be shorter than experession)
					buffer[--remainderOffset] = '0' + (value1 % 10);
					value1 /= 10;
				}
				strcpy(&buffer[startOffset+1],&buffer[remainderOffset]);
				num_ops++;
				change_made = 1;
			}
			if(sem_post(&lock))
					printErrorAndExit("Adder failed to unlock mutex");
		}
	}
	func_finished[0] = '1';
	sched_yield();
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

   // return NULL; /* remove this line */

    while (1) {
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
		if (buffer[i] == '*') {
			if(sem_wait(&lock))
					printErrorAndExit("Multiplier failed to lock mutex");
			if(isdigit(buffer[i - 1]) && isdigit(buffer[i + 1])) {
				startOffset = i - 1;
				remainderOffset = i + 1;
				int mult = 10;
				value1 = buffer[startOffset--] - '0';
				value2 = buffer[remainderOffset++] - '0';

				while(isdigit(buffer[startOffset])) {//Extract value1
					value1 += mult * (buffer[startOffset--] - '0');
					mult *= 10;
				}
				while(isdigit(buffer[remainderOffset])) //Extract value2
					value2 = (10 * value2) + (buffer[remainderOffset++] - '0');
				
				value1 *= value2;

				while(value1 >= 1) { //Copy number from rightmost digit(result will always be shorter than experession)
					buffer[--remainderOffset] = '0' + (value1 % 10);
					value1 /= 10;
				}
				strcpy(&buffer[startOffset+1],&buffer[remainderOffset]); //shift result to appear where expression started
				num_ops++;
				change_made = 1;
			}
			if(sem_post(&lock))
					printErrorAndExit("Multiplier failed to unlock mutex");
		}
	    // do we have value1 already?  If not, is this a "naked" number?
	    // if we do, is the next character after it a '+'?
	    // if so, is the next one a "naked" number?

	    // once we have value1, value2 and start and end offsets of the
	    // expression in buffer, replace it with v1+v2
	}
	func_finished[1] = '1';
	sched_yield();
	// something missing?
    }
}


/**
 * @brief	Looks for a number immediately surrounded by parentheses [e.g."(56)"]
 *  		in the buffer and, if found, removes the parentheses (first ')' then '(') leaving only the
 *  		surrounded number. 
 **/
void *degrouper(void *arg)
{
    int bufferlen;
    int i;

    while (1) {

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
		if(sem_wait(&lock))
			printErrorAndExit("Degrouper failed to lock mutex");
		if(buffer[i] == '(') {
			int j = 1;
			while(isdigit(buffer[i + j])) {
				j++;
			}
			if(buffer[i + j] == ')') {
				strcpy(&buffer[i + j], &buffer[i + j + 1]);
				strcpy(&buffer[i], &buffer[i + 1]);
				num_ops++;
				change_made = 1;
			}
		}
		if(sem_post(&lock))
				printErrorAndExit("Degrouper failed to unlock Mutex");
	    // check for '(' followed by a naked number followed by ')'
	    // remove ')' by shifting the tail end of the expression
	    // remove '(' by shifting the beginning of the expression
	}
	func_finished[2] = '1';
	sched_yield();
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
	int no_change_counter = 0; //If there is no change seen twice 
    while (1) {

	if (timeToFinish()) {
	    return NULL;
	}
	if (no_change_counter > 2) {
		printf("No progress can be made\n");
		exit(EXIT_FAILURE);
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);
	if(sem_wait(&lock))
				printErrorAndExit("Sentinel failed to lock mutex");
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
	if(sem_post(&lock))
				printErrorAndExit("Sentinel failed to unlock mutex");
	sched_yield();

	if(sem_wait(&lock))
		printErrorAndExit("Sentinel failed to lock mutex");
	if(!strcmp(func_finished, "111") && bufferlen != 0) {
		if(!change_made)
			no_change_counter++;
		else {
			change_made = 0;
		}
		func_finished[0] = func_finished[1] = func_finished[2] = '0';
	}
	if(sem_post(&lock))
		printErrorAndExit("Sentinel failed to unlock mutex");
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
		sched_yield();// spinwaiting
	}
	if(sem_wait(&lock))
		printErrorAndExit("Reader failed to lock mutex");
	/* we can add another expression now */
	strcat(buffer, tBuffer);
	strcat(buffer, ";");
	if(sem_post(&lock))
		printErrorAndExit("Reader failed to unlock mutex");
	/* Stop when user enters '.' */
	if (tBuffer[0] == '.') {
	    return NULL;
	}
	sched_yield();
    }
}


/* Where it all begins */
int smp3_main(int argc, char **argv)
{
    void *arg = 0;		/* dummy value */
	num_ops = 0;
	func_finished[0] = func_finished[1] = func_finished[2] = '0';
    /* let's create our threads */
	if(sem_init(&lock,0,1)) {
		printErrorAndExit("Failed to initialize mutex");
	}
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
    pthread_detach(readerThread);
	pthread_join(sentinelThread,NULL);
    /* everything is finished, print out the number of operations performed */
    fprintf(stdout, "Performed a total of %d operations\n", num_ops);
	pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}

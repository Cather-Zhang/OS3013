#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

// TODO: Add more fields to this struct
typedef struct job {
    int id;
    int arrival;
    int length;
    struct job * next;
    int executed;
    int turnaround;
    int response;
}job;

/*** Globals ***/ 
int seed = 100;

//This is the start of our linked list of jobs, i.e., the job list
job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length);
/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename);
void policy_FIFO(job *head);
void analyze_FIFO(job *head);
void policy_SJF(job *head);
void analyze_SJF(job *head);
void print_job(job *head);
void bubbleSort(job*head);
void bubbleSortBack(job *head);
void swap(job*a, job*b);


int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "missing variables\n");
        fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-duration\n", argv[0]);
		exit(EXIT_FAILURE);
    }

    int analysis = atoi(argv[1]);
    char *policy = argv[2],
         *workload = argv[3];
    //int slice_duration = atoi(argv[4]);

    // Note: we use a global variable to point to 
    // the start of a linked-list of jobs, i.e., the job list 
    read_workload_file(workload);
    //print_job(head);

    if (strcmp(policy, "FIFO") == 0 ) {
        policy_FIFO(head);
        if (analysis) {
            printf("Begin analyzing FIFO:\n");
            analyze_FIFO(head);
            printf("End analyzing FIFO.\n");
        }
        exit(EXIT_SUCCESS);
    }

    else if (strcmp(policy, "SJF") == 0 ) {
        policy_SJF(head);
        if (analysis) {
            printf("Begin analyzing SJF:\n");
            analyze_SJF(head);
            printf("End analyzing SJF.\n");
        }
        exit(EXIT_SUCCESS);
    }

    // TODO: Add other policies 

	exit(EXIT_SUCCESS);
}

//////// HELPER //////////
//////////////////////////


///////// FIFO ///////////

void policy_FIFO(job *head) {
    int time = 0;
    printf("Execution trace with FIFO:\n");
    job *prev = head;
    while (prev != NULL){
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, prev->id, prev->arrival, prev->length);
        prev->response = time - prev->arrival;
        time += prev->length;
        prev->turnaround = time - prev->arrival;
        prev->executed = 1;
        prev = prev->next;
    }
    printf("End of execution with FIFO.\n");
    return;
}

void analyze_FIFO(job *head) {
    double resAvg = 0.00, turnAvg = 0.00, size = 0.00;
    job *prev = head;
    while (prev != NULL){
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
        prev->id, prev->response, prev->turnaround, prev->response);
        resAvg += prev->response;
        turnAvg += prev->turnaround;
        size++;
        prev = prev->next;
    }
    resAvg = resAvg / size;
    turnAvg = turnAvg / size;
    printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", resAvg, turnAvg, resAvg);
    return;
}


/////////// SJF ///////////
void policy_SJF(job *head) {
    int time = 0;
    printf("Execution trace with SJF:\n");

    bubbleSort(head);
    job*prev = head;

    while (prev != NULL){       
        if (time < prev->arrival) {
            time = prev->arrival;
            prev->response = 0;
        }
        prev->response = time - prev->arrival;
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, prev->id, prev->arrival, prev->length);
        time += prev->length;
        prev->turnaround = time - prev->arrival;
        prev->executed = 1;
        prev = prev->next;
    }
    printf("End of execution with SJF.\n");
    return;
}

void bubbleSort(job *head)
{
    int swapped;
    job *ptr1;
    job *lptr = NULL;
  
    /* Checking for empty list */
    if (head == NULL)
        return;
  
    do
    {
        swapped = 0;
        ptr1 = head;
  
        while (ptr1->next != lptr)
        {
            if ( (ptr1->length + ptr1->arrival) > (ptr1->next->length +ptr1->next->arrival))
            { 
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void bubbleSortBack(job *head)
{
    int swapped;
    job *ptr1;
    job *lptr = NULL;
  
    /* Checking for empty list */
    if (head == NULL)
        return;
  
    do
    {
        swapped = 0;
        ptr1 = head;
  
        while (ptr1->next != lptr)
        {
            if ( ptr1->id > ptr1->next->id)
            { 
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void swap(job*a, job*b){
    job *t = (job*)malloc(sizeof(job));
    t->arrival = a->arrival;
    t->executed = a->executed;
    t->id = a->id;
    t->length = a->length;
    t->response = a->response;
    t->turnaround = a->turnaround;

    a->arrival = b->arrival;
    a->executed = b->executed;
    a->id = b->id;
    a->length = b->length;
    a->response = b->response;
    a->turnaround = b->turnaround;

    b->arrival = t->arrival;
    b->executed = t->executed;
    b->id = t->id;
    b->length = t->length;
    b->response = t->response;
    b->turnaround = t->turnaround;
}


void analyze_SJF(job *head) {
    double resAvg = 0.00, turnAvg = 0.00, size = 0.00;
    bubbleSortBack(head);
    job *prev = head;
    while (prev != NULL){
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
        prev->id, prev->response, prev->turnaround, prev->response);
        resAvg += prev->response;
        turnAvg += prev->turnaround;
        size++;
        prev = prev->next;
    }
    resAvg = resAvg / size;
    turnAvg = turnAvg / size;
    printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", resAvg, turnAvg, resAvg);
    return;
}

/////////// RR ////////////



/*Function to append a new job to the list*/
void append(int id, int arrival, int length){
    // create a new struct and initialize it with the input data
    job *tmp = (struct job*) malloc(sizeof(struct job));

    //tmp->id = numofjobs++;
    tmp->id = id;
    tmp->length = length;
    tmp->arrival = arrival;
    tmp->executed = 0;
    tmp->response = 0;
    tmp->turnaround = 0;

    // the new job is the last job
    tmp->next = NULL;

    // Case: job is first to be added, linked list is empty 
    if (head == NULL){
        head = tmp;
        return;
    }

    job *prev = head;

    //Find end of list 
    while (prev->next != NULL){
        prev = prev->next;
    }

    //Add job to end of list 
    prev->next = tmp;
    return;
}


/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename) {
    int id = 0;
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    char *line = NULL,
        *arrival = NULL, 
        *length = NULL;

    //job **head_ptr = malloc(sizeof(struct job*));

    if( (fp = fopen(filename, "r")) == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) > 1) {
        arrival = strtok(line, ",\n");
        length = strtok(NULL, ",\n");
       
        // Make sure neither arrival nor length are null. 
        assert(arrival != NULL && length != NULL);
        
        append(id++, atoi(arrival), atoi(length));
    }

    fclose(fp);

    // Make sure we read in at least one job
    assert(id > 0);

    return;
}

void print_job(job *head) {
    job *prev = head;
    while (prev != NULL){
        printf("job [%d] arrive time: %d, execution time: %d\n", prev->id, prev->arrival, prev->length);
        prev = prev->next;
    }
}


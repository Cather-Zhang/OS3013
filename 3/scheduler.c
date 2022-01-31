#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>



// TODO: Add more fields to this struct
struct job {
    int id;
    int arrival;
    int length;
    struct job *next;
    int executed;
};

/*** Globals ***/ 
int seed = 100;

//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length);
/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename);
void policy_FIFO(struct job *head);
void analyze_FIFO(struct job *head);
void policy_SJF(struct job *head);
void analyze_SJF(struct job *head);
void print_job(struct job *head);


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

void policy_FIFO(struct job *head) {
  int time = 0;
  printf("Execution trace with FIFO:\n");
  struct job *prev = head;
  while (prev != NULL){
    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, prev->id, prev->arrival, prev->length);
    time += prev->length;
    prev->executed = 1;
    prev = prev->next;
  }
  printf("End of execution with FIFO.\n");
  return;
}

void analyze_FIFO(struct job *head) {
  // TODO: Fill this in
  return;
}


/////////// SJF ///////////
//not working yet
void policy_SJF(struct job *head) {
  int time = 0, loop = 0, arr = 0; arrMin = 0, lengthMin = 0;
  printf("Execution trace with SJF:\n");

  struct job *prev = head;

  while (prev != NULL){
    if (prev->arrival != arr) {
      arr = prev->arrival;
      loop++;
    }
    prev = prev->next;
  }

  for (int i = 0; i < loop + 1; i++){
    prev = head;
    if (prev->executed == 0){
      arrMin = prev->arrival;
    }
    while (prev != NULL){


    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, prev->id, prev->arrival, prev->length);
    time += prev->length;
    prev->executed = 1;
    prev = prev->next;
  }
  }

  while (prev != NULL){
    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, prev->id, prev->arrival, prev->length);
    time += prev->length;
    prev->executed = 1;
    prev = prev->next;
  }
  printf("End of execution with SJF.\n");
  return;
}

void analyze_SJF(struct job *head) {
  // TODO: Fill this in

  return;
}

/////////// RR ////////////



/*Function to append a new job to the list*/
void append(int id, int arrival, int length){
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job*) malloc(sizeof(struct job));

  //tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->executed = 0;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty 
  if (head == NULL){
    head = tmp;
    return;
  }

  struct job *prev = head;

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

  //struct job **head_ptr = malloc(sizeof(struct job*));

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

void print_job(struct job *head) {
  struct job *prev = head;
  while (prev != NULL){
    printf("job [%d] arrive time: %d, execution time: %d\n", prev->id, prev->arrival, prev->length);
    prev = prev->next;
  }
}


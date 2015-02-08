#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

const int MAX_KEY = 65535;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

struct      list_node_s* head = NULL;
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
int         member_total=0, insert_total=0, delete_total=0;

void        Get_input(int* inserts_in_main_p);

void*       Thread_work(void* rank);

int         Insert(int value);
void        Print(void);
int         Member(int value);
int         Delete(int value);

pthread_mutex_t mutex;
pthread_mutex_t count_mutex;

int main(){

    int inserts_in_main,success,key;
   inserts_in_main=1000;
total_ops=10000;
insert_percent=.99;
search_percent=.005;
delete_percent=.005;
    //Get_input(&inserts_in_main);

    long i;
    i = 0;
    while ( i < inserts_in_main ) {
      key = rand() % MAX_KEY;
      //printf("%d\n",key);
      success = Insert(key);
      if (success) i++;
      //Print();
    }

    pthread_t* thread_handles;

#  ifdef OUTPUT
   printf("Before starting threads, list = \n");
   Print();
   printf("\n");
#  endif

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&count_mutex, NULL);

   int start=clock();
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   int finish = clock();
   printf("Elapsed time = %f seconds\n", (double)(finish - start)/ CLOCKS_PER_SEC);
   
#  ifdef OUTPUT
   printf("After threads terminate, list = \n");
   Print();
   printf("\n");
#  endif

}

void Get_input(int* inserts_in_main_p) {

   printf("How many keys should be inserted in the main thread?\n");
   scanf("%d", inserts_in_main_p);
   printf("How many total ops should be executed?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);

   printf("Number of Threads that should be tested?\n");
   scanf("%d", &thread_count);
} 

void Print(void) {
   struct list_node_s* temp;

   printf("list = ");

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
} 

int Insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else { 
      rv = 0;
   }

   return rv;
}

void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/thread_count;

   double choice;
   int memberCount=0, insertCount=0, deleteCount=0;
   for(i=0;i<ops_per_thread;){
        choice = ((double)(rand()%10000))/10000.0;
        //printf("%f\n",choice);
        if(choice <search_percent && memberCount < ops_per_thread*search_percent){
		val = rand()%MAX_KEY;
      		pthread_mutex_lock(&mutex);
      		Member(val);
      		//printf("Member %d from %ldresult %d\n",val,my_rank,i);
      		pthread_mutex_unlock(&mutex);
                memberCount++;i++;
		//printf("Member %d\n",i);
	}
        else if(choice >= search_percent && choice <search_percent+insert_percent && insertCount < ops_per_thread*insert_percent){
		val = rand()%MAX_KEY;
      		pthread_mutex_lock(&mutex);
         	Insert(val);
         	//printf("Insert %d from %ld %d\n",val,my_rank,i);
         	pthread_mutex_unlock(&mutex);
                insertCount++;i++;
	}
        else if(choice >=search_percent+insert_percent && deleteCount < ops_per_thread*delete_percent){
		val = rand()%MAX_KEY;
      		pthread_mutex_lock(&mutex);
         	Delete(val);
         	//printf("Delete %d from %ld result %d\n",val,my_rank,i);
         	pthread_mutex_unlock(&mutex);
                deleteCount++;i++;
	}

   }

   return NULL;
}

int  Member(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d is not in the list\n", value);
#     endif
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d is in the list\n", value);
#     endif
      return 1;
   }
}  


int Delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { 
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      } else { 
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      }
   } else { /* Not in list */
      rv = 0;
   }

   return rv;
}

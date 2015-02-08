#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int MAX_KEY = 65535;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

struct      list_node_s* head = NULL;  
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;

int   member_count = 0, insert_count = 0, delete_count = 0;

/* Setup and cleanup */
void        Usage(char* prog_name);
void        Get_input(int* inserts_in_main_p);

int	Insert(int value);
void	Print(void);
int	Member(int value);
int	Delete(int value);
void	Free_list(void);
int  Is_empty(void);
char Get_command(void);
int  Get_value(void);
void Serial_work(void);

int main(){

    int inserts_in_main=1000;
total_ops=10000;
insert_percent=.99;
search_percent=.005;
delete_percent=.005;
    //Get_input(&inserts_in_main);
    
    int key,success,attempts,i;
    i = attempts = 0;

    while ( i < inserts_in_main ) {
      key = rand()%65535;
      success = Insert(key);
      attempts++;
      if (success) i++;
    }

    //Print();
    int start = clock();
    Serial_work();
    int end = clock();

    printf("Time Seconds = %f\n",(double)(end-start)/ CLOCKS_PER_SEC);

}

void Get_input(int* inserts_in_main_p) {

   printf("How many keys should be inserted in the main thread?\n");
   scanf("%d", inserts_in_main_p);
   printf("How many ops should be executed?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
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

void Serial_work(void) {

   int i, val;
   double which_op;
   unsigned seed = 1;
   
   double choice;
   int memberCount=0, insertCount=0, deleteCount=0;
   for(i=0;i<total_ops;){
        choice = ((double)(rand()%10000))/10000.0;
        //printf("%f\n",choice);
        if(choice <search_percent && memberCount < total_ops*search_percent){
		val = rand()%MAX_KEY;
      		Member(val);
		memberCount++;i++;
		//printf("Member %d\n",i);
	}
        else if(choice >= search_percent && choice <search_percent+insert_percent && insertCount < total_ops*insert_percent){
		val = rand()%MAX_KEY;
      		Insert(val);
		insertCount++;i++;
		//printf("Insert %d\n",i);
	}
        else if(choice >=search_percent+insert_percent && deleteCount < total_ops*delete_percent){
		val = rand()%MAX_KEY;
      		Delete(val);
		deleteCount++;i++;
		//printf("Delete %d\n",i);
	}

   }


   /*for (i = 0; i < total_ops*search_percent; i++) {
      val = rand()%MAX_KEY;
      Member(val);
   } 
   //printf("search = %f\n",total_ops*search_percent);
    for (i = 0; i < total_ops*insert_percent; i++) {
      val = rand()%MAX_KEY;
      Insert(val);
   } 
   //printf("search = %f\n",total_ops*insert_percent);
   for (i = 0; i < total_ops*delete_percent; i++) {
      val = rand()%MAX_KEY;
      Delete(val);
      
   }*/
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

   /* Find value */
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
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

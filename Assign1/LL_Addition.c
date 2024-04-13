#include<stdio.h>
#include<stdlib.h>

typedef struct node{
    int data;
    struct node *next;
    struct node *prev; 
}node;

typedef node* list;

void initList(list *l){
	*l = NULL;
	return;
}

int reverse(list* head){
    	node* prev = NULL;
    	node* current = *head;
    	node* next = NULL;
    	int len = 0;
    	
    	while (current != NULL) {
        	len++;
        	next = current->next;
        	current->next = prev;
        	prev = current;
        	current = next;
    	}
    	*head = prev;
   	 return len;
}


void push(list *l , int data){
	node* newnode = (node*)malloc(sizeof(node));
	newnode -> data = data;
	newnode -> next = NULL;
	
	
	if((*l) == NULL){
		*l = newnode;
		newnode -> next = NULL;
	}
	
	else{
		newnode -> next = (*l);
		(*l) = newnode;
	}
	return ;
}

void append(list *l , int data){
	node* newnode = (node*)malloc(sizeof(node));
	newnode -> data = data;
	//newnode -> sign = sign;
	newnode -> next = NULL;
	
	if( (*l) == NULL){
		*l = newnode;
		return;
	}
	
	else{
	     node* temp = *l;
	     
	     while(temp -> next != NULL){
	     		temp = temp -> next;
	     }
	     temp -> next = newnode;
	     newnode -> next = NULL;
	}
	return;
}

void assignTheValueToNullList(list l1 ,  list *l2){
	
    //freeList(&l2);
	node* ptr1 = l1;
	
	while(ptr1 != NULL){
		append(&(*l2) , ptr1->data);
		ptr1 = ptr1 -> next;
	}
	
	return;
}

list addList(list *l1 , list *l2){
	
	list l3;
	initList(&l3);
	
	
	if(!(*l1) && !(*l2)){
		printf("List doesnt have any value");
		exit(0);
	}
	
	if(!(*l1)){
		assignTheValueToNullList(*l2, &l3);
		return l3;
	}
	
	if(!(*l2)){
		assignTheValueToNullList(*l1,&l3);
		return l3;
	}
	
	//display(*l2);
	reverse(l1);	
	reverse(l2);
	
	node* ptr1 = *l1;
	node* ptr2 = *l2;
	
	int sum ;
	int carry = 0;
		
	while(ptr1 != NULL || ptr2 != NULL || carry){
		sum = 0;
		if(ptr1 != NULL){
		   sum += ptr1 -> data;
		}
		
		if(ptr2 != NULL){
		   sum += ptr2 -> data;
		}
		
		sum += carry;
		
		carry = sum / 10;
		sum = sum % 10;
		
		append(&l3 ,sum);
		
		if(ptr1 != NULL){
		   ptr1 = ptr1 -> next;
		 }
		 
		if(ptr2 != NULL){    
		   ptr2 = ptr2 -> next;
		}
	}
	
	if(carry != 0){
		push(&l3 , carry);
	}
	
	
	reverse(l1);
	reverse(l2);
	
	reverse(&l3);
	
	
	return l3;
}

void freeList(list* l) {
    node* temp;
    while (*l) {
        temp = *l;
        *l = (*l)->next;
        free(temp);
    }
}

void display(list l){
	if(l == NULL){
		//printf("List is NULL");;
	}
	
	node* disp = l;
	
	while(disp != NULL){
		printf("%d",disp -> data);
		disp = disp -> next;
	}
	printf("\n");
	return;
}



int main() {
    list l1, l2, l3;
    initList(&l1);
    initList(&l2);
    initList(&l3);

    int n1, n2;
    int num;

    printf("Enter the number of digits for the first number: ");
    scanf("%d", &n1);

    printf("Enter the digits for the first number (in reverse order):\n");
    for (int i = 0; i < n1; i++) {
        scanf("%d", &num);
        append(&l1, num);
    }

    printf("Enter the number of digits for the second number: ");
    scanf("%d", &n2);

    printf("Enter the digits for the second number (in reverse order):\n");
    for (int i = 0; i < n2; i++) {
        scanf("%d", &num);
        append(&l2, num);
    }

    printf("First number: ");
    reverse(&l1);
    //display(l1);

    printf("Second number: ");
    reverse(&l2);
    //display(l2);

    // Perform addition
    l3 = addList(&l1, &l2);

    printf("Sum: ");
    display(l3);

    // Free memory
    freeList(&l1);
    freeList(&l2);
    freeList(&l3);

    return 0;
}


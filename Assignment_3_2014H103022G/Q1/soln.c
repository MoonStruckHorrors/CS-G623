//NS-37

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "helper.c"

void *check_pth(void*);
void print_paths(node*, int);

int ni, na, inp[20], arr[20][20];
int one = 1;
int zero = 0;

int main(int argc, char* argv[]) {
	int i, j;

	//Read File
	FILE *fp = fopen(argv[1], "r");

	fscanf(fp, "%d", &ni);

	for(i=0; i<ni; i++) {
		fscanf(fp, "%d", &inp[i]);
	}

	fscanf(fp, "%d", &na);

	for(i=0; i<na; i++) {
		for(j=0; j<na; j++) {
			fscanf(fp, "%d", &arr[i][j]);
		}
	}

	fclose(fp);

	//Action begins

	if(inp[0] != arr[0][0]) {
		printf("No path exists\n");
		return 0;
	}

	node* root = malloc(sizeof(node)); init_node(root, 0, 0); //(0, 0). Root node of the tree

	pthread_t tid1, tid2, tid3; pthread_attr_t attr; pthread_attr_init(&attr);

	packet* p1 = malloc(sizeof(packet)); init_packet(p1, root, 1, 1, 0);
	pthread_create(&tid1, &attr, check_pth, (void*)p1);

	packet* p2 = malloc(sizeof(packet)); init_packet(p2, root, 1, 0, 1);
	pthread_create(&tid2, &attr, check_pth, (void*)p2);
	
	packet* p3 = malloc(sizeof(packet)); init_packet(p3, root, 1, 1, 1);
	pthread_create(&tid3, &attr, check_pth, (void*)p3);

	int *op1 = &zero, *op2 = &zero, *op3 = &zero;

	pthread_join(tid1, (void**)&op1);
	pthread_join(tid2, (void**)&op2);
	pthread_join(tid3, (void**)&op3);

	//printf("%d, %d, %d\n", *op1, *op2, *op3);

	if(*op1 == 1 || *op2 == 1 || *op3 == 1)
		print_paths(root, 0);
	else
		printf("No path exists\n");

	return 0;
}

void *check_pth(void* arg) { //Function to be used by thread
	packet* p = (packet*) arg; //Unpack the arg

	if(inp[p->in_pos] != arr[p->i][p->j]) { //Current element of Input array doesn't match the input matrix element
		pthread_exit(&zero);
	} else { //Matches
		node* temp = malloc(sizeof(node));
		init_node(temp, p->i, p->j);

		ins_child(p->data, temp); //Insert child, Multiple child nodes supported

		/* More Threads */
		pthread_t tid1, tid2, tid3; pthread_attr_t attr; pthread_attr_init(&attr);

		int t1c = 0, t2c = 0, t3c = 0; //if the thread is created, set t'i'c = 1

		if(p->i + 1 < na) { //Below
			packet* p1 = malloc(sizeof(packet)); init_packet(p1, temp, p->in_pos + 1, p->i + 1, p->j);
			pthread_create(&tid1, &attr, check_pth, (void*)p1);
			t1c = 1;
		}

		if(p->j + 1 < na) { //Right
			packet* p2 = malloc(sizeof(packet)); init_packet(p2, temp, p->in_pos + 1, p->i, p->j + 1);
			pthread_create(&tid2, &attr, check_pth, (void*)p2);
			t2c = 1;
		}
	
		if(p->i + 1 < na && p->j + 1 < na) { //Diagonal
			packet* p3 = malloc(sizeof(packet)); init_packet(p3, temp, p->in_pos + 1, p->i + 1, p->j + 1);
			pthread_create(&tid3, &attr, check_pth, (void*)p3);
			t3c = 1;
		}

		int *op1, *op2, *op3; int opi1 = 0, opi2 = 0, opi3 = 0;

		//Join only if created
		if(t1c) { pthread_join(tid1, (void**)&op1); opi1 = *op1; }
		if(t2c) { pthread_join(tid2, (void**)&op2); opi2 = *op2; }
		if(t3c) { pthread_join(tid3, (void**)&op3); opi3 = *op3; }

		if(p->i + 1 == na && p->j + 1 == na) { //Reached the end, return 1
			pthread_exit(&one);
		}

		//printf("%d, %d, %d\n", opi1, opi2, opi3);

		if(opi1 == 1 || opi2 == 1 || opi3 == 1) //Atleast one path found
			pthread_exit(&one);
		else
			pthread_exit(&zero); //Nope
	}
}

void print_paths(node* n, int depth) { //Traverse the tree & print path(s)
	int i;

	for(i=0; i< n->n_count; i++) //Call print_paths with every child recursively
		print_paths(n->next[i], depth + 1);

	if(depth + 1 == ni && n->x == na - 1 && n->y == na - 1) { //Destination node at ni - 1 depth, trace back to the source (0, 0)
		node* curr = n;
		while(curr->prev != NULL) {
			printf("(%d, %d) -> ", curr->x, curr->y); //Output node
			curr = curr->prev;
		}

		printf("(%d, %d)\n", curr->x, curr->y); //Parent & \n
	}
}
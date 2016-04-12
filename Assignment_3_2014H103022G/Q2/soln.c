//NS-23

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "helper.c"

void* comp_mul(void*);

complex in[40]; //Using this for Input AND Output AND Temp.
pthread_t tid[20]; //Thread id

int main(int argc, char* argv[]) {
	
	FILE *fp = fopen(argv[1], "r");
	int n, i;

	fscanf(fp, "%d", &n);

	for(i = 0; i<n; i++) {
		fscanf(fp, "%d", &in[i].a);
		fscanf(fp, "%d", &in[i].b);

		//print_complex(&in[i]);
	}

	fclose(fp);

	while(n != 1) {
		int is_odd = 0;
		if(n % 2 != 0) is_odd = 1;

		n = n/2; //ceil[n/2] is n/2

		for(i=0; i<n; i++) { //n threads
			pthread_attr_t attr; pthread_attr_init(&attr);

			packet* p = malloc(sizeof(packet));
			init_packet(p, &in[2*i], &in[2*i + 1], i); //Prepare packet, multiply in[2*i] & in[2*i+t] & put it in in[i]

			pthread_create(&tid[i], &attr, comp_mul, (void*) p);
		}

		for(i=0; i<n; i++) {
			pthread_join(tid[i], NULL);
		}

		if(is_odd) { //quick fix for floor[n/2]
			in[n] = in[2*n]; //direct transfer for next iteration
			n++; //include the newly added last element
		}

	}

	print_complex(&in[0]);

	return 0;
}

void* comp_mul(void* arg) {
	packet* p = (packet*) arg;

	complex temp;

	mult_complex(p->a, p->b, &temp); //Use temp, don't play with fire.

	in[p->n] = temp;

	pthread_exit(0);
}
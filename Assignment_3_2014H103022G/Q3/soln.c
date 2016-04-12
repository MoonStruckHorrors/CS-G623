//NS-23

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "helper.c"

void *mul_nbr(void*);
int num_dig(int);
int get_dig(int, int);

int dn1; //Number of digits
int res[20]; //Stores final result, 0 is rightmost
pthread_t tid[20];

int main(int argc, char* argv[]) {
	int n1, n2, i, j, is_neg = 0;
	n1 = atoi(argv[1]);
	n2 = atoi(argv[2]);

	if(n1 < 0 && n2 < 0) { //Handle --
		n1 *= -1; n2 *= -1;
	}

	if(n1 * n2 < 0) { //Handle -+ || +-
		is_neg = 1;
		if (n1 < 0) n1 *= -1;
		if (n2 < 0) n2 *= -1;
	}

	//printf("%d, %d\n", n1, num_dig(n1));
	dn1 = num_dig(n1); int dn2 = num_dig(n2); //Digits in each number

	if(dn1 != dn2) {
		printf("Please input 2 N digit numbers. \n");
		return 1;
	}

	//printf("%d\n", (int)(log10((double)n1) + log10((double)n2)) + 1); //Prediction of Number of Digits in Result

	int nt = (dn1 * 2) - 1; // Number of threads, equal to number of columns in the results. NOTE: Not digits

	for(i = 0; i<nt; i++) {
		pthread_attr_t attr; pthread_attr_init(&attr);
		packet* p = malloc(sizeof(packet)); init_packet(p, n1, n2, i);

		pthread_create(&tid[i], &attr, mul_nbr, (void*)p);
	}

	//Join all the threads
	for(i = 0; i<nt; i++) {
		pthread_join(tid[i], NULL);
	}

	int digs = nt; //Final digit count

	//Perform carry forward operations, if needed
	for(i = 0; i<nt; i++) {
		if(num_dig(res[i]) > 1) {
			int temp = res[i];
			res[i] = res[i] % 10;
			if(i+1 == nt) { //last digit, initialize a new arr element
				res[i+1] = 0;
				digs++; //Final digit count++
			}

			res[i+1] += temp / 10;
		}
	}

	//Print final result

	if(is_neg) printf("-");

	for(i = digs - 1; i>=0; i--) {
		printf("%d", res[i]);
	}
	printf("\n");

	return 0;
}

void *mul_nbr(void* arg) {
	packet* p = (packet*)arg;
	int i;
	int sum_of_mul = 0;
	for(i=0; i<dn1; i++) {
		
		int offset = p->dig - i; //Determines which element from n1 is to be multiplied with ith element from n2

		if(offset >= 0 && offset < dn1) {

			int d2 = get_dig(p->num2, i);
			int d1 = get_dig(p->num1, offset);

			sum_of_mul += d1 * d2;
		}
	}

	res[p->dig] = sum_of_mul;

	pthread_exit(0);
}

int num_dig(int num) {
	int d_count = 0;
	while(num != 0) {
		num /= 10;
		d_count++;
	}
	return d_count;
}

int get_dig(int num, int dig) { //0 is rightmost
	while(dig-- != 0) {
		num = num/10;
		if(num == 0) {
			return -1;
		}
	}
	return num % 10;
}
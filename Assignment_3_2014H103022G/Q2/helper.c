typedef struct s_complex {
	int a;
	int b;
} complex;

void init_complex(complex* c, int a, int b) {
	c->a = a;
	c->b = b;
}

void mult_complex(complex* a, complex *b, complex *c) {
	c->a = (a->a * b->a) - (a->b * b->b);
	c->b = (a->a * b->b) + (a->b * b->a);
}

void print_complex(complex* c) {
	if(c->b >= 0) {
		printf("%d + %di\n", c->a, c->b);
	} else {
		printf("%d %di\n", c->a, c->b);
	}
}

typedef struct s_packet {
	complex* a;
	complex* b;
	int n; //position in output array
} packet;

void init_packet(packet* p, complex* a, complex* b, int n) {
	p->a = a;
	p->b = b;
	p->n = n;
}
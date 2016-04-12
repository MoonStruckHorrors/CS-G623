typedef struct s_node { //struct for node with multiple child nodes
	int x, y, n_count;
	struct s_node *next[10]; //Children
	struct s_node *prev; //Parent
} node;

int init_node(node* n, int x, int y) { //Constructor for node
	n->x = x;
	n->y = y;
	n->n_count = 0;
	n->prev = NULL;
}

void ins_child(node* n, node* c) { //Increment the n_count & add node to *next[] array
	n->next[n->n_count] = c;
	n->n_count++;
	c->prev = n;
}

typedef struct s_packet { //struct for packet to be sent as the arg to thread function
	node* data;
	int in_pos; //Current position in the input array
	int i, j; //Current position in the input matrix
} packet;

void init_packet(packet* p, node* n, int in_pos, int i, int j) { //Constructor for packet
	p->in_pos = in_pos;
	p->data = n;
	p->i = i; p->j = j;
}
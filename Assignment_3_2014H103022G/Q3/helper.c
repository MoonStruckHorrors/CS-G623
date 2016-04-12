typedef struct s_packet {
	int num1, num2, dig; //Dig = This packet is intended for calculation of which digit, 0 is rightmost
} packet;

void init_packet(packet* p, int num1, int num2, int dig) { //Constructor for packet
	p->num1 = num1;
	p->num2 = num2;
	p->dig = dig;
}
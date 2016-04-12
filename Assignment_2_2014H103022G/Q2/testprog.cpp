#include <iostream>

int main(int argc, char* argv[]) {
	std::cout << "OP: ";
	
	for(int i=1; i<argc; i++)
		std::cout << argv[i] << " ";
		
	if(argc < 2) std::cout << "No Args";
	
	std::cout << "\n";
	
	return 0;
}

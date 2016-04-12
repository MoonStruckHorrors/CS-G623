#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

pid_t *rpid;

class node {
public: 
	pid_t pid;
	node* parent;
	vector<node*> children;

	//Constructor
	node(pid_t pid, node* parent) {
		this->pid = pid;
		this->parent = parent;
	}
	void add_child(node* n) {
		children.push_back(n);
	}
	node* get_child(int index) {
		if(index < children.size())
			return children[index];
	}
	int child_count() {
		return children.size();
	}
	void set_parent(node* n) {
		this->parent = n;
		n->add_child(this);
	}
};

class ctree {
public:
	node* root;
	vector< vector <node*> > v2; //Vector of level vectors, 0 to N
	int max_ch, max_lvl, tot_nodes; //total nodes
	vector<int> n_lvl; //nodes at the ith level
	ofstream myfile;
	ctree(int max_lvl, int max_ch) {
		this->max_lvl = max_lvl;
		this->max_ch = max_ch;
		this->root = NULL;
		myfile.open("temp.txt");
		calc_nodes();
	}
	
	//Calculate total nodes & nodes at each level
	void calc_nodes() {
		n_lvl.push_back(1); n_lvl.push_back(1);	tot_nodes = 2;
		for(int i=2; i<=max_lvl; i++) {
			n_lvl.push_back(n_lvl[i-1] + n_lvl[i-2]);
			tot_nodes += n_lvl.at(i);
		}
	}

	//Generate the placeholder values.
	void crt_tree() {
		node* n = root;
		vector<node*> temp; temp.push_back(root); //temp vector for holding root
		v2.push_back(temp); //pushing root at level 0
		int curr_lvl = 1, curr_t = 1;
		
		//Creating levels
		while(curr_t < tot_nodes) {
			vector<node*> v;
			for(int i=0; i<n_lvl[curr_lvl]; i++) {
				v.push_back(new node(curr_t, NULL));
				curr_t++;
			}
			curr_lvl++;
			v2.push_back(v);
		}
		
		//Linking (i)th level with (i-1)th
		for(int i=1; i<=max_lvl; i++) {
			vector<node*> pv = v2.at(i-1);
			vector<node*> v = v2.at(i);
			int pv_i = 0; //Index of parent in prev vector, changes after +=max_ch
			for(int j=0; j<v.size(); j+=max_ch) {
				for(int k=j; k<j+max_ch; k++) {
					if(k<v.size()) {
						v[k]->set_parent(pv[pv_i]);
					}
				}
				pv_i++;
			}
		}
	}
	
	
	void fork_rec(node* n) {
		if(n!=root) {
			kill(getpid(), SIGSTOP); //Wait from SIGCONT from parent
		}
		pid_t ppid = getpid(); //Store ppid to send SIGCONT to children after for loop
		for(int i=0; i< n->child_count(); i++) {
			n->get_child(i)->pid = fork();
			if(!n->get_child(i)->pid) {
				/* Print Processes with pid & ppid as they are created */
				cout << "Created " << getpid() << " son of " << getppid() << endl << flush;
				
				/*Store Processes with pid & ppid in temp.txt to traverse later*/
				myfile << getpid() << "|" << getppid() << endl << flush;
				
				fork_rec(n->get_child(i));
			} else { //TODO: Remove this when complete
			}
		}
		
		usleep(360000); //wait for the processes to be actually created. Without this, SIGCONT gets sent before local SIGSTOP.
		
		if(ppid == getpid()) { //Send SIGCONT to all the child AFTER they have been forked
			for(int i=0; i< n->child_count(); i++) {
				kill(n->get_child(i)->pid, SIGCONT);
				usleep(180000); //Let the first child execute first
			}
		}
		

		for(int i=0; i<n->child_count(); i++) { //Printing the exit status
			int status;
			waitpid(n->get_child(i)->pid, &status, 0);
			cout << "Exit status for " << n->get_child(i)->pid << " is: " << status / 256 << endl;
		}
		
		if(*rpid != getpid()) //don't let root die
			exit(0);
		else
			myfile.close(); //in the root process, close the file
	}
	
	
	void print_tree() { //Print the whole tree
		vector<node*> temp = v2.at(0);
		cout << temp[0]->pid << "(root)" << endl;
		for(int i=1; i<v2.size(); i++) {
			vector<node*> temp = v2.at(i);
			for(int j=0; j<temp.size(); j++) {
				cout << temp[j]->pid << "(son of " << temp[j]->parent->pid << ") ";
			}
			cout << endl;
		}
	}
	
	int exists_v(vector<pid_t> v, pid_t val) { //Returns 1 if val exists in v
		for(int i=0; i<v.size(); i++) {
			if(v[i] == val)
				return 1;
		}
		return 0;
	}


	void read_from_file() {
		fstream f;
		f.open("temp.txt", ios::in);
		vector<pid_t> v;
		v.push_back(root->pid);
		if(f.is_open()) {
			while(f.good()) {
				string temp;
				getline(f, temp);
				string ch = temp.substr(0, temp.find('|'));
				int pidp; stringstream ss(ch); ss>>pidp;
				if(!exists_v(v, pidp)) {
					v.push_back(pidp);
				}
			}
		} f.close();
		

		int c=0;
		for(int i=0; i<v2.size(); i++) {
			vector<node*> temp = v2[i];
			for(int j=0; j<temp.size(); j++) {
				temp[j]->pid = v[c++];
			}
		}
	}
	
	void post_order_rec(node* n) {
		if(n->child_count() == 0) {
			cout << n->pid << " "; //Leaf Node
		} else {
			for(int i=0; i<=n->child_count() - 1; i++) { //L2R Children
				post_order_rec(n->get_child(i));
			}
			cout << n->pid << " "; //Then the parent
		}
	}
};

int main(int argc, char* argv[]) {
	if(argc < 3) {
		cout << "Provide atleast 2 arguments. Usage: ./prog <max_level> <max_child>" << endl;
		return 1;
	}
	
	//Shared variable to store the root's pid
	rpid = (pid_t*)mmap(NULL, sizeof *rpid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	int max_lvl = atoi(argv[1]);
	int max_child = atoi(argv[2]);
	
	cout << "Generating tree with " << max_lvl << " levels and " << max_child << " children per node." << endl;

	//Create the tree used as the path for forks
	ctree t(max_lvl, max_child);
	
	*rpid = getpid(); //Root creation
	t.root = new node(getpid(), NULL);
	t.crt_tree();	//Populate tree with placeholder values; 0..N
	t.fork_rec(t.root);
	
	while(wait(NULL) != -1); // Just in case
	
	t.read_from_file();
	
	cout << "\n=====Final Tree======\n";
	t.print_tree();
	
	cout << "\n=====Post Order======\n";
	t.post_order_rec(t.root);
	cout << "\n\n";
	return 0;
}

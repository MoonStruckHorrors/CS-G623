#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <cstdlib>
#include <sys/wait.h>
#include <vector>

using namespace std;

const int DIFF = 3;

void read_file(string fname, string& ftext) { //file(fname) --> &ftext; setpath checking
	fstream aFile;
	aFile.open(fname.c_str(), ios::in);
	
	int sp_checked = 0;
	int is_sp = 0;
	string spath;
	
	if(aFile.is_open()) {
		while(aFile.good()) {
			string temp;
			getline(aFile, temp);
			if(temp.length() > 0) {
				if(!sp_checked) { //setpath checking
					sp_checked = 1;
					if(temp.find("setpath") == 0) {
						is_sp = 1;
						spath = temp.substr(temp.find(" ") + 1, temp.length() - temp.find(" "));
						continue; //setpath line; path taken, ignore now.
					}
				}
				if(is_sp) ftext += spath; //add the path initially
				ftext += temp;
				ftext += "\n";
			}
		}
	} else {
		cout << "Error: " << fname << " does not exist" << endl;
	}
	
	aFile.close();
}

vector<string> split_args(string scmd) { //Returns a vector with command at [0] & args at [1+]
	vector<string> sv;
	while(1) {
		if(scmd.find(" ") == -1) { //more args
			sv.push_back(scmd.substr(0, scmd.length()));
			break;
		} else { //last arg
			sv.push_back(scmd.substr(0, scmd.find(" ")));
			scmd = scmd.substr(scmd.find(" ") + 1, scmd.length() - scmd.find(" "));
		}
	}
	return sv;
}

int fork_and_execv(string scmd) { //grandchild for individual commands
	vector<string> sv = split_args(scmd);
	char* args[sv.size()];
	int i;
	for(i=1; i<sv.size(); i++) { //args from vector[1]..[size] -> args array.
		args[i] = const_cast<char*>(sv[i].c_str()); //string -> const char* -> char*
	} args[i] = NULL;
	
	if(!fork()) {
		execv(sv[0].c_str(), args);
		cout << "Error: execv failed" << endl;
		exit(0); //done with error, kill process
	}
	return 0;
}

void init_fork(pid_t &pid, string ftext) { //Top Level Processes
	pid = fork();
	if(!pid) {
		kill(getpid(), SIGSTOP); //Initially Waiting for N processes to be created & for execution to start.
		while(1) {
			if(ftext.length() > 0) {
				string scmd = ftext.substr(0, ftext.find("\n")); //getline
				ftext = ftext.substr(ftext.find("\n") + 1, ftext.length() - ftext.find("\n")); //(X) Line
				if(scmd.length() > 0) {
					fork_and_execv(scmd);
					usleep(1000);
					wait(NULL);
				}
			} else {
				break; //Done with the file
			}
			kill(getpid(), SIGSTOP); //wait for further orders from the parent.
		}
		exit(0); //Exit when the the loop breaks.
	} else if(pid < 0) {
		cout << "Failed to fork(), Terminating" << endl;
		exit(1);
	}
}

int main(int argc, char* argv[]) {
	int num_files;
	vector<pid_t> proc_v; //Vector for storing children's pids.
	
	if(argc < 2) {
		cout << "Atleast one argument(filename) should be provided.\n";
		return 1;
	}
	
	num_files = argc-1;
	
	string fnames[num_files]; //Array for storing the list of file names.
	
	string ftext[num_files]; //Array for storing the text of files.
	
	for(int i=1; i<argc; i++) {
		fnames[i-1] = argv[i];
	}
	
	for(int i=0; i<argc-1; i++) { //Read files, store text in ftext array.
		read_file(fnames[i], ftext[i]);
	}
	
	for(int i=0; i<num_files; i++) { //Fork() n processes, get pids in proc_v
		proc_v.push_back(0); //Initialize element 'i'
		init_fork(proc_v[i], ftext[i]);
	}
	
	for(int i=0; i<num_files; i++) { //1, 2, 3 ...
		int all_done = 0;
		while(!all_done) {
			int any_left = 0;
			for(int j=i; j<num_files; j+=DIFF) { //1, 4, 7 or 2, 5, 8
				kill(proc_v[j], SIGCONT); //resume the 'i'th child
				
				usleep(5000); //make sure the 'i'th child has enough time to execute.
				
				waitpid(proc_v[j], NULL, WNOHANG); //kill the zombie if it's dead, WNOHANG for not halting the parent if not.
				
				if(kill(proc_v[j], 0) == 0) { //if dead, false & any_left = 0; else any_left = 1
					any_left = 1;
				}
			}
			if(!any_left) all_done = 1; //A batch of three finished execution
		}
	}	
	return 0;
}

#include <iostream>
#include <string>
#include <stdio.h>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;


void openCmd(string bc){
	string stream = "opt -print-callgraph ";
	stream.append(bc);
	stream.append(" 2>callgraph.txt 1>/dev/null");
	FILE *in = popen(stream.c_str() , "r");
}

void ReadinAndParse( string cg, string line, string callee,
		 map<string, vector<string> >& graph, string caller,
		map<string, int>& functionsupport) {
	//if((unsigned)strlen(argv) == 1){
	//}
	//if((unsigned)strlen(argv) == 2){
	//}
	ifstream infile(cg.c_str());
	// Reads in the line  from input
	while (getline(infile, line)) {
		//cout << line << "\n";
		//cout << "hello";
		//cout << line.find("Call graph node for function: ") << "\n";
		if (line.find("Call graph node for function: ") == 0) {
			callee = line.substr(line.find("'") + 1, line.length());
			callee = callee.substr(0, callee.find("'"));
			//  cout << callee << "\n";
			graph[callee];
		}
		if ((line.find("calls function") == 16) && callee != "") {
			caller = line.substr(line.find("'") + 1, line.length());
			caller = caller.substr(0, caller.find("'"));
			if (find(graph[callee].begin(), graph[callee].end(), caller)
					== graph[callee].end()) {
				graph[callee].push_back(caller); // Element not in vector.
				sort(graph[callee].begin(), graph[callee].end()); // sort vector
			}
			if (functionsupport.find(callee) == functionsupport.end()) {
				functionsupport[caller] = 1;
			} else {
				functionsupport[caller] = functionsupport[caller] + 1;
			}
			//	cout << caller << "\n";
		}
	}

	infile.close();
	}

map<string, string> GetSupport(map<string, vector<string> > graph,
		map<string, int>& functionsupport) {
	// Get pair support for function
	map<string, string> fn;
	string s = "";
	map<string, vector<string> >::iterator it;
	vector<string>::iterator it2;
	for (it = graph.begin(); it != graph.end(); it++) {
		// iterator->first = key
		// iterator->second = value
		//	cout << it->first << endl;
		//for(it2 = (it->second).begin(); it2 != (it->second).end(); it2++){
		//cout << it2;
		//}
		fn[it->first] = "";
		for (int index = 0; index < (it->second).size(); index++) {
			for (int j = index + 1; j < (it->second).size(); j++) {
				s = it->second[index] + ", " + it->second[j];
				fn[it->first] = fn[it->first] + s;
				//cout << s;
				if (functionsupport.find(s) == functionsupport.end()) {
					functionsupport[s] = 1;
				} else {
					functionsupport[s] = functionsupport[s] + 1;
				}
				//cout << it->second[index] << " ";
				//cout << it->second[j] << endl;
			}
		}
	}
	return fn;
}

void findbug(map<string, int> functionsupport, double confidence, int support,
		map<string, string> fn) {
	map<string, int>::iterator iter;
	map<string, int>::iterator iter2;
	map<string, string>::iterator it5;
	float conf;
	for (iter = functionsupport.begin(); iter != functionsupport.end();
			iter++) {
		//cout << iter->first  << endl;
		for (iter2 = functionsupport.begin(); iter2 != functionsupport.end();
				iter2++) {
			if (iter->first == iter2->first) {
				continue;
			} else if ((iter->first).find(iter2->first) != std::string::npos) {
				//	cout << iter->first << " + ";
				//	cout << iter2->first << " ";
				//	cout << endl;
				conf = float(iter->second) / float(iter2->second);
				if (conf >= confidence && iter->second >= support) {
					for (it5 = fn.begin(); it5 != fn.end(); it5++) {
						if (it5->second.find(iter2->first) != string::npos) {
							if (it5->second.find(iter->first) != string::npos) {
							} else {
								
								//			cout << "look for a bug" << endl;
					/*			cout << "bug: " << iter2->first << " " << "in "
										<< it5->first << ", pair: (" << iter->first
										<< "), support: " << iter->second
										<< ", confidence: " << conf * 100 << "%"
										<< endl;*/
								//			cout << it5->second << " + " << iter2->first << " + " << iter->first << endl;
								
					printf("bug: %s in %s, pair: (%s), support: %d, confidence: %.2f%%\n",iter2->first.c_str(),it5->first.c_str(),iter->first.c_str(),iter->second,conf * 100);
							}
						}
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {

  string line = "";
  map< string, vector<string> > graph;
  string caller = "";
  string callee = "";
  map< string, int> functionsupport;
  int support = 3;
  double confidence = .65;
  //string cg = "callgraph1.txt";
  /*
  if (argc == 4){
	  confidence = atof(argv[3]) / 100;

  }
  else if (argc == 3){
	  support = atoi(argv[2]);
  }
  else if (argc > 4 || argc < 2){
	  return -1;
  }
*/


	switch (argc) {
	case 4:
		confidence = atof(argv[3])/100.00;
	case 3:
		support = atoi(argv[2]);
	case 2:
		break;
	default:
		printf("Input must be %s <bitcode_file> <T_SUPPORT> <T_CONFIDENCE>\n",
				argv[0]);
		return 0;
	}

  openCmd(string(argv[1]));
  string cg = "callgraph.txt"; //string(argv[1]);

	ReadinAndParse(cg, line, callee, graph, caller, functionsupport);
	// Get pair support for function
	map<string, string> fn = GetSupport(graph, functionsupport);

	
	findbug(functionsupport, confidence, support, fn);
	//cout << functionsupport["A B"] << endl;
	
	
 }

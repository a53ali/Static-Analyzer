#include <iostream>
#include <string>
#include <stdio.h>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;


void openCmd(string bc){
	string stream = "opt -print-callgraph ";
	stream.append(bc);
	stream.append(" 2>callgraph.txt 1>/dev/null");
	FILE *in = popen(stream.c_str() , "r");
}

int main(int argc, char* argv[]) {

  string line = "";
  map< string, int > Encode;
  map<  int, string > decode;
  string caller = "";
  string callee = "";
  map< string, int> functionsupport;
  int support = 3;
  double confidence = .65;
  map< int, vector<int> > graph;
  //string cg = "callgraph1.txt";
  
  if (argc == 4){
	  confidence = atof(argv[3]) / 100.00;
  }
  else if (argc == 3){
	  support = atoi(argv[2]);
  }
  else if (argc > 4 || argc < 2){
	  return -1;
  }
  
  openCmd(string(argv[1]));
  string cg = "callgraph.txt"; //string(argv[1]);
  // parse graph
  int i = 0;
  ifstream infile(cg.c_str());
  // Reads in the line  from input
  while (getline(infile, line)) {
	  //cout << line << "\n";
	  //cout << "hello";
	  //cout << line.find("Call graph node for function: ") << "\n";
	  if (line.find("Call graph node for function:")!= string::npos) {
		 // cout << caller << endl;
		  caller = line.substr(line.find("'") + 1, line.length());
		  //cout << caller << endl;
		  caller = caller.substr(0, caller.find("'")  );
		 // cout << caller << endl;
		 // cout << caller << "hello\n";
		  Encode[caller] = i; 
		  decode[i] = caller;
		  i++;
	  }
	  
	  if ((line.find("calls function") == 16)) {
		  callee = line.substr(line.find("'") + 1, line.length());
		  callee = callee.substr(0, callee.find("'"));
		  if (Encode.find(callee) == Encode.end()) {
			  Encode[callee] = i;
			  decode[i] = callee;
			  i++;
			  graph[Encode[callee]].push_back(Encode[caller]);
			  sort(graph[Encode[callee]].begin(), graph[Encode[callee]].end());
		  }
		  else{
			  //it has the callee
			  //check to see if it is already 
			  if (find(graph[Encode[callee]].begin(), graph[Encode[callee]].end(), Encode[caller]) == graph[Encode[callee]].end()){
				  graph[Encode[callee]].push_back(Encode[caller]);
				  sort(graph[Encode[callee]].begin(), graph[Encode[callee]].end());
			  }
		  }
		  
		  
		  //If graph contains the function do not add
		  // if it doess add
		  //cout << "i was here " << endl;
		 // if (graph.find(Encode[callee]) == graph.end()){
			  //not found
			  
			 // graph[Encode[callee]].push_back(Encode[caller]);
		 // }
		 // else{
			  //found
			  //check to see if it is already 
		//	  if (find(graph[Encode[callee]].begin(), graph[Encode[callee]].end(), Encode[callee]) == graph[Encode[callee]].end()){
				  // it is not there
		//		  graph[Encode[callee]].push_back(Encode[caller]);
		//	  }
		//  }
	  }
  }
infile.close();
  

  map <int, int> singlesupport;
  vector<int> v1,v2,v3;
  float conf;
  int support2;
  int z = 0;

  /*
  for (map< string, int >::iterator iter = Encode.begin(); iter != Encode.end(); iter++){
	  for (map< string, int >::iterator iter2 = Encode.begin(); iter2 != Encode.end(); iter2++){
		  if (graph[iter->second] != graph[iter2->second]){
			  v1 = graph[iter->second];
			  v2 = graph[iter2->second];
			  set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
			  conf = float(v3.size()) / float(v1.size());
			  support2 = v3.size();
			  if (conf < 1 && conf >= confidence && v1.size() >= support){
				  v3.clear();
				  set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
				  for (vector<int>::iterator it = v3.begin(); it != v3.end(); it++){
					  cout << "bug: " << iter->first << " in " << decode[*it] << ", pair : (" << iter->first << ", " << iter2->first << "), support : " << support2 << ", confidence : " << conf << endl;
				  }
				  v3.clear();
			  }
		  }
	  }
  }
  */
 
  for (map< int, vector< int > >::iterator iter = graph.begin(); iter != graph.end(); iter++){
	  v1 = iter->second;
	  sort(v1.begin(), v1.end());
	  for (map< int, vector< int > >::iterator iter2 = graph.begin(); iter2 != graph.end(); iter2++){
		  if (iter->first == iter2->first){
			  continue;
		  }
		  else{
			 
			  v2 = iter2->second;
			  sort(v2.begin(), v2.end());
			  set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
			  conf  = float(v3.size()) / float(v1.size());
			  if (conf < 1 && conf > confidence && v1.size() > support){
			//	  cout << decode[iter->first];
			//	  cout << decode[iter2->first] << endl;
				  support2 = v3.size();
				  v3.clear();
				  set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
				  for (vector<int>::iterator it = v3.begin(); it != v3.end(); it++){
					  cout << "bug: " << decode[iter->first] << " in " << decode[*it] << ", pair : (" << decode[iter->first] << ", " << decode[iter2->first] << "), support : " << support2 ;
					  printf(", confidence : %.2f%%\n", conf*100);
					  z++;
				  }
			  }
			  v3.clear();
		  }
	  }  
  }

  }


	


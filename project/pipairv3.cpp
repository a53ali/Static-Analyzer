#include <iostream>
#include <string>
#include <stdio.h>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
#include <iomanip>  

using namespace std;

void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void openCmd(string bc){
	string stream = "opt -print-callgraph ";
	stream.append(bc);
	stream.append(" 2>callgraph.txt 1>/dev/null");
	FILE *in = popen(stream.c_str() , "r");
	pclose(in);
}


int main(int argc, char* argv[]) {

  string line = "";
  map< string, set<string> > graph;
  string caller = "";
  string callee = "";
  map< string, int> functionsupport;
  int support = 3;
  float confidence = 0.65;
  
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

	//openCmd(string(argv[1]));

  //if((unsigned)strlen(argv) == 1){
  
  
  //}

  //if((unsigned)strlen(argv) == 2){
  
  
  //}
  ifstream infile;
  infile.open("callgraph.txt");
  
  map<string, int> encode;
  map<int, string> decode;
  int ID = 0;

  // Reads in the line  from input
  while(getline(infile, line)){
	  //cout << line << "\n";
	  //cout << "hello";
	  //cout << line.find("Call graph node for function: ") << "\n";
	  if(line.find("Call graph node for function: ")  == 0){
		  callee = line.substr(line.find("'")+1,line.length());
		  callee = callee.substr(0, callee.find("'"));
		//  cout << callee << "\n";
		  graph[callee];	
		  ID++;
		  encode[callee] = ID;
		  decode[ID] = callee;
	  }
	  
	  if((line.find("calls function") == 16) && callee != ""){
		 caller = line.substr(line.find("'") +1 ,line.length());
		 caller = caller.substr(0, caller.find("'")); 
		 graph[callee].insert(caller);
		 if (encode.find(caller) == encode.end()){
			 ID++;
			 encode[caller] = ID;
			 decode[ID] = caller;
		}
			//	 if (find(graph[callee].begin(),graph[callee].end(), caller) == graph[callee].end()){
			//			graph[callee].push_back(caller);// Element not in vector.
			//			sort(graph[callee].begin(),graph[callee].end()); // sort vector
			//			ID++;
			//			encode[callee] = ID;
			//			decode[ID] = callee;
			//	}
			//	cout << caller << "\n";
	  }
	}


infile.close();

	
	// Get pair support for function
  map< int, set<int> > fn;

  map< string, set<string> >::iterator it;
  set<string>::iterator it2;
  for (it = graph.begin(); it != graph.end(); it++){
	  for (it2 = it->second.begin(); it2 != it->second.end(); it2++){
		  fn[encode[*it2]].insert(encode[it->first]);
	  }
  }

 /*
  std::ofstream out("out.txt");
  std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
  std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
   */
   /*
  map< int, set<int>>::iterator it3;
  set<int>::iterator it4;
  for (it3 = fn.begin(); it3 != fn.end(); it3++){
	  cout << decode[it3->first] << "   ";
	  cout << it3->second.size() << endl;

  }
  */


  map< int, set<int> >::iterator it3;
  map< int, set<int> >::iterator it4;
  set<int> intersect;
  set<int> difference;
  set<int>::iterator it5;
  set<int> s1;
  set<int> s2;

  int supp;
  float conf;
  for (it3 = fn.begin(); it3 != fn.end(); it3++){
	  for (it4 = fn.begin(); it4 != fn.end(); it4++){
		  if (it3->first != it4->first){
			  s1 = it3->second;
			  s2 = it4->second;
			  set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
				  std::inserter(intersect, intersect.begin()));
			  supp = intersect.size();
			  conf = float(supp) / float(s1.size());
			  
			 // cout << decode[it3->first] << "  " << decode[it4->first] << supp << "   " << conf << endl;
			  if (conf < 1 && conf >= confidence && supp >= support){
				  set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(difference, difference.begin()));
				  for (it5 = difference.begin(); it5 != difference.end(); it5++){
					  //cout << decode[it3->first] << "  " << decode[it4->first] << " " << supp << "   " << conf << endl;
					  if (decode[it3->first] < decode[it4->first]){
						  
						printf( "bug: %s in %s, pair: (%s, %s), support: %d, confidence: %.2f%%\n",decode[it3->first].c_str(),decode[*it5].c_str(),decode[it3->first].c_str(),decode[it4->first].c_str(),supp,conf * 100);
							fflush(stdout);
						// cout << "bug: " << decode[it3->first] << " in " << decode[*it5] << ", pair: (" << decode[it3->first] << ", " << decode[it4->first] << "), ";
						 // cout << "support: " << supp << ", confidence: " << fixed << setprecision(2) << conf * 100 << "%" << endl;
						  }
					  else{
						   printf( "bug: %s in %s, pair: (%s, %s), support: %d, confidence: %.2f%%\n",decode[it3->first].c_str(),decode[*it5].c_str(),decode[it4->first].c_str(),decode[it3->first].c_str(),supp,conf * 100);
							fflush(stdout);
						// cout << "bug: " << decode[it3->first] << " in " << decode[*it5] << ", pair: (" << decode[it4->first] << ", " << decode[it3->first] << "), ";
						 // cout << "support: " << supp << ", confidence: " << fixed << setprecision(2) << conf * 100 << "%" << endl;
					  }
				  }
			  }
			//	  set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
			//		  std::inserter(difference, difference.begin()));
			//	  for (it5 = difference.begin(); it5 != difference.end(); it5++){
			//		  cout << support << " " << decode[it3->first] << " " << decode[it4->first] << endl;
			//	  }
			//  } 
			  intersect.clear();
			  difference.clear();
		  }
	  }
  }

 
		clean_stdin();
	//cout << functionsupport["A B"] << endl;
	
	
 }

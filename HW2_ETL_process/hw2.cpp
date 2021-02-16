#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
#include <vector>
#include <time.h>
#include <chrono>

using namespace std;

void transfer(string str, string& ans, chrono::steady_clock::time_point& THRSTART, chrono::steady_clock::time_point& THREND){
	THRSTART = chrono::steady_clock::now();
	string substr;
	stringstream ss1(str);
	string anstmp;
	int k = 0;
	while(getline(ss1, substr)){
		k++;
		string subsubstr;
		stringstream ss(substr);
		ans += "\t{\n";
		for(int i = 0; i < 20; ++i){
			getline(ss, subsubstr, '|');
			if(i < 19){
				ans += "\t\t\"col_";
				ans += to_string(i+1);
				ans += "\":";
				ans += subsubstr;
				ans += ",\n";
			}
			else if(i == 19){
				ans += "\t\t\"col_";
				ans += to_string(i+1);
				ans += "\":";
				ans += subsubstr;
				ans += "\n\t},\n"; 
			}
		}
	}
	THREND = chrono::steady_clock::now();
}

int main(int argc, char** argv){
	chrono::steady_clock::time_point START,TOTALEND,THRSTART,THREND,IOSTART1,IOEND1,IOSTART2,IOEND2,CUTSTART,CUTEND; 
    START = chrono::steady_clock::now();
	IOSTART1 = chrono::steady_clock::now();
	int threadnum = atoi(argv[1]);
	fstream infile;    
	string input;
	string ansarr[threadnum];
	string data[threadnum];
	long long int linenum = 0; 
	thread threadarr[threadnum];
	infile.open("input.csv", ios::in); 
	if(infile.fail())
		cout << "input file can't open!\n";
	else{
		string str;
		while(getline(infile, str)){
			linenum++;
		}
		infile.clear();
		infile.seekg(0);
	}

	IOEND1 = chrono::steady_clock::now();
	CUTSTART = chrono::steady_clock::now();
	long long int count = linenum / threadnum;
	//cout <<linenum <<endl; 
	long long int i = 0;
	long long int j = 0;
	string temp;
	string tmp;
	while(getline(infile, tmp)){
	
		i++;
		temp += tmp;
		temp += "\n";
		cout<<i<<endl  ;
		//cout << tmp;
		
		if(i % count == 0){
			data[j] = temp;
			temp = "";		
			cout << data[j]<<endl;
			if(j < threadnum-1){
				j++;
			}
		
		}
		cout << "j"<<j<<endl;
		
	}
	
	if(i % count != 0){
		data[j] += temp;
		temp = "";
			cout << data[j]<<endl;
		//j++;
	}/*
	for(long long int k = 0; k < threadnum; ++k){
		cout << data[k]<<endl;
	}*/
	cout << "j"<<j<<endl;
	infile.close();
	CUTEND = chrono::steady_clock::now();
	ansarr[0].insert(0, "[\n");
	for(int k = 0; k < threadnum; ++k){
		threadarr[k] = std::thread(transfer, data[k], ref(ansarr[k]), ref(THRSTART), ref(THREND));
	}
	for(int k = 0; k < threadnum; ++k){
		threadarr[k].join();
	}
	IOSTART2 = chrono::steady_clock::now();
	fstream outfile;
	outfile.open("output.json", ios::out | ios::trunc); 
	if(outfile.fail())
		cout << "output file can't open!\n";
	else{
		for(int k = 0 ; k < threadnum-1; ++k){
			cout << ansarr[k];
			outfile << ansarr[k];	
		}
		ansarr[threadnum-1].erase(ansarr[threadnum-1].end()-2, ansarr[threadnum-1].end()-1);
		ansarr[threadnum-1] += "]";
		outfile << ansarr[threadnum-1];
	}
	outfile.close();
	IOEND2 = chrono::steady_clock::now();
	TOTALEND = chrono::steady_clock::now();
	cout << "I/O cost¡G" << chrono::duration_cast<std::chrono::microseconds>(IOEND1 - IOSTART1+IOEND2-IOSTART2).count() / 1000000.0 << " sec" << endl;
	cout << "cut data cost¡G" << chrono::duration_cast<std::chrono::microseconds>(CUTEND - CUTSTART).count() / 1000000.0 << " sec" << endl;
	cout << "thread cost¡G" << chrono::duration_cast<std::chrono::microseconds>(THREND - THRSTART).count() / 1000000.0 << " sec" << endl;
	cout << "Total cost¡G" << chrono::duration_cast<std::chrono::microseconds>(TOTALEND - START).count() / 1000000.0 << " sec" << endl;
	return 0;
}


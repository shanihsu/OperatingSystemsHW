#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;


int main(int argc, char** argv){
	fstream infile;  
	fs::create_directories("./temp");    
    long long int block = 500000000;
	//long long int maxblock = block;
    long long int num = 1;
    string in = argv[1];
	infile.open(in, ios::in);
    if(infile.fail())
		cout << "file can't open!\n";
	else{
		vector<int> buffer; //cut data into size of blocks
		while(!infile.eof()){  //read file
			int judge = 0;
			int x;
			buffer.clear();
			//cut 'block' data once time
			for(long long int i = 0; i < block; ++i){
				infile >> x;
				if(infile.eof()){
					if(i == 0){
						judge = 2;
						break;
					}
					judge = 1;
					break;
				}
				buffer.push_back(x);
				/*if (block < buffer.capacity()) {
			    	block = buffer.capacity();
			    }
			    if(maxblock < block){
			    	maxblock = block;
				}*/
			}
			if(judge == 2)
				break;
			string s = "./temp/run" + to_string(1)+"_"+to_string(num) + ".txt";
			num++;
			sort(buffer.begin(), buffer.end());
			fstream tmpfile;
			tmpfile.open(s, ios::out | ios::trunc);
			for(long long int i = 0; i < buffer.size(); ++i){
				tmpfile << buffer[i] <<endl;
			}
			tmpfile.close();
			if(judge == 1)
				break;
		}
	}
	num--;
	infile.close();
	long long int blocksize = block/3;
	vector<int> inbf1(blocksize); //cut data into size of blocks
	vector<int> inbf2(blocksize); //cut data into size of blocks
	vector<int> outbf(blocksize); //cut data into size of blocks
	int k = 1;
	string s1;
	string s2;
	string s3;
	while(num > 1){
		int count = 0;//count number of every run file
		for(long long int i = 1; i <= num; i += 2){  //read number of files
			count++;
			s1 = "./temp/run" +to_string(k)+"_"+ to_string(i) + ".txt";
			s2 = "./temp/run" +to_string(k)+"_"+ to_string(i+1) + ".txt";
			s3 = "./temp/run" +to_string(k+1)+"_"+ to_string(i/2+1) + ".txt";
			fstream tmpfile1;
			fstream tmpfile2;
			fstream runfile;
			tmpfile1.open(s1, ios::in);
			tmpfile2.open(s2, ios::in);
			runfile.open(s3, ios::out | ios::trunc);
			inbf1.clear();
			inbf2.clear();
			outbf.clear();
			
			//read file
			int cancatch1 = 0;
			int cancatch2 = 0;
			if(!tmpfile1.fail()){
				for(long long int j = 0; j < blocksize; ++j){
					int x;
					tmpfile1 >> x;
					if(tmpfile1.eof()){
						break;
					}
					inbf1.push_back(x);
				}
			}
			else if(tmpfile1.fail()){
				cancatch1 = 1;
			}
			
			if(!tmpfile2.fail()){
				for(long long int j = 0; j < blocksize; ++j){
					int x;
					tmpfile2 >> x;
					if(tmpfile2.eof()){
						break;
					}
					inbf2.push_back(x);
				}
			}
			else if(tmpfile2.fail()){
				cancatch2 = 1;
			}
			//sort
			while((!(cancatch1 ==1 && cancatch2 == 1 && inbf1.size() == 0 && inbf2.size() == 0))){
				int judge = 0;
				while(!(inbf1.size() == 0 || inbf2.size() == 0)){  //if inbf1 or inbf2 are not null,then compare
					if(outbf.size() == blocksize){
						for(long long int j = 0; j < blocksize; ++j){
							runfile << outbf[j] << endl;
						}
						outbf.clear();
					}
					if(inbf1[0] <= inbf2[0]){
						outbf.push_back(inbf1[0]);
						inbf1.erase(inbf1.begin());
					} 
					else{
						outbf.push_back(inbf2[0]);
						inbf2.erase(inbf2.begin());
					}
				}
				if(inbf1.size() == 0 && cancatch1 == 0){  //if inbf1 is null and tmpfile1 is not null,then catch data
					for(long long int j = 0; j < blocksize; ++j){
						int x;
						tmpfile1 >> x;
						if(tmpfile1.eof()){
							cancatch1 = 1;
							break;
						}
						inbf1.push_back(x);	
					}
				}
				else if(inbf2.size() == 0 && cancatch2 == 0){ //if inbf2 is null and tmpfile2 is not null,then catch data
					for(long long int j = 0; j < blocksize; ++j){
						int x;
						tmpfile2 >> x;
						if(tmpfile2.eof()){
							cancatch2 = 1;
							break;
						}
						inbf2.push_back(x);
					}
				}
				else if(inbf1.size() == 0 && cancatch1 == 1){ //if inbf1 is null and tmpfile1 is null,then catch data from tmpfile2
					for(long long int j = 0; j < inbf2.size(); ++j){
						outbf.push_back(inbf2[0]);
						inbf2.erase(inbf2.begin());
					}
					while(!tmpfile2.eof()){
						for(long long int j = 0; j < blocksize; ++j){
							int x;
							tmpfile2 >> x;
							if(tmpfile2.eof()){
								cancatch2 = 1;
								break;
							}
							inbf2.push_back(x);
						}
						for(long long int j = 0; j < inbf2.size(); ++j){
							outbf.push_back(inbf2[0]);
							inbf2.erase(inbf2.begin());
						}
					}
				}
				else if(inbf2.size() == 0 && cancatch2 == 1){ //if inbf1 is null and tmpfile1 is null,then catch data from tmpfile2
					for(long long int j = 0; j < inbf1.size(); ++j){
						outbf.push_back(inbf1[0]);
						inbf1.erase(inbf1.begin());
					}
					while(!tmpfile1.eof()){
						for(long long int j = 0; j < blocksize; ++j){
							int x;
							tmpfile1 >> x;
							if(tmpfile1.eof()){
								cancatch1 = 1;
								break;
							}
							inbf1.push_back(x);
						}
						for(long long int j = 0; j < inbf1.size(); ++j){
							outbf.push_back(inbf1[0]);
							inbf1.erase(inbf1.begin());
						}
					}
				}
			}
			if(cancatch1 == 1 && cancatch2 == 1){
				for(long long int j = 0; j < outbf.size(); ++j){
					runfile << outbf[j] << endl;
				}
				outbf.clear();
			}
			tmpfile1.close();
			tmpfile2.close();
			runfile.close();
			remove(s1.c_str());
			remove(s2.c_str());
		}
		k++;
		num = count;
	}
	string str = "output.txt";
	string tmps = "./temp/run1_1.txt";
	fstream tmp;
	tmp.open(str);
	if(tmp.good()){
		tmp.close();
		remove(str.c_str());
	}
	else
		tmp.close();
	rename(s3.c_str(), str.c_str());
	rename(tmps.c_str(), str.c_str());
	fs::remove_all("./temp");
	return 0;
}


#include <iostream>
#include <fstream>
#include <string>
#include <experimental/filesystem>
#include <sstream>
#include <cstddef>         // std::size_t

using namespace std;
namespace fs = std::experimental::filesystem;

#define PAGE_SIZE 10
#define VALUE_SIZE 4

struct keyvalue
{
	long long key = -1;
	string value;
};

struct pagetable
{
	long long hittime = 0;
	long long index = -1;
	keyvalue kv[VALUE_SIZE];
};


int main(int argc, char** argv){
	pagetable pt[PAGE_SIZE];
	int pagenum = 0; //numbers of page
	fstream infile, outfile, tmpfile;
	string filename = argv[1];
	string tmpname, outname;
	size_t found = filename.find_last_of("/\\");
	size_t tail = filename.find(".input");
	outname = filename.substr(found+1, tail-found-1);
	infile.open(filename,ios::in);
	fs::create_directories("./storage");
	if(infile.fail())
		cout << "file can't open!\n";
	else{
		string str, value;
		long long key1,key2;
		int isHit = 0, isFirst = 1;
		while (infile >> str){
			switch (str[0]){
				case 'P': //PUT
					infile >> key1;
					infile >> value;
					
					//find key hit page or not
					isHit = 0;
					for(int i = 0; i < pagenum; ++i){
						//page which key in is in memory 
						if(key1/VALUE_SIZE == pt[i].index){
							pt[i].kv[(key1%VALUE_SIZE)].key = key1;
							pt[i].kv[(key1%VALUE_SIZE)].value = value;
							isHit = 1;
							break;
						}
					}
					//key doesn't hit page
					if(isHit == 0){
						if(pagenum < PAGE_SIZE){ //add page
							pt[pagenum].index = key1/VALUE_SIZE;
							pt[pagenum].kv[(key1%VALUE_SIZE)].key = key1;
							pt[pagenum].kv[(key1%VALUE_SIZE)].value = value;
							pagenum++;
						}
						else if(pagenum >= PAGE_SIZE){ //add to file
							long long filekey;
							string filevalue;
							int isExist = 0; 
							tmpname = "./storage/";
							tmpname += to_string(key1/VALUE_SIZE);
							tmpname += ".txt";
							tmpfile.open(tmpname, ios::out|ios::app);
							if(tmpfile.fail())
								cout << "tmpoutfile can't open!\n";
							tmpfile.close();
							tmpfile.open(tmpname, ios::in|ios::out);
							if(tmpfile.fail())
								cout << "tmpinfile can't open!\n";
							while(tmpfile >> filekey){
								cout <<filekey<<" "<< key1<<endl;
								if(filekey == key1){ //key is exist in file, so update
									isExist = 1;
									cout << tmpfile.tellg()<<"\n";
									cout << tmpfile.tellp()<<"\n";
									tmpfile.seekp(tmpfile.tellp());
									tmpfile << " " << value <<"\n";
									break;
									
								}
								tmpfile >> filevalue;
							}
							tmpfile.close();
							if(isExist == 0){ //key isn't exsit in file, so add to end of file
								tmpfile.open(tmpname, ios::out | ios::app);
								tmpfile << key1 <<" " << value <<"\n";
								//cout << key1 <<" " << value <<"\n";
								tmpfile.close();
							}
							
						}
					}
				break;
				case 'G': //GET
					infile >> key1;
					outname = "./" + outname;
					outname += ".output";
					if(isFirst == 1){
						//outfile.open(outname, ios::out | ios::trunc);
					}
					//key in memory
					for(int i = 0; i < pagenum; ++i){
						if(key1/VALUE_SIZE == pt[i].index){
							//outfile << pt[i].kv[key1%VALUE_SIZE];
						}
					}
				break;
				case 'S': //SCAN
					infile >> key1 >> key2;
				break;
			}
		}
		//store page into file
		for(int i = 0; i < pagenum; ++i){
			tmpname = "./storage/";
			tmpname += to_string(pt[i].index);
			tmpname += ".tmp";
			tmpfile.open(tmpname,ios::out | ios::trunc);
			for(int j = 0; j < VALUE_SIZE; ++j){
				if(pt[i].kv[j].key != -1){
					key1 = pt[i].kv[j].key;
					value = pt[i].kv[j].value;
					tmpfile << key1 <<" " << value <<"\n";
				}
			}
			tmpfile.close();	
		}
	}
	infile.close();
	return 0;
} 

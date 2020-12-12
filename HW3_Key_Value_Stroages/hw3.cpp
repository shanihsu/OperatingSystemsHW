#include <iostream>
#include <fstream>
#include <string>
#include <experimental/filesystem>
#include <sstream>
#include <cstddef>         // std::size_t

using namespace std;
namespace fs = std::experimental::filesystem;

#define PAGE_SIZE 1000
#define VALUE_SIZE 100

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
	/*cout <<sizeof(int)<<endl;
	cout << sizeof(keyvalue)<<endl;
	cout << sizeof(pagetable)<<endl;*/
	pagetable pt[PAGE_SIZE];
	int pagenum = 0; //numbers of page
	fstream infile, outfile, tmpfile;
	string filename = argv[1];
	string tmpname, outname,findname;
	size_t found = filename.find_last_of("/\\");
	size_t tail = filename.find(".input");
	findname = filename.substr(found+1, tail-found-1);
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
							tmpname += ".tmp";
							tmpfile.open(tmpname, ios::out|ios::app);
							if(tmpfile.fail())
								cout << "tmpoutfile can't open!\n";
							tmpfile.close();
							tmpfile.open(tmpname, ios::in|ios::out);
							if(tmpfile.fail())
								cout << "tmpinfile can't open!\n";
							while(tmpfile >> filekey){
								if(filekey == key1){ //key is exist in file, so update
									isExist = 1;
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
								tmpfile.close();
							}
						}
					}
				break;
				case 'G': //GET
					infile >> key1;
					outname = "./" + findname;
					outname += ".output";
					isHit = 0;
					if(isFirst == 1){  //first output
						outfile.open(outname, ios::out | ios::trunc);
						isFirst = 0;
					}
					else{
						outfile.open(outname, ios::out | ios::app);
					}
				
					//key in memory
					for(int i = 0; i < pagenum; ++i){
						if(key1/VALUE_SIZE == pt[i].index){
							if(pt[i].kv[key1%VALUE_SIZE].key != -1){ //get key in page
								outfile << pt[i].kv[key1%VALUE_SIZE].value << "\n";
							}
							else{
								outfile << "EMPTY\n";
							}
							pt[i].hittime++;
							isHit = 1;
							break;
						}
					}
					//key isn't in memory
					if(isHit == 0){
						long long ptnum = pagenum, min, minnum;
						if(pagenum >= PAGE_SIZE){
							//put page which the smallest hittime into file
							min = pt[0].hittime, minnum = 0;
							for(int i = 0; i < pagenum; ++i){ //find min used page
								if(pt[i].hittime < min){
									min = pt[i].hittime;
									minnum = i;
								}
							}
							//put page to file
							tmpname = "./storage/";
							tmpname += to_string(pt[minnum].index);
							tmpname += ".tmp";
							tmpfile.open(tmpname, ios::out|ios::trunc);
							if(tmpfile.fail())
								cout << "tmpputfile1 can't open!\n";
							for(int i = 0; i < VALUE_SIZE; ++i){
								if(pt[minnum].kv[i].key != -1){
									tmpfile << pt[minnum].kv[i].key << " " << pt[minnum].kv[i].value <<"\n";
								}
							}
							tmpfile.close();
							ptnum = minnum;
						}
						//put file which key1 is in to page
						tmpname = "./storage/";
						tmpname += to_string(key1/VALUE_SIZE);
						tmpname += ".tmp";
						tmpfile.open(tmpname, ios::out|ios::app);
						if(tmpfile.fail())
							cout << "tmpputfile2 can't open!\n";
						tmpfile.close();
						tmpname = "./storage/";
						tmpname += to_string(key1/VALUE_SIZE);
						tmpname += ".tmp";
						tmpfile.open(tmpname, ios::in);
						if(tmpfile.fail())
							cout << "tmpputfile3 can't open!\n";
						pt[ptnum].index = key1/VALUE_SIZE;
						if(ptnum == minnum){ //page is full
							pt[minnum].hittime = 0;
							for(int i = 0; i < VALUE_SIZE; ++i){
								pt[minnum].kv[i].key = -1;
								pt[minnum].kv[i].value = "";
							}
						}
						long long filekey;
						string filevalue;
						while(tmpfile >> filekey){ //store context from file to page
							pt[ptnum].kv[filekey%VALUE_SIZE].key = filekey;
							tmpfile >> filevalue;
							pt[ptnum].kv[filekey%VALUE_SIZE].value = filevalue;
						}
						tmpfile.close();
						if(pt[ptnum].kv[key1%VALUE_SIZE].key != -1){ //get key in page
							outfile << pt[ptnum].kv[key1%VALUE_SIZE].value << "\n";
						}
						else{
							outfile << "EMPTY\n";
						}
						pt[ptnum].hittime++;
						if(ptnum == pagenum) pagenum++;
					}
					outfile.close();
				break;
				case 'S': //SCAN
					infile >> key1 >> key2;
					outname = "./" + findname;
					outname += ".output";
					isHit = 0;
					if(isFirst == 1){  //first output
						outfile.open(outname, ios::out | ios::trunc);
						isFirst = 0;
					}
					else{
						outfile.open(outname, ios::out | ios::app);
					}
					for(long long k = key1/VALUE_SIZE; k <= key2/VALUE_SIZE; ++k){
						long long keytmp1 = k*VALUE_SIZE, keytmp2 = (k+1)*VALUE_SIZE-1;
						if(keytmp1 < key1){
							keytmp1 = key1;
						}
						if(keytmp2 > key2){
							keytmp2 = key2;
						}
						//key in memory
						for(int i = 0; i < pagenum; ++i){
							if(keytmp1/VALUE_SIZE == pt[i].index){
								for(long long j = keytmp1; j <= keytmp2; ++j){
									if(pt[i].kv[j%VALUE_SIZE].key != -1){ //get key in page
										outfile << pt[i].kv[j%VALUE_SIZE].value << "\n";
									}
									else{
										outfile << "EMPTY\n";
									}
									pt[i].hittime++;
								}
								isHit = 1;
								break;
							}
						}
						//key isn't in memory
						if(isHit == 0){
							long long ptnum = pagenum, min, minnum;
							if(pagenum >= PAGE_SIZE){
								//put page which the smallest hittime into file
								min = pt[0].hittime, minnum = 0;
								for(int i = 0; i < pagenum; ++i){ //find min used page
									if(pt[i].hittime < min){
										min = pt[i].hittime;
										minnum = i;
									}
								}
								//put page to file
								tmpname = "./storage/";
								tmpname += to_string(pt[minnum].index);
								tmpname += ".tmp";
								tmpfile.open(tmpname, ios::out|ios::trunc);
								if(tmpfile.fail())
									cout << "tmpputfile1 can't open!\n";
								for(int i = 0; i < VALUE_SIZE; ++i){
									if(pt[minnum].kv[i].key != -1){
										tmpfile << pt[minnum].kv[i].key << " " << pt[minnum].kv[i].value <<"\n";
									}
								}
								tmpfile.close();
								ptnum = minnum;
							}
							
							//put file which key1 is in to page
							//initial
							tmpname = "./storage/";
							tmpname += to_string(keytmp1/VALUE_SIZE);
							tmpname += ".tmp";
							tmpfile.open(tmpname, ios::out|ios::app);
							if(tmpfile.fail())
								cout << "tmpputfile2 can't open!\n";
							tmpfile.close();
							tmpname = "./storage/";
							tmpname += to_string(keytmp1/VALUE_SIZE);
							tmpname += ".tmp";
							tmpfile.open(tmpname, ios::in);
							if(tmpfile.fail())
								cout << "tmpputfile3 can't open!\n";
							pt[ptnum].index = keytmp1/VALUE_SIZE;
							if(ptnum == minnum){  //page is full
								pt[minnum].hittime = 0;
								for(int i = 0; i < VALUE_SIZE; ++i){
									pt[minnum].kv[i].key = -1;
									pt[minnum].kv[i].value = "";
								}
							}
							
							//store
							long long filekey;
							string filevalue;
							while(tmpfile >> filekey){ //store context from file to page
								pt[ptnum].kv[filekey%VALUE_SIZE].key = filekey;
								tmpfile >> filevalue;
								pt[ptnum].kv[filekey%VALUE_SIZE].value = filevalue;
							}
							tmpfile.close();
							//get
							for(long long j = keytmp1; j <= keytmp2; ++j){
								if(pt[ptnum].kv[j%VALUE_SIZE].key != -1){ //get key in page
									outfile << pt[ptnum].kv[j%VALUE_SIZE].value << "\n";
								}
								else{
									outfile << "EMPTY\n";
								}
								pt[ptnum].hittime++;
							}
							if(ptnum == pagenum) pagenum++;
						}
					}
					outfile.close();
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

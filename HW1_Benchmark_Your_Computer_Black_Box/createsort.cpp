#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <vector>
#include <cstdio>
#include "math.h"
#include "time.h"

using namespace std;

int main(){
	long long int number;
	cin >> number; //create number of data
	default_random_engine generator;
	uniform_int_distribution<int> distribution(-2147483648,2147483647);
	//create input.txt
	fstream infile;     
	infile.open("input.txt", ios::out | ios::trunc); 
	if(infile.fail())
		cout << "file can't open!\n";
	else{
		for(long long int i = 0; i < number; ++i){
			infile << distribution(generator) << endl;
		}
	}
	infile.close();
	return 0;
}


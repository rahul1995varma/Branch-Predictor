#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iomanip>
#include<map>

typedef unsigned long ulong;

using namespace std;

class gshare{

 protected:
  ulong index, predictions, mispredictions; 
  int mask1, mask2, gbhr, predicted_branch, size, iG, h;
  map<int,int> predictor_table;
  map<int,int>::iterator it;

 public:
  void init(int iG, int h) {
    this->iG = iG;
    this->h = h;
    index = predictions = mispredictions = 0;
    gbhr = 0;
    size = (int) pow(2, (double) iG);
    mask1 = size - 1;
    mask2 = (int) pow(2, (double) iG-h) - 1;
    for (int i = 0; i < size; i++) predictor_table[i] = WEAKLY_TAKEN;
  }

  void set_index(ulong address){
    ulong temp, xored;
    temp = address >> 2;
    temp = temp & mask1;
    if (h > 0) {
      xored = gbhr ^ (temp >> (iG - h));
      xored = xored << (iG - h);
      temp = temp & mask2; 
      index = xored | temp;
    }
    else
      index = temp;
  }

  int access(ulong address, int actual_branch){
    predictions++;
    set_index(address);
    predicted_branch = predictor_table.at(index) >= 2 ? TAKEN : NOTTAKEN;
    return update_predictor(actual_branch, predicted_branch); 
  }

  int update_predictor(int actual_branch, int predicted_branch){
    switch (actual_branch){
    case TAKEN:
      if(predictor_table.at(index) < 3) {
	predictor_table.at(index)++;
      }
      update_gbhr(1);
      if(predicted_branch == NOTTAKEN) {
	mispredictions++;
	return 0;
      }
      break;

    case NOTTAKEN:
      if(predictor_table.at(index) > 0) {
	predictor_table.at(index)--;
      }
      update_gbhr(0);
      if(predicted_branch == TAKEN) {
	mispredictions++;
	return 0;
      }
      break; 
    }
    return 1;
  }

  void update_gbhr(int i){
    if (h > 0) {
      gbhr = gbhr >> 1; 
      gbhr = gbhr | (i << (h - 1));
    }
  }
  
  bool is_taken(ulong address){
    set_index(address);
    return predictor_table.at(index) >=2 ? true : false;
  }


  void print_output(){
    cout << "OUTPUT\nnumber of predictions: " << predictions << "\nnumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (float)mispredictions*100/predictions << "%\n";
  }

  void print_stats(){
    cout << "FINAL GSHARE CONTENTS\n";
    for (it=predictor_table.begin(); it!=predictor_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n";
  }
  
};


  

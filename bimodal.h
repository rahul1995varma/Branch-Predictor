#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iomanip>
#include<map>

typedef unsigned long ulong;

#ifndef COMM_ENUMS
#include "common_enums.h"
#define COMM_ENUMS
#endif



using namespace std;

class bimodal{
 protected:

  ulong index, predictions, mispredictions;
  int mask, predicted_branch, size; 
  map<int,int> predictor_table;
  map<int,int>::iterator it;

 public:
  void init(int iB) {
    index = predictions = mispredictions = 0; 
    size = (int) pow(2, (double) iB);
    mask  = size - 1;
    for (int i = 0; i < size; i++) predictor_table[i] = WEAKLY_TAKEN; 
  }

  void set_index(ulong address){
    index = address >> 2;
    index = index & mask; 
  }

  int access(ulong address, int actual_branch){
    predictions++;
    set_index(address);
    predicted_branch = predictor_table.at(index) >= 2 ? TAKEN : NOTTAKEN;   
    return update_predictor(actual_branch, predicted_branch);
  }

  bool is_taken(ulong address){
    set_index(address);
    return predictor_table.at(index) >=2 ? true : false;    
  }

  int update_predictor(int actual_branch, int predicted_branch) {
    switch (actual_branch){
    case TAKEN:
      if(predictor_table.at(index) < 3) {
	predictor_table.at(index)++;
      }
      if(predicted_branch == NOTTAKEN) {
	mispredictions++;
	return 0;
      }
      break;

    case NOTTAKEN:
      if(predictor_table.at(index) > 0) {
	predictor_table.at(index)--;
      }
      if(predicted_branch == TAKEN) {
	mispredictions++;
	return 0;
      }
      break; 
    }
    return 1;
  }
  
  void print_output(){
    cout << "OUTPUT\nnumber of predictions: " << predictions << "\nnumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (float)mispredictions*100/predictions << "%\n";
  }

  void print_stats(){
    cout << "FINAL BIMODAL CONTENTS\n";
    for (it=predictor_table.begin(); it!=predictor_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n"; 
  }
  
};



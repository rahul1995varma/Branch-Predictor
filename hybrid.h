#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iomanip>
#include<map>

#include "bimodal.h"
#include "gshare.h"
typedef unsigned long ulong;

class hybrid : public bimodal, public gshare {

 protected:
  ulong index, predictions, mispredictions;
  int mask, predicted_branch, size;
  bool gshare_predict, bimodal_predict;
  map<int, int> chooser_history_table;
  map<int, int>::iterator it;
  bimodal * b;
  gshare * g;
  
 public:
  void init(int iB, int iG, int h, int k) {
    b = new bimodal;
    g = new gshare;
    b->init(iB);
    g->init(iG,h);
    index = predictions = mispredictions = 0;
    size = (int) pow(2, (double) k);
    mask = size - 1;
    for (int i = 0; i < size; i++) chooser_history_table[i] = WEAKLY_NOT_TAKEN; 
  }

  void set_index(ulong address){
    index = address >> 2;
    index = index & mask;    
  }

  void if_misprediction(bool prediction, int actual_branch){
    if ((prediction && actual_branch == NOTTAKEN) || (!prediction && actual_branch == TAKEN))
      mispredictions++; 
  }

  bool if_correct_prediction(bool prediction, int actual_branch){
    return (((prediction && actual_branch == TAKEN) || (!prediction && actual_branch == NOTTAKEN)));    
  }

  int access(ulong address, int actual_branch){
    predictions++;
    set_index(address);
    bimodal_predict = b->is_taken(address);
    gshare_predict = g->is_taken(address);

    if(chooser_history_table.at(index) < 2){
      if_misprediction(bimodal_predict, actual_branch);
      g->update_gbhr(actual_branch);
      b->access(address, actual_branch);
    } else {
      if_misprediction(gshare_predict, actual_branch);
      g->access(address, actual_branch); 
    }

    bool bimodal_correct = if_correct_prediction(bimodal_predict, actual_branch);
    bool gshare_correct = if_correct_prediction(gshare_predict, actual_branch);

    if(bimodal_correct && !gshare_correct)
      if(chooser_history_table.at(index) > 0)
	chooser_history_table.at(index)--;

    if(gshare_correct && !bimodal_correct)
      if(chooser_history_table.at(index) < 3)
	chooser_history_table.at(index)++;    

    return 1;
  }

  void print_output(){
    cout << "OUTPUT\nnumber of predictions: " << predictions << "\nnumber of mispredictions: " << mispredictions << fixed << setprecision(2) << "\nMisprediction rate: " << (float)mispredictions*100/predictions << "%\n";    
  }

  void print_stats(){
    cout << "FINAL CHOOSER CONTENTS\n";
    for (it=chooser_history_table.begin(); it!=chooser_history_table.end(); ++it) 
      cout << it->first << "\t" << it->second << "\n";
    g->print_stats();
    b->print_stats();
  }
  
};


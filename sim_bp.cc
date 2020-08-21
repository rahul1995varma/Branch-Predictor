#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iomanip>
#include <cassert>
#include <map>
#include "hybrid.h"

int PREDICTOR_TYPE;

typedef unsigned long ulong;
using namespace std;

bool read_command_line_input (int argc, char *argv[], int *iB, int *iG, int *h,int *k, FILE ** trace_file){
  char * fname;
  if((argc < 4) || (argc > 7)) {
    cout << "Invalid Number of Arguments" << endl;
    cout << "\nUsage:\n\t\t./sim bimodal <M2> <tracefile>\nor\t\t./sim gshare <M1> <N> <tracefile>\nor\t\t./sim hybrid <K> <M1> <N> <M2> <tracefile>\n\n";
    return false;
  }

  if(strcmp(argv[1],"bimodal") == 0) PREDICTOR_TYPE = BIMODAL;
  else if(strcmp(argv[1],"gshare") == 0) PREDICTOR_TYPE = GSHARE;
  else if(strcmp(argv[1],"hybrid") == 0) PREDICTOR_TYPE = HYBRID;

  switch(PREDICTOR_TYPE){
  case BIMODAL:
    *iB = atoi(argv[2]);
    fname = argv[3];
    cout << "COMMAND\n./sim_bp " << argv[1] << " " <<  *iB << " " << fname << endl; 
    break;

  case GSHARE:
    *iG = atoi(argv[2]);
    *h = atoi(argv[3]);
    if (*h > *iG){ 
      cout << "Value of h greater than iG" << endl;
      return false;
    }
    fname = argv[4];
    cout << "COMMAND\n./sim_bp " << argv[1] << " " << *iG << " " << *h  << " " << fname << endl;
    break;

  case HYBRID:
    *k = atoi(argv[2]);
    *iG = atoi(argv[3]);
    *h = atoi(argv[4]);
    *iB = atoi(argv[5]); 
    if( *h > *iG){ 
      cout << "Value of h greater than iG" << endl;
      return false;
    }
    fname = argv[6];
    cout << "COMMAND\n./sim_bp " << argv[1] << " "<< *k << " " << *iG << " " << *h  << " " << *iB  << " "<< fname << endl;
    break;

  default:
    cout << "\nWrong predictor type" << endl;
    return false;
  }


  assert(fname);
  *trace_file = fopen(fname, "r+");
  if(*trace_file == 0) {
    cout << "\nThere is a problem reading the trace file." <<endl;
    return false;
  }
  return true;
}

int branch_label(char actual_branch){
  if(actual_branch == 't') return TAKEN;
  else return NOTTAKEN;
}

int main(int argc, char *argv[]){
  ulong address;
  char actual_branch;
  int iB, iG, h, k;
  FILE * trace_file;
  bool is_config_success = read_command_line_input(argc, argv, &iB, &iG, &h, &k, &trace_file);

  if(!is_config_success) return 0;
  bimodal * b = new bimodal;
  gshare * g = new gshare;
  hybrid * H = new hybrid;

  assert(PREDICTOR_TYPE <= 2);

  switch(PREDICTOR_TYPE){
  case BIMODAL: 
    b->init(iB);
    while(1) {
      if(fscanf(trace_file, "%lx %c", &address, &actual_branch) != EOF)
	  b->access(address, branch_label(actual_branch));
	else break;
      }
      b->print_output();
      b->print_stats();
      break;

    case GSHARE:
      g->init(iG,h);  
      while(1) {
	if(fscanf(trace_file, "%lx %c", &address, &actual_branch) != EOF)
	  g->access(address, branch_label(actual_branch));
	else break;
      }
      g->print_output();
      g->print_stats();
      break;

    case HYBRID:
      H->init(iB, iG, h, k);
      while(1) {
	if(fscanf(trace_file, "%lx %c", &address, &actual_branch) != EOF)
	  H->access(address, branch_label(actual_branch));
	else break;
      }
      H->print_output();
      H->print_stats();
      break;
    
    default:
      cout << "not implemented yet" << endl;
    
    }
    return 1;
  }

#include "data.h"
#include "model.h"
#include "prob.h"
#include <vector>
#include <utility>
#include <stdio.h>
#include <iostream>
using namespace std;

void initModel( const char* );
void merge( WordsDistrib&, WordsDistrib& );
vector< pair< int, prob > > normalize( WordsDistrib& );
void outputFile( int, Model& );

int main() {
  FILE* fin = fopen( "filelist_train/filelist.list", "r" );

  char list[ 100 ];
  while ( fscanf( fin, "%s", list ) != EOF ) 
    initModel( list );

  fclose( fin );
}

void initModel( const char* list ) {
  cout << list << endl;
  FILE* fin = fopen( list, "r" ); 

  char file[ 100 ];
  WordsDistrib trainData, temp;
  while ( fscanf( fin, "%s", file ) != EOF ) {
    temp = read( file );
    //merge( trainData, temp );
  }
}

void merge( WordsDistrib& data, WordsDistrib& give ) {
  //TODO
  //data may be empty
}

vector< pair< int, prob > > normalize( WordsDistrib& data ) {
  //TODO
  vector< pair< int, prob > > temp;
  return temp;
}

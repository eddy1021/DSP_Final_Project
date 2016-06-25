#include "data.h"
#include "model.h"
#include "prob.h"
#include <vector>
#include <utility>
#include <stdio.h>
#include <iostream>
using namespace std;

void initModel( Model&, const char* );
void merge( WordsDistrib&, WordsDistrib& );
vector< pair< int, prob > > transform( WordsDistrib& );

int main() {
  FILE* fin = fopen( "filelist_train/filelist.list", "r" );

  Model m;
  int index = 0;
  char list[ 100 ], fname[ 100 ];
  while ( fscanf( fin, "%s", list ) != EOF ) { 
    initModel( m, list );
    sprintf( fname, "models/model_%02d.mod", index++ );
    m.write( fname );
  }

  fclose( fin );
}

void initModel( Model& m, const char* list ) {
  FILE* fin = fopen( list, "r" ); 

  char file[ 100 ];
  WordsDistrib trainData, temp;
  while ( fscanf( fin, "%s", file ) != EOF ) {
    temp = read( file );
    merge( trainData, temp );
  }

  m.para = transform( trainData );
}

void merge( WordsDistrib& data, WordsDistrib& give ) {
  if ( data.empty() ) 
    data = give;
  else {
    WordsDistrib temp;
    size_t ig = 0;
    for ( size_t i = 0 ; i < data.size() ; ++i ) {
      if ( ig >= give.size() || data[ i ].Word < give[ ig ].Word )
        temp.push_back( data[ i ] );
      else if ( data[ i ].Word == give[ ig ].Word ) { 
        temp.push_back( make_pair( data[ i ].Word, data[ i ].Count + give[ ig ].Count ) );
        ++ig;
      }
      else {
        while ( ig < give.size() && data[ i ].Word > give[ ig ].Word ) {
          temp.push_back( give[ ig ] );
          ++ig;
        }
        if ( ig < give.size() && data[ i ].Word == give[ ig ].Word ) {
          temp.push_back( make_pair( data[ i ].Word, data[ i ].Count + give[ ig ].Count ) );
          ++ig;
        }
        else  
          temp.push_back( data[ i ] );
      }
    }
    for ( ; ig < give.size() ; ++ig ) 
      temp.push_back( give[ ig ] );
    data = temp;
  }
}

vector< pair< int, prob > > transform( WordsDistrib& data ) {
  vector< pair< int, prob > > temp;
  long long num = 0;
  for ( size_t i = 0  ; i < data.size() ; ++i )
    num += data[ i ].Count;
  for ( size_t i = 0 ; i <  data.size() ; ++i ) 
    temp.push_back( make_pair( data[ i ].Word, prob( ( double ) data[ i ].Count / num ) ) );
  return temp;
}

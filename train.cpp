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

char outputFile[ ][ 21 ] = { "models/model_00.mod",
                             "models/model_01.mod",
                             "models/model_02.mod",
                             "models/model_03.mod",
                             "models/model_04.mod",
                             "models/model_05.mod",
                             "models/model_06.mod",
                             "models/model_07.mod",
                             "models/model_08.mod",
                             "models/model_09.mod",
                             "models/model_10.mod",
                             "models/model_11.mod",
                             "models/model_12.mod",
                             "models/model_13.mod",
                             "models/model_14.mod",
                             "models/model_15.mod",
                             "models/model_16.mod",
                             "models/model_17.mod",
                             "models/model_18.mod",
                             "models/model_19.mod" };

int main() {
  FILE* fin = fopen( "filelist_train/filelist.list", "r" );

  Model m;
  int index = 0;
  char list[ 100 ];
  while ( fscanf( fin, "%s", list ) != EOF ) { 
    initModel( m, list );
    m.write( outputFile[ index++ ] );
  }

  fclose( fin );
}

void initModel( Model& m, const char* list ) {
  cout << list << endl;
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
    for ( size_t i = 0, ig = 0  ; i < data.size() ; ++i ) {
      if ( ig >= give.size() || data[ i ].Word < give[ ig ].Word )
        temp.push_back( make_pair( data[ i ].Word, data[ i ].Count ) );
      else if ( data[ i ].Word == give[ ig ].Word ) { 
        temp.push_back( make_pair( data[ i ].Word, data[ i ].Count + give[ ig ].Count ) );
        ++ig;
      }
      else {
        temp.push_back( make_pair( give[ ig ].Word, give[ ig ].Count ) );
        ++ig;
      }
    }
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

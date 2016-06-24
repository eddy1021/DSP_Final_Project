#include "data.h"
#include "model.h"
#include "prob.h"
#include <string>
#include <vector>
#include <utility>
#include <stdio.h>
#include <iostream>
using namespace std;


void init();
vector< char* > testModel( const char* fileName );
vector< pair< int, double > > transform( WordsDistrib& );
double calProb( vector< pair< int, double > >&, int );
void output();

Model m[ 21 ];
char inputFile[ ][ 21 ] = {  "models/model_00.mod",
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
                             "models/model_19.mod",
                             "models/model_20.mod" };
char modelName[ ][ 21 ] = { "model_00",
                            "model_01",
                            "model_02",
                            "model_03",
                            "model_04",
                            "model_05",
                            "model_06",
                            "model_07",
                            "model_08",
                            "model_09",
                            "model_10",
                            "model_11",
                            "model_12",
                            "model_13",
                            "model_14",
                            "model_15",
                            "model_16",
                            "model_17",
                            "model_18",
                            "model_19",
                            "model_20" };
vector< string > ansF;
vector< vector< char* > > ansM;

int main() {
  init();
  FILE* fin1 = fopen( "filelist_test/filelist.list", "r" );

  char list[ 100 ];
  while ( fscanf( fin1, "%s", list ) != EOF ) { 
    FILE* fin2 = fopen( list, "r" );

    char path[ 100 ];
    while ( fscanf( fin2, "%s", path ) != EOF ) {
      ansF.push_back( string( path ) );
      ansM.push_back( testModel( path ) );
    }

    fclose( fin2 );
  }

  output();
  fclose( fin1 );
}

void init() {
  for ( int i = 0 ; i < 21 ; ++i )
    m[ i ].load( inputFile[ i ] );
}

vector< char* > testModel( const char* fileName ) {
  WordsDistrib tdb = read( fileName );
  vector< pair< int, double > > temp = transform( tdb );
  vector< pair< double, char* > > ta;
  for ( int i = 0 ; i < 21 ; ++i ) 
    ta.push_back( make_pair( calProb( temp, i ), modelName[ i ] ) );
  
  sort( ta.begin(), ta.end() );
  vector< char* > ans;
  for ( size_t i = 0 ; i < 3 ; ++i )  
    ans.push_back( ta[ i ].second );
  return ans;
}

vector< pair< int, double > > transform( WordsDistrib& data ) {
  vector< pair< int, double > > temp;
  long long num = 0;
  for ( size_t i = 0  ; i < data.size() ; ++i )
    num += data[ i ].Count;
  for ( size_t i = 0 ; i <  data.size() ; ++i ) 
    temp.push_back( make_pair( data[ i ].Word, ( double ) data[ i ].Count / num ) );
  return temp;
}

double calProb( vector< pair< int, double > >& data, int index ) {
  Model cm = m[ index ];
  double ans = 0;
  for ( size_t i = 0, im = 0 ; i < data.size() ; ++i ) {
    while ( ( im + 1 ) < cm.para.size() && 
            data[ i ].Word > cm.para[ im ].Word )
      ++im;
    if ( im >= cm.para.size() || data[ i ].Word < cm.para[ im ].Word )
      ans = ans + data[ i ].second * data[ i ].second;
    else if ( data[ i ].Word == cm.para[ im ].Word ) {
      ans = ans + ( data[ i ].second - cm.para[ im ].second.value() ) *
                  ( data[ i ].second - cm.para[ im ].second.value() );
      ++im;
    }
    else {
      ans = ans + data[ i ].second * data[ i ].second;
      ++im;
    }
  }
  return ans;
}

void output() {
  FILE* fout = fopen( "analysis.out", "w" );

  for ( size_t i = 0 ; i < ansF.size() ; ++i ) {
    fprintf( fout, "%s ", ansF[ i ].c_str() );
    for ( size_t j = 0 ; j < ansM[ i ].size() ; ++j ) 
      fprintf( fout, "%s ", ansM[ i ][ j ] );
    fprintf( fout, "\n" );
  }

  fclose( fout );
}


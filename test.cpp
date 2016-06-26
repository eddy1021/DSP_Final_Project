#include "data.h"
#include "model.h"
#include "prob.h"
#include <string>
#include <vector>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

#define ModelNum 20
#define TopicNum 3
double Threshold = 0.00001;
int PenaltyOOV = 10;
int lowerbound = 0;
int upperbound = 255;

void init();
bool isGood( char* );
vector< char* > testModel( const char* fileName );
vector< pair< int, double > > transform( WordsDistrib& );
double calProb( vector< pair< int, double > >&, int );
double cost( double, double );
void output();
void acc();

Model m[ ModelNum ];
char modelName[ ][ 20 ] = { "atheism",
                            "autos",
                            "baseball",
                            "christian",
                            "crypt",
                            "eletronics",
                            "forsale",
                            "graphics",
                            "guns",
                            "hockey",
                            "ibm.pc.hardware",
                            "mac.hardware",
                            "med",
                            "mideast",
                            "motorcycles",
                            "ms-windows.misc",
                            "politics",
                            "religion",
                            "space",
                            "windows.x" };
vector< string > ansF;
vector< vector< char* > > ansM;

char USAGE[] = "USAGE: ./test -default\n       ./test [ -penalty #NUM ] [ -threshold #NUM ] [ -wordLen #lowerbound #upperbound ]";

inline void Exit( const char* msg = USAGE ){
  cerr << msg << endl;
  exit( 0 );
}

inline bool isN( const char* token ){
  int len = strlen( token );
  for( int i = 0 ; i < len ; i ++ )
    if( !isdigit( token[ i ] ) )
      return false;
  return true;
}

int main( int argc, char* argv[ ] ) {
  if( argc == 1 ) Exit();
  for( int i = 1 ; i < argc ; ){
    if( strcmp( argv[ i ] , "-penalty" ) == 0 ){
      if( i + 1 >= argc || !isN( argv[ i + 1 ] ) )
        Exit( "Expected a positive integer after -penalty" );
      PenaltyOOV = atoi( argv[ i + 1 ] );
      i += 2;
    }else if( strcmp( argv[ i ] , "-threshold" ) == 0 ){
      if( i + 1 >= argc || !isGood( argv[ i + 1 ] ) )
        Exit( "Expected a positive real number after -threshold" );
      Threshold = atof( argv[ i + 1 ] );
      i += 2;
    }else if( strcmp( argv[ i ] , "-wordLen" ) == 0 ){
      if( i + 2 >= argc || !isN( argv[ i + 1 ] ) || !isN( argv[ i + 2 ] ) )
        Exit( "Expected two positive integer after -wordLen" );
      lowerbound = atoi( argv[ i + 1 ] );
      upperbound = atoi( argv[ i + 2 ] );
      if( lowerbound > upperbound )
        Exit( "lowerbound <= upperbound should be satisfied" );
      i += 3;
    }else if( strcmp( argv[ i ] , "-default" ) == 0 && i == 1 && i + 1 == argc )
      break;
    else Exit();
  }

  init();

  clock_t start, end;
  start = clock();
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
  
  end = clock();
  cout << ( double )( end - start ) / CLOCKS_PER_SEC << "(s)/";
  acc();
}

void init() {
  char fname[ 100 ];
  for ( int i = 0 ; i < ModelNum ; ++i ) {
    sprintf( fname, "models/model_%02d.mod", i );
    m[ i ].load( fname );
  }
}

bool isGood( char* c ) {
  int len = strlen( c );
  for ( int i = 0 ; i < len ; ++i )
    if ( ( c[ i ] < '0' || c[ i ] > '9' ) && c[ i ] != '.' )
      return false;
  return true;
}

vector< char* > testModel( const char* fileName ) {
  WordsDistrib tdb = read( fileName , lowerbound , upperbound );
  vector< pair< int, double > > temp = transform( tdb );
  vector< pair< double, char* > > ta;
  for ( int i = 0 ; i < ModelNum ; ++i ) 
    ta.push_back( make_pair( calProb( temp, i ), modelName[ i ] ) );
  
  sort( ta.begin(), ta.end() );
  vector< char* > ans;
  for ( size_t i = 0 ; i < TopicNum ; ++i )  
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
  Model& cm = m[ index ];
  double ans = 0;
  size_t im = 0;
  for ( size_t i = 0 ; i < data.size() ; ++i ) {
    if ( im >= cm.para.size() || data[ i ].Word < cm.para[ im ].Word )
      ans += cost( data[ i ].second, 0 ) * PenaltyOOV;
    else if ( data[ i ].Word == cm.para[ im ].Word ) {
      ans += cost( data[ i ].second, cm.para[ im ].second.value() );
      ++im;
    }
    else {
      while ( im < cm.para.size() && 
              data[ i ].Word > cm.para[ im ].Word ) { 
        if ( cm.para[ im ].second.value() > Threshold )
          ans += cost( cm.para[ im ].second.value(), 0 ) * PenaltyOOV;
        else 
          ans += cost( cm.para[ im ].second.value(), 0 );
        ++im;
      }
      if ( im < cm.para.size() && data[ i ].Word == cm.para[ im ].Word ) {
        ans += cost( data[ i ].second, cm.para[ im ].second.value() );
        ++im;
      }
      else  
        ans += cost( data[ i ].second, 0 ) * PenaltyOOV;
    }
  }
  for ( ; im < cm.para.size() ; ++im )
    ans += cost( cm.para[ im ].second.value(), 0 ) * PenaltyOOV;
  return ans;
}

double cost( double a, double b ) {
  return ( a - b ) * ( a - b );
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

void acc() {
  ifstream ifs( "analysis.out" );
  string temp, target;
  double count = 0, success = 0;
  while ( getline( ifs, target, ' ' ) ) {
    ++count;
    for ( int i = 0 ; i < 3 ; ++i ) {
      getline( ifs, temp, ' ' );
      if ( strstr( target.c_str(), temp.c_str() ) != NULL ) {
        ++success;
      }
    }
  }

  cout << success / count * 100 << "%" << endl;
}

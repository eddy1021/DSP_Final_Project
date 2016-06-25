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

int main( int argc, char* argv[ ] ) {
  init();
  if ( argc != 1 ) {
    if( strcmp( "-penalty" , argv[ 1 ] ) ) {
      puts( "USAGE: ./test [ -penalty #NUM ] [ -threshold #NUM ]" ); 
      exit(0);
    }
    if ( argc >= 3 && !isGood( argv[ 2 ] ) ) {
      printf( "%s is not a number!\n", argv[ 2 ] );
      exit(0);
    }
    else 
      PenaltyOOV = atoi( argv[ 2 ] );
    if ( argc >= 5 && strcmp( "-threshold", argv[ 3 ] ) ) {
      puts( "USAGE: ./test [ -penalty #NUM ] [ -threshold #NUM ]" ); 
      exit(0);
    }
    if ( argc >= 5 && !isGood( argv[ 4 ] ) ) {
      printf( "%s is not a number!\n", argv[ 4 ] );
      exit(0);
    }
    else 
      Threshold = atof( argv[ 4 ] );
  }

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
  WordsDistrib tdb = read( fileName );
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

#ifndef _MODEL_H_
#define _MODEL_H_

#include "prob.h"
#include <stdio.h>
#include <vector>
#include <utility>

using namespace std;

class Model{
public:
  vector< pair< int, prob > > para;
#define Word first
#define Prob second
  bool loaded;
  Model(){ loaded = false; }
  Model( const char* filename ){
    loaded = load( filename );
  }
  inline bool isLoad(){ return loaded; }
  inline bool load( const char* filename ){
    FILE* fin = fopen( filename , "r" );
    if( !fin ) return loaded = false;
    
    para.clear();
    int _hsh;
    double _vl;
    while( fscanf( fin , "%d%lf" , &_hsh , &_vl ) == 2 )
      para.push_back( { _hsh , prob( _vl ) } );

    fclose( fin );
    return loaded = true;
  }
  inline bool write( const char* filename ){
    FILE* fout = fopen( filename , "w" );
    if( !fout ) return false;

    for( size_t i = 0 ; i < para.size() ; i ++ )
      fprintf( fout , "%d %.6f\n" , para[ i ].Word , para[ i ].Prob.value() );

    fclose( fout );
    return true;
  }
};

#endif

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
using namespace std;

int main() {
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

#ifndef _PROB_H_
#define _PROB_H_

const double unit = 1e4;

// treated as normal float number:
class prob{
public:
  double _d;
  prob(           ){ _d =  0; }
  prob( int    _x ){ _d = _x * unit; }
  prob( double _x ){ _d = _x * unit; }
  inline prob operator+( const prob& tprob ){
    return prob( _d + tprob._d );
  }
  inline prob operator-( const prob& tprob ){
    return prob( _d - tprob._d );
  }
  inline prob operator*( const prob& tprob ){
    return prob( _d * tprob._d / unit );
  }
  inline prob operator/( const prob& tprob ){
    return prob( _d / tprob._d * unit );
  }
  bool operator<( const prob& tprob ){
    return _d < tprob._d;
  }
  inline double value() const{
    return _d / unit;
  }
};

#endif

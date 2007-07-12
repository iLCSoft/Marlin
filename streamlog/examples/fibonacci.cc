#include "fibonacci.h"

namespace streamlog_test{

  fibonacci::fibonacci(unsigned n) :_n(n) {
    _f.resize(_n) ;
    
    if(_n > 0) { 
      
      _f[0] = 0 ;
      
      if(_n > 1 ) {
	
	_f[1] = 1 ;
	
	for(unsigned i=2; i<_n; ++i){
	  
	  _f[i] = _f[i-1] + _f[i-2] ;
	}
      }
    }
  }
  
  unsigned fibonacci::sum() {
    unsigned sum =0 ;
    for(unsigned i=0; i<_n; ++i){
      sum += _f[i] ;
    }
    return sum ;
  }

}

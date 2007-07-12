#ifndef fibonacci_h
#define fibonacci_h

#include <vector>

namespace streamlog_test{

  class fibonacci{
  private:
    unsigned _n ;
    std::vector<unsigned> _f ;
    
  public:
    explicit fibonacci(unsigned n) ; 

    unsigned sum() ;
    
  };


}
#endif

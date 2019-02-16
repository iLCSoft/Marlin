#ifndef MARLIN_CONCURRENCY_INTERNAL_h
#define MARLIN_CONCURRENCY_INTERNAL_h 1

#include <map>
#include <string>

namespace marlin {
  
  namespace concurrency {
    
    class StringUtil {
      template <template T>
      static std::string toString( const T &var ) {
        std::ostringstream oss;
        if ((oss << t).fail()) {
            throw marlin::Exception();
        }
        return oss.str();
      }
      
      template <template T>
      static std::string toString( const T &var ) {
        
      }
    }
    

    
    
  } // end namespace concurrency 

} // end namespace marlin 

#endif

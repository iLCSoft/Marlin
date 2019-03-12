#ifndef MARLIN_CONCURRENCY_INTERNAL_h
#define MARLIN_CONCURRENCY_INTERNAL_h 1

// -- std headers
#include <map>
#include <string>
#include <typeinfo>

// -- marlin headers
#include "marlin/Exceptions.h"

namespace EVENT {
  class LCRunHeader ;
  class LCEvent ;
}

namespace marlin {

  namespace concurrency {

    using Exception = lcio::Exception ;

    /**
     *  @brief  HashHelper class
     *  Helper class to generate hash 64 id
     */
    class HashHelper {
    private:
      /// Init value for hash 64
      static const unsigned long long int hashinit = 14695981039346656037ull ;

    public:
      static constexpr unsigned long long int doByte(unsigned long long int hash, unsigned char val) {
        return (hash ^ val) * 1099511628211ull ;
      }

      /**
       *  @brief  Generate a hash 64 from a string
       *
       *  @param  key the input string key
       */
      static unsigned long long int hash64( const char *key ) {
        unsigned char* str = (unsigned char*)key ;
        unsigned long long int hash = hashinit ;
        for ( ; *str; ++str) hash = doByte(hash, *str) ;
        return hash ;
      }

      /**
       *  @brief  Generate a hash 64 from the typeid name
       */
      template <typename T>
      static unsigned long long int typeHash64() {
        static unsigned long long int code = hash64(typeid(T).name()) ;
        return code ;
      }
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  StringUtil class
     *          Simple utility class for string operations
     */
    class StringUtil {
    public:
      /**
       *  @brief  Convert a type to string
       *
       *  @param  var the value to convert
       */
      template <typename T>
      static std::string typeToString( const T &var ) ;

      /**
       *  @brief  Convert a variable to string
       *
       *  @param  str the string to convert
       */
      template <typename T>
      static T stringToType( const std::string &str ) ;

      template <typename T>
      static std::vector<T> split(const std::string &inputString, const std::string &delimiter = " ") ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline std::string StringUtil::typeToString( const T &var ) {
      std::ostringstream oss ;
      if ((oss << var).fail()) {
        throw Exception( "Couldn't convert value to string" ) ;
      }
      return oss.str() ;
    }

    //--------------------------------------------------------------------------

    template <>
    inline std::string StringUtil::typeToString( const std::string &var ) {
      return var ;
    }

    //--------------------------------------------------------------------------

    template <>
    inline std::string StringUtil::typeToString( const bool &var ) {
      return (var ? "true" : "false") ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T StringUtil::stringToType( const std::string &str ) {
      T t ;
      std::istringstream iss(str) ;
      if ((iss >> t).fail()) {
        throw Exception( "Couldn't convert string to value" ) ;
      }
      return t ;
    }

    //--------------------------------------------------------------------------

    template <>
    inline std::string StringUtil::stringToType( const std::string &str ) {
      return str;
    }

    //--------------------------------------------------------------------------

    template <>
    inline bool StringUtil::stringToType( const std::string &str ) {
      if ( str == "true"  || str == "1" || str == "on"  ) return true ;
      if ( str == "false" || str == "0" || str == "off" ) return false ;
      throw Exception( "Couldn't convert value to string" ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> StringUtil::split(const std::string &inputString, const std::string &delimiter) {
      std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
      std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);
      typename std::vector<T> tokens ;
      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        T value = stringToType<T>(inputString.substr(lastPos, pos - lastPos)) ;
        tokens.push_back(value) ;
        lastPos = inputString.find_first_not_of(delimiter, pos) ;
        pos = inputString.find_first_of(delimiter, lastPos) ;
      }
      return tokens ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

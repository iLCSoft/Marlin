#ifndef MARLIN_UTILS_h
#define MARLIN_UTILS_h 1

// -- std headers
#include <map>
#include <string>
#include <sstream>
#include <typeinfo>

// -- marlin headers
#include <marlin/Exceptions.h>

namespace marlin {

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
     *  @brief  Convert a vector of type to vector of string
     *
     *  @param  vars the values to convert
     */
    template <typename T>
    static std::vector<std::string> typeToString( const std::vector<T> &vars ) ;

    /**
     *  @brief  Convert a variable to string
     *
     *  @param  str the string to convert
     */
    template <typename T>
    static T stringToType( const std::string &str ) ;

    /**
     *  @brief  Convert a string vector to a vector of specified type
     *
     *  @param  strs the string vector to convert
     */
    template <typename T>
    static std::vector<T> stringToType( const std::vector<std::string> &strs ) ;

    /**
     *  @brief  Split the string with the corresponding delimiter
     *  
     *  @param  inputString the input string to split
     *  @param  delimiter the string delimiter
     */
    template <typename T>
    static std::vector<T> split( const std::string &inputString, const std::string &delimiter = " " ) ;
    
    /**
     *  @brief  Weird overload for scalar types.
     *  Just returns the conversion to string.
     *  See overload version with vector instead.
     *  
     *  @param  input the input value 
     *  @param  delimiter the string delimiter (unused here)
     */
    template <typename T>
    static std::string join( const T &input, const std::string &delimiter = " " ) ;
    
    /**
     *  @brief  Join the input values from the vector with the corresponding delimiter
     *  and returns a string representation of it.
     *  
     *  @param  input the input vector or values to join
     *  @param  delimiter the string delimiter
     */
    template <typename T>
    static std::string join( const std::vector<T> &input, const std::string &delimiter = " " ) ;
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
  inline std::vector<std::string> StringUtil::typeToString( const std::vector<T> &vars ) {
    std::vector<std::string> result ;
    result.reserve( vars.size() ) ;
    for( auto var : vars ) {
      result.push_back( typeToString<T>( var ) ) ;
    }
    return result ;
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
  inline std::vector<T> StringUtil::stringToType( const std::vector<std::string> &strs ) {
    std::vector<T> result ;
    result.reserve( strs.size() ) ;
    for( auto str : strs ) {
      result.push_back( stringToType<T>( str ) ) ;
    }
    return result ;
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
  
  //--------------------------------------------------------------------------
  
  // weird overloading function that just converts the value to string ...
  template <typename T>
  inline std::string StringUtil::join( const T &input, const std::string &/*delimiter*/ ) {
    return typeToString( input ) ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline std::string StringUtil::join( const std::vector<T> &input, const std::string &delimiter ) {
    std::stringstream ss ;
    for( auto iter = input.begin() ; iter != input.end() ; ++iter ) {
      ss << typeToString( *iter ) ;
      if( std::next(iter) != input.end() ) {
        ss << delimiter ;
      }
    }
    return ss.str() ;
  }

} // end namespace marlin

#endif

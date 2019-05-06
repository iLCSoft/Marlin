#ifndef MARLIN_STRINGPARAMETERS_h
#define MARLIN_STRINGPARAMETERS_h 1

// -- std headers
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <iostream>

// -- marlin headers
#include <marlin/Utils.h>

namespace marlin {

  /** Simple parameters class for Marlin.
   *  Holds named parameters as string vectors.
   *  @author F. Gaede, DESY
   *  @author R. Ete, DESY
   *  @version $Id: StringParameters.h,v 1.5 2006-11-10 11:56:07 engels Exp $
   */
  class StringParameters {
    typedef std::map<std::string, std::vector<std::string>> ParametersMap ;
    friend std::ostream& operator<< ( std::ostream& , const StringParameters& ) ;

  public:
    StringParameters() = default ;
    ~StringParameters() = default ;
    StringParameters( const StringParameters &sp ) = default ;

    /**
     *  @brief  Add a parameter without value.
     *  Does nothing if the parameter exists
     *
     *  @param  key the prameter key
     */
    void add( const std::string& key ) ;

    /**
     *  @brief  Add a parameter with its value. Note that the value is
     *  appended to the parameter list. To overwrite a parameter value,
     *  use replace() instead
     *
     *  @param  key the parameter key
     *  @param  value the parameter value
     */
    template <typename T>
    void add( const std::string& key, const T &value ) ;

    /**
     *  @brief  Add a parameter with its values. Note that the values are
     *  appended to the parameter list. To overwrite a parameter value,
     *  use replace() instead
     *
     *  @param  key the parameter key
     *  @param  values the parameter values
     */
    template <typename T>
    void add( const std::string& key, const std::vector<T> &values ) ;

    /**
     *  @brief  Replace a parameter with a new value.
     *
     *  @param  key the parameter key
     *  @param  value the parameter value
     */
    template <typename T>
    void replace( const std::string& key, const T &value ) ;

    /**
     *  @brief  Replace a parameter with a new values.
     *
     *  @param  key the parameter key
     *  @param  values the parameter values
     */
    template <typename T>
    void replace( const std::string& key, const std::vector<T> &values ) ;

    /**
     *  @brief  Erase a parameter
     *
     *  @param  key the parameter key
     */
    void erase( const std::string& key ) ;

    /**
     *  @brief  Whether the parameter is set, meaning the entry already exists,
     *  but has no value. To check if a parameter exists, use exists() instead
     *
     *  @param  key the parameter key
     */
    bool isParameterSet( const std::string& key ) const ;

    /**
     *  @brief  Whether the parameter exists.
     *
     *  @param  key the parameter key
     */
    bool exists( const std::string& key ) const ;

    /**
     *  @brief  Unset a parameter
     *
     *  @param  key the parameter key
     */
    void unset( const std::string &key ) ;

    /**
     *  @brief  Unset all parameters
     */
    void unset() ;

    /**
     *  @brief  Get the parameter keys
     */
    std::vector<std::string> keys() const ;

    /**
     *  @brief  Get a parameter value. If the value doesn't exists, an
     *  exception is thrown.
     *
     *  @param  key the parameter key
     */
    template <typename T>
    T getValue( const std::string& key ) const ;

    /**
     *  @brief  Get parameter values. If the values do not exists, an
     *  exception is thrown.
     *
     *  @param  key the parameter key
     */
    template <typename T>
    std::vector<T> getValues( const std::string& key ) const ;

    /**
     *  @brief  Get a parameter value. If the value doesn't exists, the fallback
     *  value provided as second argument is returned
     *
     *  @param  key the parameter key
     *  @param  fallback the fallback value if not found
     */
    template <typename T>
    T getValue( const std::string& key , const T &fallback ) const ;

    /**
     *  @brief  Get parameter values. If the values do not exists, the fallback
     *  values provided as second argument is returned
     *
     *  @param  key the parameter key
     *  @param  fallback the fallback value if not found
     */
    template <typename T>
    std::vector<T> getValues( const std::string& key , const std::vector<T> &fallback ) const ;

    /**
     *  @brief  Get a parameter value by reference
     *
     *  @param  key the parameter key
     *  @param  value the value to receive
     */
    template <typename T>
    void get( const std::string& key , T &value ) const ;

    /**
     *  @brief  Get parameter values by reference
     *
     *  @param  key the parameter key
     *  @param  values the values to receive
     */
    template <typename T>
    void get( const std::string& key , std::vector<T> &values ) const ;

  protected:
    ///< The parameters map
    ParametersMap          _map {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  std::ostream& operator<< (  std::ostream& s, const StringParameters& p ) ;

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::add( const std::string& key, const T &value ) {
    add( key ) ;
    auto iter = _map.find( key ) ;
    iter->second.push_back( StringUtil::typeToString<T>( value ) )  ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::add( const std::string& key, const std::vector<T> &values ) {
    add( key ) ;
    auto iter = _map.find( key ) ;
    for( auto val : values ) {
      iter->second.push_back( StringUtil::typeToString<T>(val) ) ;
    }
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::replace( const std::string& key, const T &value ) {
    erase( key ) ;
    add( key, value ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::replace( const std::string& key, const std::vector<T> &values ) {
    erase( key ) ;
    add( key , values ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline T StringParameters::getValue( const std::string& key ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      throw Exception( "StringParameters::getAs: parameter '" + key + "' not found" ) ;
    }
    return StringUtil::stringToType<T>( iter->second.at(0) ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::vector<T> StringParameters::getValues( const std::string& key ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      throw Exception( "StringParameters::getAs: parameter '" + key + "' not found" ) ;
    }
    return StringUtil::stringToType<T>( iter->second ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline T StringParameters::getValue( const std::string& key , const T &fallback ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      return fallback ;
    }
    return StringUtil::stringToType<T>( iter->second.at(0) ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::vector<T> StringParameters::getValues( const std::string& key , const std::vector<T> &fallback ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      return fallback ;
    }
    return StringUtil::stringToType<T>( iter->second ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::get( const std::string& key , T &value ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      return ;
    }
    value = StringUtil::stringToType<T>( iter->second.at(0) ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void StringParameters::get( const std::string& key , std::vector<T> &values ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      return ;
    }
    values = StringUtil::stringToType<T>( iter->second ) ;
  }

} // end namespace marlin
#endif

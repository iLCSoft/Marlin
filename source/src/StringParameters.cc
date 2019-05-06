#include <marlin/StringParameters.h>

// -- std headers
#include <sstream>
#include <exception>

namespace marlin {

  void StringParameters::add( const std::string& key ) {
    _map.insert( ParametersMap::value_type(key, ParametersMap::mapped_type()) ) ;
  }

  //--------------------------------------------------------------------------

  void StringParameters::erase( const std::string& key ) {
    _map.erase( key );
  }

  //--------------------------------------------------------------------------

  bool StringParameters::isParameterSet(  const std::string& key ) const {
    auto iter = _map.find( key ) ;
    if( _map.end() == iter ) {
      return false ;
    }
    return (not iter->second.empty() ) ;
  }

  //--------------------------------------------------------------------------

  bool StringParameters::exists( const std::string& key ) const {
    return (_map.find( key ) != _map.end() ) ;
  }

  //--------------------------------------------------------------------------

  void StringParameters::unset( const std::string &key ) {
    auto iter = _map.find( key ) ;
    if( _map.end() != iter ) {
      iter->second.clear() ;
    }
  }

  //--------------------------------------------------------------------------

  void StringParameters::unset() {
    for( auto iter : _map ) {
      iter.second.clear() ;
    }
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> StringParameters::keys() const {
    std::vector<std::string> result ;
    result.reserve( _map.size() ) ;
    for( auto m : _map ) {
      result.push_back( m.first );
    }
    return result ;
  }

  //--------------------------------------------------------------------------

  std::ostream& operator<< (  std::ostream& s,  const StringParameters& p ) {
    for( auto m : p._map ) {
      s << "      " << m.first << ": " ;
      for( auto val : m.second ) {
        s << "[" << val << "]  " ;
      }
      s << std::endl ;
    }
    return s ;
  }

} //namsepace marlin

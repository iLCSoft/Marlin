#ifndef MARLIN_CONCURRENCY_PARAMETERS_h
#define MARLIN_CONCURRENCY_PARAMETERS_h 1

#include <map>
#include <string>

namespace marlin {
  
  namespace concurrency {
    
    class Parameters {
    public:
      typedef std::map< std::string, std::vector<std::string>>  map_type ;

    public:
      template <typename T>
      void set( const std::string &name , const T & values );
      
      template <typename T>
      T parameterAs( const std::string &name ) const ;
      
      template <typename T>
      std::vector<T> parametersAs( const std::string &name ) const ;
      
      bool parameterExists( const std::string &name ) const ;
      
      void clear() ;
      
      void count() const ;
      
    private:
      /// Parameter map
      map_type         _parameters {} ;
    };
    
    
    template <typename T>
    inline void Parameters::set( const std::string &name , const T & value ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        iter = _parameters.insert( map_type::value_type( name, {} ) ) ;
      }
      iter->second.clear() ;
      iter->second.push_back( std::to_string( value ) );
    }
    
    template <typename T>
    T parameterAs( const std::string &name ) const ;
    
    template <typename T>
    std::vector<T> parametersAs( const std::string &name ) const ;
    
    bool parameterExists( const std::string &name ) const ;
    
    void clear() ;
    
    void count() const ;
    
    
  } // end namespace concurrency 

} // end namespace marlin 

#endif

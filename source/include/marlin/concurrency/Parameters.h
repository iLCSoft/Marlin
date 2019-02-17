#ifndef MARLIN_CONCURRENCY_PARAMETERS_h
#define MARLIN_CONCURRENCY_PARAMETERS_h 1

#include <map>
#include <string>
#include <typeindex>
#include <algorithm>

#include <marlin/concurrency/Internal.h>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Parameter class.
     *  Hold a parameter as string vector.
     */
    class Parameter {
    public:
      typedef std::vector<std::string> Container;
    public:
      /**
       *  @brief  Default constructor
       */
      Parameter() ;

      /**
       *  @brief  Constructor with value
       */
      template <typename T>
      Parameter( const T &value ) ;

      /**
       *  @brief  Constructor with values
       *
       *  @param  values the parameter values
       */
      template <typename T>
      Parameter( const std::vector<T> &values ) ;

      /**
       *  @brief  Copy constructor
       *
       *  @param  parameter parameter to copy
       */
      Parameter( const Parameter &parameter ) ;

      /**
       *  @brief  Assignment operator
       *
       *  @param  rhs the parameter to assign
       */
      Parameter& operator=(const Parameter &rhs) ;

      /**
       *  @brief  Assignment operator
       *
       *  @param  rhs a value to assign
       */
      template <typename T>
      Parameter& operator=(const T &rhs) ;

      /**
       *  @brief  Assignment operator
       *
       *  @param  rhs values to assign
       */
      template <typename T>
      Parameter& operator=(const std::vector<T> &rhs) ;

      /**
       *  @brief  Set the parameter value
       *
       *  @param  value the value to set
       */
      template <typename T>
      void set( const T &value ) ;

      /**
       *  @brief  Set the parameter values
       *
       *  @param  values the values to set
       */
      template <typename T>
      void set( const std::vector<T> &values ) ;

      /**
       *  @brief  Add the parameter value
       *
       *  @param  value the value to add
       */
      template <typename T>
      void add( const T &value ) ;

      /**
       *  @brief  Add the parameter values
       *
       *  @param  values the values to add
       */
      template <typename T>
      void add( const std::vector<T> &values ) ;

      /**
       *  @brief  Get the parameter value
       */
      template <typename T>
      T get() const ;

      /**
       *  @brief  Get the parameter value.
       *
       *  @param  fallback returned value if not initialized
       */
      template <typename T>
      T get( const T &fallback ) const ;

      /**
       *  @brief  Get the parameter value
       */
      template <typename T>
      std::vector<T> getVector() const ;

      /**
       *  @brief  Get the parameter value
       *
       *  @param  fallback returned value if not initialized
       */
      template <typename T>
      std::vector<T> getVector( const std::vector<T> &fallback ) const ;

      /**
       *  @brief  Set the parameter description
       *
       *  @param  desc the parameter description
       */
      void setDescription( const std::string &desc ) ;

      /**
       *  @brief  Get the parameter description
       */
      const std::string &description() const ;

      /**
       *  @brief  Get the underlying parameter type
       */
      const std::type_index &type() const ;

      /**
       *  @brief  Whether the parameter has been initialized
       */
      bool isInitialized() const ;

      /**
       *  @brief Reset the parameter
       */
      void reset() ;

      /**
       *  @brief  Print the parameter in the stream
       *
       *  @param  os the output stream
       */
      void print(std::ostream &os = std::cout) ;

    private:
      /// The parameter container
      Container        _parameter {} ;
      /// Whether the parameter has been initialized
      bool             _initialized {false} ;
      /// The underlying parameter type info
      std::type_index  _type ;
      /// The parameter description
      std::string      _description {} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  Parameters class.
     *          Hold parameters as string or string list
     *          and provide conversion functions
     */
    class Parameters {
    public:
      typedef std::map< std::string, Parameter >  map_type ;

    public:
      /**
       *  @brief  Create a new un-initialized parameter
       *  If the parameter exists, it is overwritten
       *
       *  @param  name the parameter name
       */
      Parameter &create( const std::string &name ) ;

      /**
       *  @brief  Set a parameter value. Append if doesn't exists or replace it.
       *
       *  @param name  the parameter name
       *  @param value the parameter value
       */
      template <typename T>
      Parameter &set( const std::string &name , const T & value );

      /**
       *  @brief  Set parameter values. Append if doesn't exists or replace it.
       *
       *  @param  name  the parameter name
       *  @param  values the values to set
       */
      template <typename T>
      Parameter &set( const std::string &name , const std::vector<T> &values );

      /**
       *  @brief  Add a parameter value. Append to exisiting, else set it.
       *
       *  @param name  the parameter name
       *  @param value the parameter value
       */
      template <typename T>
      Parameter &add( const std::string &name , const T & value );

      /**
       *  @brief  Add parameter values. Append to exisiting, else set it.
       *
       *  @param  name the parameter name
       *  @param  values the values to add
       */
      template <typename T>
      Parameter &add( const std::string &name , const std::vector<T> &values );

      /**
       *  @brief  Get a parameter value (single) as specified type.
       *          Throw an exception if not found
       *
       *  @param  name the parameter name
       */
      template <typename T>
      T get( const std::string &name ) const ;

      /**
       *  @brief  Get a parameter value (single) as specified type.
       *          Return the fallback value if not found
       *
       *  @param  name the parameter name
       *  @param  fallback the fallback value if not found
       */
      template <typename T>
      T get( const std::string &name , const T &fallback ) const ;

      /**
       *  @brief  Get the parameter values (vector) as specified type.
       *          Throw an exception if not found
       *
       *  @param  name the parameter name
       */
      template <typename T>
      std::vector<T> get( const std::string &name ) const ;

      /**
       *  @brief  Get the parameter values (vector) as specified type.
       *          Return the fallback value if not found
       *
       *  @param  name the parameter name
       *  @param  fallback the fallback value if not found
       */
      template <typename T>
      std::vector<T> get( const std::string &name , const std::vector<T> &fallback ) const ;

      /**
       *  @brief  Get the parameter names
       */
      std::vector<std::string> names() const ;

      /**
       *  @brief  Remove a parameter
       *
       *  @param name the parameter name
       */
      void remove( const std::string &name ) ;

      /**
       *  @brief  Check the existence of a parameter
       *
       *  @param  name the parameter name
       */
      bool exists( const std::string &name ) const ;

      /**
       *  @brief  Check if the parameter exists and initialized
       *
       *  @param  name the parameter name
       */
      bool isInitialized( const std::string &name ) const ;

      /**
       *  @brief  Clear the parameter list
       */
      void clear() ;

      /**
       *  @brief  Get the number of parameters
       */
      unsigned int count() const ;

      /**
       *  @brief  Print parameters in console
       */
      void print() const;

    private:
      /// Parameter map
      map_type         _parameters {} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter::Parameter( const T &value ) :
      _type( typeid(value) ) {
      set( value ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter::Parameter( const std::vector<T> &values ) :
      _type( typeid(values) ) {
      set( values );
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter& Parameter::operator=(const T &rhs) {
      set( rhs );
      return *this;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter& Parameter::operator=(const std::vector<T> &rhs) {
      set( rhs );
      return *this;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Parameter::set( const T &value ) {
      _parameter.resize(1);
      _parameter = StringUtil::typeToString( value ) ;
      _initialized = true ;
      _type = std::type_index( typeid( T ) ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Parameter::set( const std::vector<T> &values ) {
      _parameter.clear() ;
      _parameter.reserve( values.size() ) ;
      auto iter = values.begin() ;
      while ( iter != values.end() ) {
        _parameter.push_back( StringUtil::typeToString( *iter ) );
        iter++;
      }
      _initialized = true ;
      _type = std::type_index( typeid( T ) ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Parameter::add( const T &value ) {
      if ( not isInitialized() ) {
        _parameter.push_back( StringUtil::typeToString( value ) );
        _initialized = true ;
        _type = std::type_index( typeid( T ) ) ;
      }
      else {
        if ( _type != std::type_index( typeid( T ) )
          && _type != std::type_index( typeid( std::vector<T> ) ) ) {
            throw Exception( "Parameter::add: type mismatch" );
        }
        _parameter.push_back( StringUtil::typeToString( value ) );
        _type = std::type_index( typeid( std::vector<T> ) ) ;
      }
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Parameter::add( const std::vector<T> &values ) {
      if ( not isInitialized() ) {
        auto iter = values.begin() ;
        while ( iter != values.end() ) {
          _parameter.push_back( StringUtil::typeToString( *iter ) );
          iter++;
        }
        _initialized = true ;
        _type = std::type_index( typeid( std::vector<T> ) ) ;
      }
      else {
        if ( _type != std::type_index( typeid( T ) )
          && _type != std::type_index( typeid( std::vector<T> ) ) ) {
            throw Exception( "Parameter::add: type mismatch" );
        }
        auto iter = values.begin() ;
        while ( iter != values.end() ) {
          _parameter.push_back( StringUtil::typeToString( *iter ) );
          iter++;
        }
        _type = std::type_index( typeid( std::vector<T> ) ) ;
      }
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T Parameter::get() const {
      if( not isInitialized() ) {
        throw Exception( "Parameter::get: parameter not initialized" ) ;
      }
      return StringUtil::stringToType<T>( _parameter.at(0)) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T Parameter::get( const T &fallback ) const {
      if( not isInitialized() ) {
        return fallback ;
      }
      return StringUtil::stringToType<T>( _parameter.at(0)) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    std::vector<T> Parameter::getVector() const {
      if( not isInitialized() ) {
        throw Exception( "Parameter::getVector: parameter not initialized" ) ;
      }
      typename std::vector<T> vec ;
      std::transform( _parameter.begin(), _parameter.end(), std::back_inserter(vec), StringUtil::stringToType<T> ) ;
      return vec ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> Parameter::getVector( const std::vector<T> &fallback ) const {
      if( not isInitialized() ) {
        return fallback ;
      }
      typename std::vector<T> vec ;
      std::transform( _parameter.begin(), _parameter.end(), std::back_inserter(vec), StringUtil::stringToType<T> ) ;
      return vec ;
    }

    //--------------------------------------------------------------------------

    inline void Parameter::print( std::ostream &os ) {
      if ( not isInitialized() ) {
        return;
      }
      for ( auto str : _parameter ) {
        os << str << " ";
      }
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    inline Parameter &Parameters::create( const std::string &name ) {
      _parameters[name] = Parameter() ;
      return _parameters.find( name )->second ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter &Parameters::set( const std::string &name , const T & value ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        iter = _parameters.insert( map_type::value_type( name, Parameter(value) ) ) ;
        return iter->second ;
      }
      iter->second = value ;
      return iter->second ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter &Parameters::set( const std::string &name , const std::vector<T> &values ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        iter = _parameters.insert( map_type::value_type( name, Parameter(values) ) ) ;
        return iter->second ;
      }
      iter->second = values ;
      return iter->second ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter &Parameters::add( const std::string &name , const T & value ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        iter = _parameters.insert( map_type::value_type( name, {} ) ) ;
        return iter->second ;
      }
      iter->second.add( value );
      return iter->second ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline Parameter &Parameters::add( const std::string &name , const std::vector<T> &values ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        iter = _parameters.insert( map_type::value_type( name, Parameter(values) ) ) ;
        return iter->second ;
      }
      iter->second.add( values );
      return iter->second ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T Parameters::get( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() == iter ) {
        throw Exception( "Parameter '" + name + "' not found!" );
      }
      return iter->second.get<T>();
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T Parameters::get( const std::string &name , const T &fallback ) const {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() == iter ) {
        return fallback;
      }
      return iter->second.get<T>( fallback );
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> Parameters::get( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() == iter ) {
        throw Exception( "Parameter '" + name + "' not found!" );
      }
      return iter->second.getVector<T>();
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> Parameters::get( const std::string &name , const std::vector<T> &fallback ) const {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() == iter ) {
        return fallback;
      }
      return iter->second.getVector<T>( fallback );
    }

    //--------------------------------------------------------------------------

    inline void Parameters::remove( const std::string &name ) {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() != iter ) {
        _parameters.erase( iter ) ;
      }
    }

    //--------------------------------------------------------------------------

    inline std::vector<std::string> Parameters::names() const {
      std::vector<std::string> vec ;
      for ( auto iter: _parameters ) {
        vec.push_back( iter.first ) ;
      }
      return vec ;
    }

    //--------------------------------------------------------------------------

    inline bool Parameters::exists( const std::string &name ) const {
      return ( _parameters.end() != _parameters.find( name ) ) ;
    }

    //--------------------------------------------------------------------------

    inline bool Parameters::isInitialized( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if ( _parameters.end() == iter ) {
        return false ;
      }
      return iter->second.isInitialized();
    }

    //--------------------------------------------------------------------------

    inline void Parameters::clear() {
      _parameters.clear();
    }

    //--------------------------------------------------------------------------

    inline unsigned int Parameters::count() const {
      return _parameters.size();
    }

    //--------------------------------------------------------------------------

    inline void Parameters::print() const {
      for ( auto iter : _parameters ) {
        std::cout << " - " << iter.first << ": ";
        iter.second.print();
        std::cout << std::endl;
      }
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

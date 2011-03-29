#include "marlin/ProcessorLoader.h"

#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include <set>


namespace marlin{


ProcessorLoader::ProcessorLoader(
        lcio::StringVec::const_iterator first, 
        lcio::StringVec::const_iterator last ) {


    _loadError=false;
    lcio::StringVec::const_iterator current = first ;

    std::set<std::string> _checkDuplicateLibs;

    while( current != last ){

        std::string libName( *current ) ;
        size_t idx;
        idx = libName.find_last_of("/");
        // the library basename, i.e. /path/to/libBlah.so --> libBlah.so
        std::string libBaseName( libName.substr( idx + 1 ) );

        char *real_path = realpath(libName.c_str(), NULL);

        if( real_path != NULL ){
            std::cout << "<!-- Loading shared library : " << real_path << " ("<< libBaseName << ")-->" << std::endl ;

            // use real_path
            free(real_path);
        }
        else{
            std::cout << "<!-- Loading shared library : " << libName << " ("<< libBaseName << ")-->" << std::endl ;
        }

        
        if( _checkDuplicateLibs.find( libBaseName ) == _checkDuplicateLibs.end() ){
            _checkDuplicateLibs.insert( libBaseName ) ;
        }
        else{
            std::cout << std::endl << "<!-- ERROR loading shared library : " << libName << std::endl
                << "    ->    Trying to load DUPLICATE library -->" << std::endl << std::endl ;
            _loadError=true;
        }


        if( ! _loadError ){

            //void* libPointer  = dlopen( libName.c_str() , RTLD_NOW) ;
            //void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY ) ;
            //void* libPointer  = dlopen( libName.c_str() , RTLD_NOW | RTLD_GLOBAL) ;
            void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;

            if( libPointer == 0 ){
                std::cout << std::endl << "<!-- ERROR loading shared library : " << libName << std::endl
                          << "    ->    "   << dlerror() << " -->" << std::endl << std::endl ;
                _loadError=true;
            }
            else{
                _libs.push_back( libPointer ) ;
            }
        }

        ++current ;

    }
}


ProcessorLoader::~ProcessorLoader() {


  char * s = std::getenv("MARLIN_DEBUG" ) ;
  
  // do not unload processors if $MARLIN_DEBUG is set to 1
  // useful for debugging with valgrind (https://jira.slac.stanford.edu/browse/MAR-45)
  if( std::string("1").compare( (s?s:"0") ) == 0 ) {
    std::cout << std::endl << "<!-- MARLIN_DEBUG=1 set in your environment - skip unloading processors --> " << std::endl ;
  } else {
    for( LibVec::iterator it = _libs.begin() ; it != _libs.end() ; ++it ) {
        dlclose( *it ) ;
    }
  }
}


} // namespace marlin

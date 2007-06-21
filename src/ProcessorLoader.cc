#include "marlin/ProcessorLoader.h"

#include <dlfcn.h>
#include <iostream>

namespace marlin{

  ProcessorLoader::ProcessorLoader( lcio::StringVec::const_iterator  first, 
				    lcio::StringVec::const_iterator last ){


    lcio::StringVec::const_iterator current = first ;
    
    while( current != last ){
      
      //std::string libName( "lib" + *current + ".so" ) ;
      std::string libName( *current ) ;
      
//       void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY ) ;
//       void* libPointer  = dlopen( libName.c_str() , RTLD_NOW) ;

	 std::cout << "<!-- Loading shared library : " 
		   << libName << " -->" << std::endl ;
       void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;
       //void* libPointer  = dlopen( libName.c_str() , RTLD_NOW | RTLD_GLOBAL) ;

       if( libPointer == 0 ){

	 std::cout  << std::endl 
		 << "<!-- ERROR loading shared library : " 
		 << libName << std::endl << "    ->    "   << dlerror() << " -->" << std::endl ;
	 

       }else{


	 _libs.push_back( libPointer ) ;
       }

      ++current ;
    }
  }


  ProcessorLoader::~ProcessorLoader() {

    for( LibVec::iterator it = _libs.begin() ; it != _libs.end() ; ++it ) {

      dlclose( *it ) ;
    }
  }
} // namespace marlin

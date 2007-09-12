#include <iostream>
#include <fstream>

#include "streamlog/streamlog.h"

#include "fibonacci.h"
#include "ATemplate.h"


int main(int argc, char** argv) {

  std::ofstream* file = 0 ;
  
  std::string binname( argv[0] ) ;
  binname = binname.substr( binname.find_last_of("/") + 1 , binname.size() ) ;

  if( argc > 1 ) {

    file = new std::ofstream(  argv[1]  )  ;

    streamlog::out.init( *file , binname ) ;

    std::cout << "  writing log to file : " << argv[1] << std::endl ;

  }
  else{
    streamlog::out.init( std::cout , binname) ;
  }
 
  streamlog::out.addLevelName<streamlog::MESSAGE0>() ;


  std::cout << "  hello world on std::cout " << std::endl ;
  
  
  streamlog_out( DEBUG ) << " this debug message will be printed only if " <<  std::endl  
		   << " the current log level is larger or equal to DEBUG::level and DEBUG::active==true" 
		   <<  std::endl ;
  
  
  if( streamlog::out.write<streamlog::DEBUG>() ){

    streamlog_test::fibonacci fibo(10) ;

    streamlog::out() << " sum of first 10 fibonacci numbers: " << fibo.sum() << std::endl ;
  }

  streamlog_message( MESSAGE ,   
		     streamlog_test::fibonacci fibo(20) ;  , 
		     " sum of first 20 fibonacci numbers: " << fibo.sum() << std::endl ;
		     )
  



  {
    streamlog::logscope scope(streamlog::out) ;
  
    scope.setName( "Subroutine") ;
    //scope.setLevel<streamlog::MESSAGE3>() ;
    scope.setLevel( "MESSAGE3" )  ;

    if( streamlog::out.write<streamlog::DEBUG>() )
      streamlog::out() << "    This message should never appear !!!! " << std::endl ;

    if( streamlog::out.write<streamlog::MESSAGE4>() )
      streamlog::out() << "    message in subroutine " << std::endl ;
 

  }

  if( streamlog::out.write<streamlog::MESSAGE>() )
    streamlog::out() << " another log  message .... " << std::endl ;
  
  if( streamlog::out.write<streamlog::WARNING2>() )
    streamlog::out() << " another log  message .... " << std::endl ;

  if( streamlog::out.write<streamlog::ERROR>() )
    streamlog::out() << " - no error - just testing .... " << std::endl ;


  streamlog_test::ATemplate<std::string> atemp ;

  atemp.doIt() ;



  if(file)
    delete file ;

}

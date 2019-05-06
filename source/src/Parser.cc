#include "marlin/Parser.h"

#include <algorithm>

namespace marlin{

  // open steering file with processor names
  Parser::Parser( const std::string&  fileName) :
    _current(0) , _fileName( fileName ) {
  }

  Parser::~Parser(){
  }


  void Parser::parse(){

    std::ifstream inFile( _fileName.c_str()  ) ;

    if( ! inFile ){
      std::cerr << "Parser::Parser:  couldn't open file: " << _fileName  << std::endl ;
      return ;
    }


    std::string inputLine ;
    StringParameters params ;

    while( readNextValidLine( inputLine , inFile)  ){


      std::vector<std::string> tokens ;
      LCTokenizer t( tokens ,' ') ;


      std::for_each( inputLine.begin(),inputLine.end(), t ) ;

      //     std::cout << " tokens "  ;
      //     for(int i=0;i<tokens.size();i++){
      //       std::cout <<  tokens[i] << "| " ;
      //     }
      //     std::cout << std::endl ;

      // ignore empty lines (containing whitespace and tabs only )
      if( tokens.size() < 1 ) continue ;

      // check whether a new section starts:
      if(  tokens[0] == ".begin"   ) {

	if( tokens.size() < 2 ) {
	  std::cerr << " Parser::parse : section has to have a name: .begin sectionName " << std::endl ;
	  exit(1) ;
	}
	_map[ tokens[1] ] = std::make_shared<StringParameters>();
	_current = _map[ tokens[1] ].get() ;
	//       std::cout << " Parser::parse: >>> creating new map entry : " <<  tokens[1] << std::endl ;

      } else if (  tokens[0] == ".end"   ) {

	_current = 0  ;

      } else if ( _current != 0 ) {

  auto key = tokens[0] ;
  tokens.erase( tokens.begin() ) ;
	_current->add( key, tokens ) ;


	//       std::cout << " Parser::parse: >>> "
	// 		<<  tokens[0] << " = "
	// 		<< _current->getIntVal( tokens[0] )   << " | "
	// 		<< _current->getFloatVal( tokens[0] ) << " | "
	// 		<< _current->getStringVal( tokens[0] )
	// 		<< std::endl ;

      }
    }


    // do some postprocessing ( create global entry with available processors )

    std::vector<std::string> availableProcs ;
    // availableProcs.push_back("AvailableProcessors") ;

    StringParameters* global = 0 ;

    for( StringParametersMap::const_iterator iter = _map.begin() ; iter != _map.end() ; iter++){

      std::string name = iter->first ;

      StringParameters* p = iter->second.get() ;

      std::string type = p->getValue<std::string>("ProcessorType") ;

      if( type.size() > 0 ) {

	availableProcs.push_back( name ) ;
      }

      if( name == "Global" )
	global = p ;

      //     std::cout << " parameter section " << iter->first
      // 	      << std::endl
      // 	      << *iter->second
      // 	      << std::endl ;

    }
    if( global != 0 )
      global->add( "AvailableProcessors", availableProcs ) ;
  }


  std::shared_ptr<StringParameters> Parser::getParameters( const std::string& sectionName ) const {

    for( StringParametersMap::const_iterator iter = _map.begin() ; iter != _map.end() ; iter++){
      //     std::cout << " parameter section " << iter->first
      // 	      << std::endl
      // 	      << *iter->second
      // 	      << std::endl ;
    }

     return _map[ sectionName ] ;

//     StringParametersMap::const_iterator iter ;
//     if( ( iter = _map.find( sectionName ) ) != _map.end() )
//       return  iter->second  ;
//     else
//       return 0 ;
  }


  int Parser::readNextValidLine( std::string& str , std::istream& stream){

    while(  ! stream.eof()  ) {

      getline( stream , str ) ;


      char firstChar = ' ' ;
      bool haveFirst = false ;

      for(unsigned int i=0; i < str.length() ; i ++){

	// replace tabs and cariage returns with whitespace
	if( (str[i] == '\t') || (str[i] == '\r') )  str[i] = ' ' ;

	// get first non whitespace character
	if( ! haveFirst &&  str[i] != ' ' ){
	  firstChar = str[i] ;
	  haveFirst = true ;
	}
      }
      //    if( str.length() != 0  && str[0] != '#' )
      if( str.length() != 0  && firstChar != '#' )
	return  str.length() ;
    }

    return 0 ;
  }



  void Parser::write(const std::string& fname) const{
    std::ifstream inFile( _fileName.c_str()  ) ;

    if( ! inFile ){
      std::cerr << "Parser::write:  couldn't open input file: " << _fileName  << std::endl ;
      return ;
    }

    std::ofstream outFile( fname.c_str()  ) ;

    if( ! outFile ){
      std::cerr << "Parser::write:  couldn't open output file: " << fname  << std::endl ;
      return ;
    }

    outFile << inFile.rdbuf() ;
    inFile.close() ;
    outFile.close() ;
  }




}  // namespace marlin

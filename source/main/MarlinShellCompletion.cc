
#include <marlin/XMLParser.h>
#include <marlin/Utils.h>

#include <set>

void dumpParameters( const std::string &section, std::shared_ptr<marlin::StringParameters> parameters, const std::set<std::string> &skip ) ;
void processCmdLine( int argc, char **argv, std::string &fname, marlin::CommandLineParametersMap &opts ) ;


/**
 *  Completion helper program. For internal use only
 *  Parse a Marlin steering file and printout of all possible 
 *  options that can be modified in the steering file by using 
 *  command line. The output is used by the bash completion program
 *  for the Marlin/MarlinMT program
 */
int main(int argc, char** argv ) {

  // ---- catch all uncaught exceptions in the end ...
  try {
    if( argc < 2 ) {
      // Return silently as this is used for shell completion
      return 1 ;
    }

    std::string steeringFileName ;
    marlin::CommandLineParametersMap opts ;

    processCmdLine( argc, argv, steeringFileName, opts ) ;

    std::unique_ptr<marlin::XMLParser> parser ( new marlin::XMLParser(steeringFileName) ) ;
    parser->setCmdLineParameters( opts ) ;
    parser->parse() ;

    auto sections = parser->getSections() ;
    // Global, Geometry, Constants, DataSource + Processors
    
    for( auto section : sections ) {
      auto parameters = parser->getParameters( section ) ;
      if( nullptr == parameters ) {
        continue ;
      }
      auto parameterKeys = parameters->keys() ;
      if( section == "Global" ) {
        dumpParameters( "global", parameters, { "ActiveProcessor", "AvailableProcessors", "ProcessorConditions" } ) ;
      }
      else if( section == "Geometry" ) {
        dumpParameters( "geometry", parameters, { "GeometryType" } ) ;
      }
      else if( section == "DataSource" ) {
        dumpParameters( "datasource", parameters, { "DataSourceType" } ) ;
      }
      else if( section == "Constants" ) {
        dumpParameters( "constant", parameters, {} ) ;
      }
      else {
        // processor case
        dumpParameters( section, parameters, { "ProcessorClone", "ProcessorCritical", "ProcessorName", "ProcessorType" } ) ;
      }
    }
    return 0 ;
  } 
  catch( std::exception& e) {
    std::cout << "Caught exception: " << e.what() << std::endl ;
    return 1 ;
  }
}



void dumpParameters( const std::string &section, std::shared_ptr<marlin::StringParameters> parameters, const std::set<std::string> &skip ) {
  auto parameterKeys = parameters->keys() ;
  for( auto key : parameterKeys ) {
    if( key.substr(0, 8) == "_marlin." ) {
      continue ;
    }
    if( skip.end() != skip.find( key ) ) {
      continue ;
    }
    std::cout << "--" << section << "." << key << "=" << std::endl ;          
  }
}



void processCmdLine( int argc, char **argv, std::string &fname, marlin::CommandLineParametersMap &opts ) {
  fname = argv[1] ;
  for( int i=2 ; i<argc ; i++ ) {
    std::string arg = argv[i] ;
    if ( arg.substr( 0, 2 ) == "--" ) {
      auto argVec = marlin::StringUtil::split<std::string>( arg.substr( 2 ) , "=" ) ;
      if ( argVec.size() != 2 ) {
        // std::cout << "*** invalid command line option: " << arg << std::endl ;
        continue ;
      }
      auto argKey = marlin::StringUtil::split<std::string>( argVec[0] , "." ) ;
      if ( argKey.size() != 2 ) {
        // std::cout << "*** invalid command line option: " << arg << std::endl ;
        continue ;
      }
      opts[ argKey[0] ][ argKey[1] ] = argVec[1] ;
    }
  }
}




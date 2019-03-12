#ifndef MARLIN_APPLICATION_h
#define MARLIN_APPLICATION_h 1

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/XMLParser.h"

namespace marlin {

  /**
   *  @brief  Application class
   */
  class Application {
  public:
    // traits
    typedef std::vector<std::string> CmdLineArguments ;

  public:
    /**
     *  @brief  Constructor
     */
    Application() ;

    /**
     *  @brief  Initialize the application
     *
     *  @param  argc argc from main function
     *  @param  argv argv from main function
     */
    void init( int argc, char **argv ) ;

    /**
     *  @brief  Run the Marlin application
     */
    void run() ;

    /**
     *  @brief  Get the program name
     */
    const std::string &program() const { return _programName ; }

    /**
     *  @brief  Get the command line arguments
     */
    const CmdLineArguments &arguments() const { return _arguments ; }

    /**
     *  @brief  Print command line usage
     */
    void printUsage() const ;
    
    /**
     *  @brief  Get the global section parameters
     */
    std::shared_ptr<StringParameters> globalParameters () const ;
    
    /**
     *  @brief  Get the processor parameters
     *
     *  @param  name the processor name
     */
    std::shared_ptr<StringParameters> processorParameters ( const std::string &name ) const ;
    
    /**
     *  @brief  Get the XML constants
     */
    std::shared_ptr<StringParameters> constants () const ;
    
    /**
     *  @brief  Get the active processor list
     */
    StringVec activeProcessors () const ;
    
    /**
     *  @brief  Get the active processor conditions list
     */
    StringVec processorConditions () const ;
    
    /**
     *  @brief  Get the concurrency level under which the application should run
     *  This initialized after calling init, else returns 0
     */
    unsigned int concurrencyLevel() const ;

  private:
    /**
     *  @brief  Parse the command line arguments
     */
    void parseCommandLine( CommandLineParametersMap &cmdLineOptions ) ;

  private:
    /// The program name. Initialized on init()
    std::string                _programName {} ;
    /// The arguments from the main function after init has been called
    CmdLineArguments           _arguments {} ;
    /// Whether the application has been initialized
    bool                       _initialized {false} ;
    /// The steering file name
    std::string                _steeringFileName {} ;
    /// The concurrency as read from the command line or steering file
    unsigned int               _concurrency {0};
    /// The XML steering file parser
    std::shared_ptr<XMLParser> _parser {nullptr} ;
  };

} // end namespace marlin

#endif

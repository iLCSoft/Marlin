#ifndef MARLIN_PROCESSORFACTORY_h
#define MARLIN_PROCESSORFACTORY_h 1

// -- std headers
#include <string>
#include <map>
#include <memory>

namespace marlin {
  
  class Processor ;
  class Application ;
  class StringParameters ;

  /**
   *  @brief  ProcessorFactory class
   *  Responsible for processor instance creation and initialization.
   */
  class ProcessorFactory {
    using ProcessorMap = std::map<std::string, std::shared_ptr<Processor>> ;
    
  public:
    ProcessorFactory() = default ;
    ~ProcessorFactory() = default ;
    
    /**
     *  @brief  Create a processor instance from parameters
     *  The processor type is contained in the processor parameters.
     *  The processor is also initialized by calling Processor::baseInit().
     *
     *  @param  app the application in which the processor runs
     *  @param  name the processor name
     *  @param  parameters the processor parameters
     */
    std::shared_ptr<Processor> createProcessor( 
      const Application *app,
      const std::string &name, 
      std::shared_ptr<StringParameters> parameters) ;
    
  private:
    ///< A map of single instance processors
    ProcessorMap                  _singleProcessors {} ;
  };

} // end namespace marlin

#endif

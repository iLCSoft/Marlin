#ifndef MARLIN_DATASOURCEPLUGIN_h
#define MARLIN_DATASOURCEPLUGIN_h 1

// -- marlin headers
#include <marlin/Parameter.h>
#include <marlin/Logging.h>

// -- std headers
#include <functional>
#include <memory>
#include <string>

// lcio forward declaration
namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {

  class Application ;

  /**
   *  @brief  DataSourcePlugin class
   *  Responsible for reading/getting LCEvent and LCRunHeader
   *  in the framework for further processing
   */
  class DataSourcePlugin : public Parametrized {
  public:
    using EventFunction = std::function<void(std::shared_ptr<EVENT::LCEvent>)> ;
    using RunHeaderFunction = std::function<void(std::shared_ptr<EVENT::LCRunHeader>)> ;
    using Logger = Logging::Logger ;

  public:
    virtual ~DataSourcePlugin() = default ;

    /**
     *  @brief  Constructor
     *
     *  @param  dstype the data source plugin type
     */
    DataSourcePlugin( const std::string &dstype ) ;

    /**
     *  @brief  Initialize the plugin using application parameters
     *
     *  @param  app the application in which the plugin is running
     */
    void init( const Application *app ) ;

    /**
     *  @brief  Get the data source type
     */
    const std::string &type() const ;

    /**
     *  @brief  Get the data source description
     */
    const std::string &description() const ;

    /**
     *  @brief  Initialize the plugin
     */
    virtual void init() = 0 ;

    /**
     *  @brief  Read one record from the input stream
     *
     *  Users must call processRunHeader() or processEvent()
     *  to forward it to the framework. Returns true on success.
     *  If the end of the stream is reached, return false.
     */
    virtual bool readOne() = 0 ;

    /**
     *  @brief  Read the full stream until the end
     *  See readOne() for details
     */
    virtual void readAll() ;

    /**
     *  @brief  Set the callback function to process on event read
     *
     *  @param  func the callback function
     */
    void onEventRead( EventFunction func ) ;

    /**
     *  @brief  Set the callback function to process on run header read
     *
     *  @param  func the callback function
     */
    void onRunHeaderRead( RunHeaderFunction func ) ;

    /**
     *  @brief  Get the plugin logger
     */
    Logger logger() const ;

  protected:
    /**
     *  @brief  Must be called by daughter classes in readStream()
     *  to process an event in the framework
     *
     *  @param  event the event to process
     */
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Must be called by daughter classes in readStream()
     *  to process an event in the framework
     *
     *  @param  event the event to process
     */
    void processEvent( std::shared_ptr<EVENT::LCEvent> event ) ;

  protected:
    ///< The data source description
    std::string              _description {"No description"} ;

  private:
    ///< The data source type
    const std::string        _type ;
    ///< The plugin logger
    Logger                   _logger {nullptr} ;
    ///< The callback function on event read
    EventFunction            _onEventRead {nullptr} ;
    ///< The callback function on run header read
    RunHeaderFunction        _onRunHeaderRead {nullptr} ;
  };

}

#endif

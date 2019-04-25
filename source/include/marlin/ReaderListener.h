#ifndef MARLIN_READERLISTENER_h
#define MARLIN_READERLISTENER_h 1

// -- lcio headers
#include <MT/LCReaderListener.h>

// -- std headers
#include <functional>

namespace marlin {

  /**
   *  @brief  ReaderListener class.
   *
   *  Simple implementation of a reader listener.
   *  Callback functions can be set using lambda function, std::function objects
   *  or resulting call of std::bind call.
   *
   *  Example with lambda functions:
   *  @code{cpp}
   *  ReaderListener listener ;
   *  listener.onEventRead( [](std::shared_ptr<EVENT::LCEvent> event){
   *    std::cout << "Read event no " << event->getEventNumber() << std::endl ;
   *  }) ;
   *  listener.onRunHeaderRead( [](std::shared_ptr<EVENT::LCRunHeader> rhdr){
   *    std::cout << "Read run header no " << rhdr->getRunNumber() << std::endl ;
   *  }) ;
   *  @endcode
   *
   *  Example with std::bind and custom class method:
   *  @code{cpp}
   *  using namespace std::placeholders ;
   *  UserClass user ;
   *  ReaderListener listener ;
   *  listener.onEventRead( std::bind(&UserClass::processEvent, &user, _1) ) ;
   *  listener.onRunHeaderRead( std::bind(&UserClass::processRunHeader, &user, _1) ) ;
   *  @endcode
   *
   * Note that the current implementation forward the processing only on
   * modifyEvent() and modifyRunHeader(). Thus the data can be modified in
   * callback functions. This is a requirement for concurrent applications
   */
  class ReaderListener : public MT::LCReaderListener {
  public:
    using LCEventFunction = std::function<void(std::shared_ptr<EVENT::LCEvent>)> ;
    using LCRunHeaderFunction = std::function<void(std::shared_ptr<EVENT::LCRunHeader>)> ;

  public:
    ReaderListener() = default ;
    ~ReaderListener() = default ;
    ReaderListener(const ReaderListener&) = delete ;
    ReaderListener &operator=(const ReaderListener&) = delete ;

    /**
     *  @brief  Set the callback function to process on event read
     */
    void onEventRead( LCEventFunction func ) ;

    /**
     *  @brief  Set the callback function to process on run header read
     */
    void onRunHeaderRead( LCRunHeaderFunction func ) ;

  private:
    // from MT::LCReaderListener
    void processEvent( std::shared_ptr<EVENT::LCEvent> event ) override ;
    void modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) override ;
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> hdr ) override ;
    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> hdr ) override ;

  private:
    ///< Callback function on event read
    LCEventFunction        _onEventRead {nullptr} ;
    ///< Callback function on run header read
    LCRunHeaderFunction    _onRunHeaderRead {nullptr} ;
  };

}

#endif

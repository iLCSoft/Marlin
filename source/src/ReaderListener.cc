#include <marlin/ReaderListener.h>

namespace marlin {

  void ReaderListener::onEventRead( LCEventFunction func ) {
    _onEventRead = func ;
  }
  
  //--------------------------------------------------------------------------
  
  void ReaderListener::onRunHeaderRead( LCRunHeaderFunction func ) {
    _onRunHeaderRead = func ;
  }
  
  //--------------------------------------------------------------------------

  void ReaderListener::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    if( nullptr != _onEventRead ) {
      _onEventRead( event ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void ReaderListener::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    if( nullptr != _onRunHeaderRead ) {
      _onRunHeaderRead( rhdr ) ;
    }
  }

}

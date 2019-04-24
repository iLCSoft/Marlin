#include <marlin/MarlinPEPApplication.h>

// -- marlin headers
#include <marlin/Utils.h>
#include <marlin/concurrency/PEPScheduler.h>

using namespace std::placeholders ;

namespace marlin {
  
  void MarlinPEPApplication::runApplication() {
    _fileReader.open( _lcioFileList ) ;
    // skip events if requested
    if ( _skipNEvents > 0 ) {
      logger()->log<WARNING>() << " --- Will skip first " << _skipNEvents << " event(s)" << std::endl ;
      _fileReader.skipNEvents( _skipNEvents ) ;
    }
    try {
      MT::LCReaderListenerList listeners = {&_readerListener} ;
      if( _maxRecordNumber > 0 ){
        try {
          _fileReader.readStream( listeners, _maxRecordNumber ) ;
        }
        catch( IO::EndOfDataException &e ) {
          logger()->log<WARNING>() << e.what() << std::endl ;
        }
      } 
      else {
        _fileReader.readStream( listeners ) ;
      }
    }
    catch( StopProcessingException &e ) {
      logger()->log<ERROR>() << std::endl
          << " **********************************************************" << std::endl
          << " *                                                        *" << std::endl
          << " *   Stop of EventProcessing requested by processor :     *" << std::endl
          << " *                  "  << e.what()                           << std::endl
          << " *     will call end() method of all processors !         *" << std::endl
          << " *                                                        *" << std::endl
          << " **********************************************************" << std::endl
          << std::endl ;
    }
    catch( RewindDataFilesException &e ) {
      // TODO re-activate this functionality
      logger()->log<WARNING>() << "Files rewind functionality is not available in MarlinMT" << std::endl ;
      logger()->log<WARNING>() << "Application will terminate normally now ..." << std::endl ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::init() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    configureScheduler() ;
    configureFileReader() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::end() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _scheduler->end() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::printUsage() const {

  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::configureScheduler() {
    _scheduler = std::make_shared<concurrency::PEPScheduler>() ;
    _scheduler->init( this ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::configureFileReader() {
    // configure the file reader
    auto globals = globalParameters() ;
    globals->getStringVals( "LCIOInputFiles" , _lcioFileList ) ;
    if ( _lcioFileList.empty() ) {
      logger()->log<ERROR>() << "No LCIO iput files found in </global> section of steering file" << std::endl ;
      throw Exception( "No LCIO iput files found in </global> section of steering file" ) ;
    }
    _maxRecordNumber = globals->getIntVal( "MaxRecordNumber" ) ;
    _skipNEvents = globals->getIntVal( "SkipNEvents" ) ;
    EVENT::StringVec readCollectionNames {} ;
    globals->getStringVals( "LCIOReadCollectionNames" , readCollectionNames ) ;
    if ( not readCollectionNames.empty() ) {
      logger()->log<WARNING>()
        << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** " 
        << std::endl ;
      for( auto collection : readCollectionNames ) {
        logger()->log<WARNING>()  << "     " << collection << std::endl ;
      } 
      logger()->log<WARNING>() 
        << " *************************************************************************************************** " << std::endl ;
      _fileReader.setReadCollectionNames( readCollectionNames ) ;
    }
    // configure the reader listener
    _readerListener.onEventRead( std::bind( &MarlinPEPApplication::onEventRead, this, _1 ) ) ;
    _readerListener.onRunHeaderRead( std::bind( &MarlinPEPApplication::onRunHeaderRead, this, _1 ) ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::onEventRead( std::shared_ptr<EVENT::LCEvent> event ) {
    EventList events ;
    while( _scheduler->freeSlots() == 0 ) {
      _scheduler->popFinishedEvents( events ) ;
      if( not events.empty() ) {
        processFinishedEvents( events ) ;
        events.clear() ;
        break;
      }        
      std::this_thread::sleep_for( std::chrono::milliseconds(1) ) ;
    }
    _scheduler->pushEvent( event ) ;
    // check a second time
    _scheduler->popFinishedEvents( events ) ;
    if( not events.empty() ) {
      processFinishedEvents( events ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::onRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    logger()->log<MESSAGE9>() << "New run header no " << rhdr->getRunNumber() << std::endl ;
    _scheduler->processRunHeader( rhdr ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinPEPApplication::processFinishedEvents( const EventList &events ) const {
    // simple printout for the time being
    for( auto event : events ) {
      logger()->log<MESSAGE9>() 
        << "Run no " << event->getRunNumber() 
        << ", event no " << event->getEventNumber() 
        << " finished" << std::endl ;
    }
  }

} // namespace marlin

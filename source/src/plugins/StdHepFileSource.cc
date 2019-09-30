#ifndef MARLIN_STDHEPFILESOURCE_h
#define MARLIN_STDHEPFILESOURCE_h 1

// -- marlin headers
#include <marlin/DataSourcePlugin.h>
#include <marlin/ReaderListener.h>
#include <marlin/PluginManager.h>
#include <marlin/Logging.h>

// -- lcio headers
#include <IMPL/LCEventImpl.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <UTIL/LCStdHepRdr.h>
#include <UTIL/LCTOOLS.h>

// -- std headers
#include <functional>

using namespace std::placeholders ;

namespace marlin {

  /**
   *  @brief  StdHepFileSource class
   */
  class StdHepFileSource : public DataSourcePlugin {
    using FileReader = std::shared_ptr<UTIL::LCStdHepRdr> ;

  public:
    StdHepFileSource() ;
    ~StdHepFileSource() = default ;

    // from DataSourcePlugin
    void init() ;
    bool readOne() ;

  private:
    Property<std::string> _fileName {this, "StdHepFileName",
            "The StdHep input file name" } ;
            
    Property<int> _maxRecordNumber {this, "MaxRecordNumber",
            "The maximum number of events to read", 0 } ;

    Property<std::string> _collectionName {this, "CollectionName",
            "The name of the MCParticle collection to add in the event", "MCParticle" } ;

    ///< The stdhep file reader
    FileReader                  _fileReader {nullptr} ;
    ///< Whether we are processing the first event
    bool                        _isFirstEvent {true} ;
    ///< The current number of read events
    int                         _currentReadEvents {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  StdHepFileSource::StdHepFileSource() :
    DataSourcePlugin("StdHep") {
    _description = "Reads StdHep files as input and creates LCIO events with MCParticle collections" ;
  }

  //--------------------------------------------------------------------------

  void StdHepFileSource::init() {
    // create the file reader
    _fileReader = std::make_shared<FileReader::element_type>( _fileName.get().c_str() ) ;
  }

  //--------------------------------------------------------------------------

  bool StdHepFileSource::readOne() {
    // first call is a run header
    if( _isFirstEvent ) {
      auto rhdr = std::make_shared<IMPL::LCRunHeaderImpl>() ;
      rhdr->setDescription( " Events read from stdhep input file: " + _fileName.get() ) ;
      rhdr->setRunNumber( 0 ) ;
      processRunHeader( rhdr ) ;
      _isFirstEvent = false ;
      return true ;
    }
    // next calls are events from the stdhep reader
    auto collection = _fileReader->readEvent() ;
    if( nullptr == collection ) {
      return false ;
    }
    if( (_currentReadEvents > 0) and (_currentReadEvents >= _maxRecordNumber) ) {
      delete collection ;
      return false ;
    }
    auto event = std::make_shared<IMPL::LCEventImpl>() ;
    event->setRunNumber( 0 ) ;
    event->setEventNumber( _currentReadEvents ) ;
    event->addCollection( collection, _collectionName ) ;
    processEvent( event ) ;
    ++_currentReadEvents ;
    return true ;
  }

  MARLIN_DECLARE_DATASOURCE_NAME( StdHepFileSource, "StdHep" )

}

#endif

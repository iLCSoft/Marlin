#ifndef MARLIN_LCIOFILESOURCE_h
#define MARLIN_LCIOFILESOURCE_h 1

// -- marlin headers
#include <marlin/DataSourcePlugin.h>
#include <marlin/ReaderListener.h>
#include <marlin/PluginManager.h>
#include <marlin/Logging.h>

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>
#include <MT/LCReader.h>
#include <MT/LCReaderListener.h>

// -- std headers
#include <functional>

using namespace std::placeholders ;

namespace marlin {

  /**
   *  @brief  LCIOFileSource class
   */
  class LCIOFileSource : public DataSourcePlugin {
    using FileReader = MT::LCReader ;
    using FileReaderPtr = std::shared_ptr<FileReader> ;

  public:
    LCIOFileSource() ;
    ~LCIOFileSource() = default ;

    // from DataSourcePlugin
    void init() ;
    bool readOne() ;

  private:
    ///< The LCIO input file list
    EVENT::StringVec            _inputFileNames {} ;
    ///< The number of events to skip on file open
    int                         _skipNEvents {0} ;
    ///< The maximum nuber of records to read
    int                         _maxRecordNumber {0} ;
    ///< The list of LCIO collections to read
    EVENT::StringVec            _readCollectionNames {} ;
    ///< The LCIO file listener
    ReaderListener              _listener {} ;
    ///< The LCIO file reader
    FileReaderPtr               _fileReader {nullptr} ;
    ///< The current number of read records
    int                         _currentReadRecords {0} ;
    ///< Whether to perform a lazy unpack of event
    bool                        _lazyUnpack {false} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOFileSource::LCIOFileSource() :
    DataSourcePlugin("LCIO") {

    _description = "Read LCIO events and run headers from files on disk" ;

    registerParameter( "LCIOInputFiles",
      "The list of LCIO input files",
      _inputFileNames,
      EVENT::StringVec()) ;

    registerParameter( "MaxRecordNumber",
      "The maximum number of records to read",
      _maxRecordNumber,
      static_cast<int>(0)) ;

    registerParameter( "SkipNEvents",
      "The number of events to skip on file open",
      _skipNEvents,
      static_cast<int>(0)) ;

    registerParameter( "LCIOReadCollectionNames",
      "An optional list of LCIO collections to read from event",
      _readCollectionNames,
      EVENT::StringVec()) ;
      
    registerParameter( "LazyUnpack",
      "Set to true to perform a lazy unpacking after reading out an event",
      _lazyUnpack,
      false) ;
      
  }

  //--------------------------------------------------------------------------

  void LCIOFileSource::init() {
    auto flag = FileReader::directAccess ;
    if( _lazyUnpack ) {
      flag |= FileReader::lazyUnpack ;
    }
    _fileReader = std::make_shared<FileReader>( flag ) ;
    _listener.onRunHeaderRead( std::bind( &LCIOFileSource::processRunHeader, this, _1 ) ) ;
    _listener.onEventRead( std::bind( &LCIOFileSource::processEvent, this, _1 ) ) ;

    if( _inputFileNames.empty() ) {
      throw Exception( "LCIOFileSource::init: LCIO input file list is empty" ) ;
    }
    _fileReader->open( _inputFileNames ) ;
    if ( _skipNEvents > 0 ) {
      logger()->log<WARNING>() << " --- Will skip first " << _skipNEvents << " event(s)" << std::endl ;
      _fileReader->skipNEvents( _skipNEvents ) ;
    }
    if ( not _readCollectionNames.empty() ) {
      logger()->log<WARNING>()
        << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** "
        << std::endl ;
      for( auto collection : _readCollectionNames ) {
        logger()->log<WARNING>()  << "     " << collection << std::endl ;
      }
      logger()->log<WARNING>()
        << " *************************************************************************************************** " << std::endl ;
      _fileReader->setReadCollectionNames( _readCollectionNames ) ;
    }
  }

  //--------------------------------------------------------------------------

  bool LCIOFileSource::readOne() {
    try {
      _fileReader->readNextRecord( &_listener ) ;
      ++_currentReadRecords ;
      if( (_maxRecordNumber > 0) and (_currentReadRecords >= _maxRecordNumber) ) {
        return false ;
      }
    }
    catch( IO::EndOfDataException &e ) {
      return false ;
    }
    return true ;
  }

  MARLIN_DECLARE_DATASOURCE_NAME( LCIOFileSource, "LCIO" )

}

#endif

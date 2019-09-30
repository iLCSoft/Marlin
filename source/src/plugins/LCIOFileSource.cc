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
    Property<std::vector<std::string>> _inputFileNames {this, "LCIOInputFiles",
                "The list of LCIO input files" } ;

    Property<int> _maxRecordNumber {this, "MaxRecordNumber",
                "The maximum number of records to read", 0 } ;
      
    Property<int> _skipNEvents {this, "SkipNEvents",
                "The number of events to skip on file open", 0 } ;
                
    Property<std::vector<std::string>> _readCollectionNames {this, "LCIOReadCollectionNames",
                "An optional list of LCIO collections to read from event" } ;
                
    Property<bool> _lazyUnpack {this, "LazyUnpack",
                "Set to true to perform a lazy unpacking after reading out an event", false } ;
    
    ///< The LCIO file listener
    ReaderListener              _listener {} ;
    ///< The LCIO file reader
    FileReaderPtr               _fileReader {nullptr} ;
    ///< The current number of read records
    int                         _currentReadRecords {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOFileSource::LCIOFileSource() :
    DataSourcePlugin("LCIO") {
    _description = "Read LCIO events and run headers from files on disk" ;
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

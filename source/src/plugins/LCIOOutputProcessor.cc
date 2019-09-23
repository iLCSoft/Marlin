
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/PluginManager.h>

// -- lcio headers
#include <lcio.h>
#include <MT/LCWriter.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <UTIL/LCTOOLS.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>

// -- std headers
#include <iostream>
#include <algorithm>
#include <bitset>

namespace marlin {
  
  /** Default output processor. If active every event is writen to the 
   *  specified LCIO file.  
   *  Make sure that the processor is the last one in your list
   *  of active processors. You can optionally drop some collections from the
   *  event before it gets written to the file, e.g. you can drop 
   *  all collections of types SimCalorimeterHit and SimTrackerHit. It is the users
   *  responsibility to check whether the droped objects are still referenced by other 
   *  objects (e.g. LCRelations) and drop those collections as well - if needed. 
   *  If CalorimeterHit and TrackerHit objects are droped then Tracks and clusters will be store w/o
   *  pointers to hits.
   * 
   *  <h4>Output</h4> 
   *  file containing the LCIO events
   * 
   * @param LCIOOutputFile  name of outputfile incl. path
   * @param LCIOWriteMode   WRITE_NEW, WRITE_APPEND  [optional]
   * @param DropCollectionNames   name of collections to be droped  [optional]
   * @param DropCollectionTypes   type of collections to be droped  [optional]
   * 
   * 
   * @param DropCollectionNames   drops the named collections from the event
   * @param DropCollectionTypes   drops all collections of the given type from the event
   * @param LCIOOutputFile        name of output file
   * @param LCIOWriteMode         write mode for output file:  WRITE_APPEND or WRITE_NEW
   * @param KeepCollectionNames   names of collections that are to be kept unconditionally
   * @param fullSubsetCollections optionally write all objects in subset collections to the file
   *   
   * 
   * @author F. Gaede, DESY
   * @version $Id: LCIOOutputProcessor.h,v 1.8 2008-04-15 10:14:24 gaede Exp $ 
   */
  class LCIOOutputProcessor : public Processor {
  private:
    typedef std::vector< IMPL::LCCollectionVec* > SubSetVec ;
    typedef std::shared_ptr<MT::LCWriter> Writer ;

  public:
    LCIOOutputProcessor() ;
    LCIOOutputProcessor(const LCIOOutputProcessor&) = delete ;
    LCIOOutputProcessor& operator=(const LCIOOutputProcessor&) = delete ;
    ~LCIOOutputProcessor() = default ;

    /**Open the LCIO outputfile.
     */
    void init() ;

    /** Write every run header.
     */
    void processRunHeader( EVENT::LCRunHeader* run) ;

    /** Write every event.
     */
    void processEvent( EVENT::LCEvent * evt ) ; 

    /** Close outputfile.
     */
    void end() ;

    /** Drops the collections specified in the steering file parameters DropCollectionNames and 
     *  DropCollectionTypes. 
     */
    void dropCollections( EVENT::LCEvent * evt ) ;
    
    
  private:
    std::set<std::string> getWriteCollections( EVENT::LCEvent * evt ) const ;

  private:
    Property<std::string> _lcioOutputFile {this, "LCIOOutputFile", 
             "Name of the LCIO output file", "outputfile.slcio" } ;
    
    Property<std::string> _lcioWriteMode {this, "LCIOWriteMode",
             "Write mode for output file:  WRITE_APPEND, WRITE_NEW or None", "None" } ;

    OptionalProperty<std::vector<std::string>> _dropCollectionNames {this, "DropCollectionNames" , 
             "drops the named collections from the event", {"TPCHits", "HCalHits"} } ; 
    
    OptionalProperty<std::vector<std::string>> _dropCollectionTypes {this, "DropCollectionTypes" , 
             "drops all collections of the given type from the event", {"SimTrackerHit", "SimCalorimeterHit"} } ; 
    
    OptionalProperty<std::vector<std::string>> _keepCollectionNames {this, "KeepCollectionNames" , 
             "force keep of the named collections - overrules DropCollectionTypes (and DropCollectionNames)", {"MyPreciousSimTrackerHits"} } ; 
    
    // OptionalProperty<int> _splitFileSizekB {this, "SplitFileSizekB" , 
    //          "will split output file if size in kB exceeds given value - doesn't work with APPEND and NEW", 1992294 } ;

    // runtime members
    Writer                _writer {nullptr} ;
    std::atomic<int>      _nRuns {0} ;
    std::atomic<int>      _nEvents {0} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOOutputProcessor::LCIOOutputProcessor() : 
    Processor("LCIOOutputProcessor") {
    _description = "Writes the current event to the specified LCIO outputfile." ;    
    // no lock, the writer implementation is thread safe
    forceRuntimeOption( Processor::RuntimeOption::Critical, false ) ;
    // don't duplicate opening/writing of output file
    forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
  }

  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::init() {
    printParameters() ;
    _writer = std::make_shared<Writer::element_type>() ;
    if ( _lcioWriteMode == "WRITE_APPEND" ) {  	 
      _writer->open( _lcioOutputFile , EVENT::LCIO::WRITE_APPEND ) ;
    }
    else if ( _lcioWriteMode == "WRITE_NEW" ) {
      _writer->open( _lcioOutputFile , EVENT::LCIO::WRITE_NEW ) ;
    }
    else {
      _writer->open( _lcioOutputFile ) ;
    }
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::processRunHeader( EVENT::LCRunHeader* run) { 
    _writer->writeRunHeader( run ) ;
    _nRuns++ ;
  }
  
  //--------------------------------------------------------------------------

  std::set<std::string> LCIOOutputProcessor::getWriteCollections( EVENT::LCEvent * evt ) const {
    auto colNames = evt->getCollectionNames() ;
    std::set<std::string> writeCollections {} ;
    // loop over collections and collect the ones to actually write
    for( auto colName : *colNames ) {
      auto col = evt->getCollection( colName ) ;
      std::string collectionType  = col->getTypeName() ;
      auto typeIter = std::find( _dropCollectionTypes.get().begin(), _dropCollectionTypes.get().end(), collectionType ) ;
      auto nameIter = std::find( _dropCollectionNames.get().begin(), _dropCollectionNames.get().end(), colName ) ;
      auto keepIter = std::find( _keepCollectionNames.get().begin(), _keepCollectionNames.get().end(), colName ) ;
      if ( _dropCollectionTypes.isSet() && typeIter != _dropCollectionTypes.get().end() ) {
         continue ;
      }
      if ( _dropCollectionNames.isSet() && nameIter != _dropCollectionNames.get().end() ) {	
         continue ;
      }
      if( _keepCollectionNames.isSet() && keepIter != _keepCollectionNames.get().end() ) {	
         continue ;
      }
      writeCollections.insert( colName ) ;
    }
    return writeCollections ;
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::processEvent( EVENT::LCEvent * evt ) {
    auto writeCols = getWriteCollections( evt ) ;
    _writer->writeEvent( evt, writeCols ) ;
    _nEvents ++ ;
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::end() { 
    log<MESSAGE4>() << std::endl 
  			      << "LCIOOutputProcessor::end()  " << name() 
  			      << ": " << _nEvents.load() << " events in " << _nRuns.load() << " runs written to file  " 
  			      <<  _lcioOutputFile
  			      << std::endl
  			      << std::endl ;
    _writer->close() ;
    _writer = nullptr ;
  }

  MARLIN_DECLARE_PROCESSOR( LCIOOutputProcessor )
}

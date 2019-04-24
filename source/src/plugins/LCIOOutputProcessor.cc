
// -- marlin headers
#include <marlin/Processor.h>

// -- lcio headers
#include <lcio.h>
#include <IO/LCWriter.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <UTIL/LCTOOLS.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCSplitWriter.h>

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

  public:
    LCIOOutputProcessor() ;
    LCIOOutputProcessor(const LCIOOutputProcessor&) = delete ;
    LCIOOutputProcessor& operator=(const LCIOOutputProcessor&) = delete ;
    ~LCIOOutputProcessor() = default ;
    
    Processor *newProcessor() { 
      return new LCIOOutputProcessor ; 
    }

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
    // processor parameters
    std::string           _lcioOutputFile {""} ;
    std::string           _lcioWriteMode {""} ;
    EVENT::StringVec      _dropCollectionNames {} ;
    EVENT::StringVec      _dropCollectionTypes {} ;
    EVENT::StringVec      _keepCollectionNames {} ;
    EVENT::StringVec      _fullSubsetCollections {} ;
    int                   _splitFileSizekB {1992294} ; // 1.9 GB in kB

    // runtime members
    SubSetVec             _subSets{} ;
    IO::LCWriter         *_lcWrt {nullptr} ;
    int                   _nRun {0} ;
    int                   _nEvt {0} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOOutputProcessor::LCIOOutputProcessor() : 
    Processor("LCIOOutputProcessor") {
      
    _description = "Writes the current event to the specified LCIO outputfile."
      " Needs to be the last ActiveProcessor." ;
    
    registerProcessorParameter( "LCIOOutputFile" , 
				" name of output file "  ,
				_lcioOutputFile ,
				std::string("outputfile.slcio") ) ;
    
    registerProcessorParameter( "LCIOWriteMode" , 
				"write mode for output file:  WRITE_APPEND, WRITE_NEW or None"  ,
				_lcioWriteMode ,
				std::string("None") ) ;

    StringVec dropNamesExamples ;
    dropNamesExamples.push_back("TPCHits");
    dropNamesExamples.push_back("HCalHits");

    registerOptionalParameter( "DropCollectionNames" , 
 			       "drops the named collections from the event"  ,
 			       _dropCollectionNames ,
 			       dropNamesExamples ) ;
    
    StringVec dropTypesExample ;
    dropTypesExample.push_back("SimTrackerHit");
    dropTypesExample.push_back("SimCalorimeterHit");
    
    registerOptionalParameter( "DropCollectionTypes" , 
			       "drops all collections of the given type from the event"  ,
			       _dropCollectionTypes ,
			       dropTypesExample ) ;
    
    StringVec keepNamesExample ;
    keepNamesExample.push_back("MyPreciousSimTrackerHits");

    registerOptionalParameter( "KeepCollectionNames" , 
			       "force keep of the named collections - overrules DropCollectionTypes (and DropCollectionNames)"  ,
			       _keepCollectionNames ,
			       keepNamesExample ) ;

    StringVec fullSubsetExample ;
    fullSubsetExample.push_back("MCParticlesSkimmed");

    registerOptionalParameter( "FullSubsetCollections" , 
			       " write complete objects in subset collections to the file (i.e. ignore subset flag)"  ,
			       _fullSubsetCollections ,
			       fullSubsetExample ) ;

    registerOptionalParameter( "SplitFileSizekB" , 
			       "will split output file if size in kB exceeds given value - doesn't work with APPEND and NEW"  ,
			       _splitFileSizekB, 
			       _splitFileSizekB ) ;
                        
    // make it thread-safe by using a lock
    forceRuntimeOption( Processor::RuntimeOption::Critical, true ) ;
    // don't duplicate opening/writing of output file
    forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
  }

  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::init() {

    printParameters() ;

    if (  parameterSet("SplitFileSizekB") ) {
      _lcWrt = new LCSplitWriter( LCFactory::getInstance()->createLCWriter(), _splitFileSizekB*1024  ) ;
    } 
    else {
      _lcWrt = LCFactory::getInstance()->createLCWriter() ;
    }

    if ( _lcioWriteMode == "WRITE_APPEND" ) {  	 
      _lcWrt->open( _lcioOutputFile , LCIO::WRITE_APPEND ) ;
    }
    else if ( _lcioWriteMode == "WRITE_NEW" ) {
      _lcWrt->open( _lcioOutputFile , LCIO::WRITE_NEW ) ;
    }
    else {
      _lcWrt->open( _lcioOutputFile ) ;
    }
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::processRunHeader( LCRunHeader* run) { 
    _lcWrt->writeRunHeader( run ) ;
    _nRun++ ;
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::dropCollections( LCEvent * evt ) {
    const StringVec*  colNames = evt->getCollectionNames() ;

    // if all tracker hits are droped we don't store the hit pointers with the tracks below ...
    bool trackerHitsDroped = false ;
    bool calorimeterHitsDroped = false ;

    if( parameterSet("DropCollectionTypes") ) {
      if( std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end()
		     , LCIO::TRACKERHIT ) != _dropCollectionTypes.end() ) {
	      trackerHitsDroped =  true ;
      }
      if( std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end()
		     , LCIO::CALORIMETERHIT )   != _dropCollectionTypes.end()  ) {
	      calorimeterHitsDroped =  true ;
      }
    }      
    
    for( StringVec::const_iterator it = colNames->begin(); it != colNames->end() ; it++ ) {
      
      IMPL::LCCollectionVec*  col =  dynamic_cast<LCCollectionVec*> (evt->getCollection( *it ) ) ;
      
      std::string collectionType  = col->getTypeName() ;
      auto typeIter = std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end(), collectionType ) ;
      auto nameIter = std::find( _dropCollectionNames.begin(), _dropCollectionNames.end(), *it ) ;
      auto keepIter = std::find( _keepCollectionNames.begin(), _keepCollectionNames.end(), *it ) ;
      auto subsetIter = std::find( _fullSubsetCollections.begin(), _fullSubsetCollections.end(), *it ) ;
      
      if ( parameterSet("DropCollectionTypes") && typeIter != _dropCollectionTypes.end() ) {
	       col->setTransient( true ) ;
      }
      if ( parameterSet("DropCollectionNames") && nameIter != _dropCollectionNames.end() ) {	
	       col->setTransient( true ) ;
      }

      if( parameterSet("KeepCollectionNames") && keepIter != _keepCollectionNames.end() ) {	
	       col->setTransient( false ) ;
      }
      if( parameterSet("FullSubsetCollections") && subsetIter != _fullSubsetCollections.end() ) {
      	if( col->isSubset() ) {
      	  col->setSubset( false ) ;
      	  _subSets.push_back(col) ;
      	}
      }
      // don't store hit pointers if hits are droped
      if ( collectionType == LCIO::TRACK && trackerHitsDroped ) {
      	std::bitset<32> flag( col->getFlag() ) ;
      	flag[ LCIO::TRBIT_HITS ] = 0 ;
       	col->setFlag( flag.to_ulong() ) ;
      }
      if(  collectionType == LCIO::CLUSTER && calorimeterHitsDroped ) {
      	std::bitset<32> flag( col->getFlag() ) ;
      	flag[ LCIO::CLBIT_HITS ] = 0 ;
       	col->setFlag( flag.to_ulong() ) ;
      }
    }
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::processEvent( LCEvent * evt ) {
    dropCollections( evt ) ;
    _lcWrt->writeEvent( evt ) ;
    // revert subset flag - if any 
    for( SubSetVec::iterator sIt = _subSets.begin() ; 
         sIt != _subSets.end() ;  ++sIt  ) {
      
      (*sIt)->setSubset( true ) ;
    }
    _subSets.clear() ;
    _nEvt ++ ;
  }
  
  //--------------------------------------------------------------------------

  void LCIOOutputProcessor::end() { 

    log<MESSAGE4>() << std::endl 
  			      << "LCIOOutputProcessor::end()  " << name() 
  			      << ": " << _nEvt << " events in " << _nRun << " runs written to file  " 
  			      <<  _lcioOutputFile  
  			      << std::endl
  			      << std::endl ;
    
    _lcWrt->close() ;
    delete _lcWrt;
    _lcWrt = nullptr;
  }

  LCIOOutputProcessor anLCIOOutputProcessor ;

}

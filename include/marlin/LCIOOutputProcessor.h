#ifndef LCIOOutputProcessor_h
#define LCIOOutputProcessor_h 1

#include "Processor.h"
#include "lcio.h"
#include "IO/LCWriter.h"


using namespace lcio ;

namespace IMPL{
  class LCCollectionVec ;
}

namespace marlin{

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
  
    typedef std::vector< LCCollectionVec* > SubSetVec ;


  public:  

    virtual Processor*  newProcessor() { return new LCIOOutputProcessor ; }
  
  
    LCIOOutputProcessor() ;

    /** C'tor for possible subclasses */
    LCIOOutputProcessor(const std::string& typeName) ;

    /** D'tor for subclasses */
    virtual ~LCIOOutputProcessor() ;

    /**Open the LCIO outputfile.
     */
    virtual void init() ;

    /** Write every run header.
     */
    virtual void processRunHeader( LCRunHeader* run) ;

    /** Write every event.
     */
    virtual void processEvent( LCEvent * evt ) ; 

    /** Close outputfile.
     */
    virtual void end() ;

    /** Drops the collections specified in the steering file parameters DropCollectionNames and 
     *  DropCollectionTypes. 
     */
    void dropCollections( LCEvent * evt ) ;


  protected:

    std::string _lcioOutputFile ;
    std::string _lcioWriteMode ; 

    StringVec _dropCollectionNames ; 
    StringVec _dropCollectionTypes ; 
    StringVec _keepCollectionNames ; 
    StringVec _fullSubsetCollections ; 

    int _splitFileSizekB ;

    SubSetVec _subSets ;

    LCWriter* _lcWrt ;
    int _nRun ;
    int _nEvt ;

  private:
  
    /** Inititalization for constructors */
    void myConstruct() ;

  } ;

} // end namespace marlin 
#endif




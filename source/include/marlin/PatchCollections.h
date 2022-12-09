#ifndef PatchCollections_h
#define PatchCollections_h 1

#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "lcio.h"
#include <string>
#include <set>
#include <map>  // pair

using namespace lcio ;
using namespace marlin ;

namespace UTIL{
  class CheckCollections ;
}

/** Patch events with empty collections that are not present in all events.
 *  This could be useful, e.g. in context of conversion to EDM4hep, where every
 *  collection needs to be in every event in a given file.
 *
 *  NB:  run this processor before any other processor that is processing events.
 * 
 * @param  PatchCollections:   pairs of: CollectionName  CollectionType
 * 
 * @author F. Gaede, DESY
 * @date   Dec, 2022
 */
namespace marlin {

  class PatchCollections : public Processor {

    typedef std::set< std::pair< std::string, std::string > > SET ;

  public:

    virtual Processor*  newProcessor() { return new PatchCollections ; }

    PatchCollections() ;

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() ;

    /** Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /** Called for every event - the working horse.
     */
    virtual void processEvent( LCEvent * evt ) ;

    virtual void check( LCEvent * evt ) ;

//    virtual void modifyEvent( LCEvent *evt ) ;

    /** Called after data processing for clean up.
     */
    virtual void end() ;

    virtual const std::string & name() const { return Processor::name() ; }


  protected:

    std::vector<std::string> _colList{};
    SET _patchCols{};

    int _nRun=-1;
    int _nEvt=-1;
    UTIL::CheckCollections* _colCheck ;


  } ;

} // namespace marlin

#endif




#ifndef PatchCollections_h
#define PatchCollections_h 1

#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <utility>  // pair
#include <memory>

using namespace lcio ;
using namespace marlin ;

namespace UTIL{
  class CheckCollections ;
}

/** Patch events with empty collections that are not present in all events.
 *  This could be useful, e.g. in context of conversion to EDM4hep, where every
 *  collection needs to be in every event in a given file.
 *  There are two modes of how this can be used:
 *
 *  If @ParseInputFiles is true  all inputfiles are parsed for missing collections on init() and
 *                      then these are patched as needed in the processing - might cause some delay in program start
 *                      Collections in  @PatchCollections are ignored in this case
 *
 *  If @ParseInputFiles is false, all users specified collections in @PatchCollections are patched as empty
 *                      collections if needed - this will be faster, but requires the user to know which collections
 *                      to patch (e.g. from the LCIO tool `check_missing_collections`)
 *
 *  NB:  run this processor before any other processor that is processing events.
 * 
 * @param  ParseInputFiles:   if true, all inputfiles are parsed for missing collections on init
 * @param  PatchCollections:   pairs of: CollectionName  CollectionType
 * 
 * @author F. Gaede, DESY
 * @date   Dec, 2022
 */
namespace marlin {

  class PatchCollections : public Processor {

    typedef std::vector< std::pair< std::string, std::string > > Vector;

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

    StringVec _colList{};
    Vector _patchCols{};
    bool _parseFiles = false ;

    int _nRun=-1;
    int _nEvt=-1;
    std::unique_ptr<UTIL::CheckCollections> _colCheck{nullptr};

  } ;

} // namespace marlin

#endif




#ifdef MARLIN_USE_AIDA

#ifndef AIDAProcessor_h
#define AIDAProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"

namespace AIDA{
  class IAnalysisFactory ;
  class ITreeFactory ;
  class ITree ;
  class IHistogramFactory ;
  class ITupleFactory ;
} ;

class HistoManager ;

using namespace lcio ;

namespace marlin {


/** Module that creates an AIDA file and histogram and tuple factories that 
 * can be used in all processors of a Marlin application.
 */

class AIDAProcessor : public marlin::Processor {
  
 public:
  
  virtual Processor*  newProcessor() ;

  
  AIDAProcessor() ;

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


  /** Called after data processing for clean up.
   */
  virtual void end() ;


  /** Returns an AIDA histogram factory for the given processor with
   *  the current directory set to the processor's name.
   *  Call right before using the factory to create histograms.
   */
  static AIDA::IHistogramFactory* histogramFactory( const Processor* proc ) ;
  
  /** Returns an AIDA tuple factory for the given processor with
   *  the current directory set to the processor's name.
   *  Call right before using the factory to create tuples.
   */
  static AIDA::ITupleFactory* tupleFactory( const Processor* proc ) ;
  
  /** Returns an AIDA treefor the given processor with
   *  the current directory set to the processor's name.
   *  can be used to create subdirectories for a processor. 
   */
  static AIDA::ITree* tree( const Processor* proc ) ;
  
protected:

  AIDA::IAnalysisFactory* _analysisFactory ; 
  AIDA::ITreeFactory* _treeFactory ; 
  AIDA::ITree*  _tree ;
  AIDA::IHistogramFactory* _histoFactory ; 
  AIDA::ITupleFactory* _tupleFactory ; 

  std::string _fileType ;
  std::string _fileName ;
  int _compress ;
    
private:
  
  static AIDAProcessor* _me ;
} ;

} // end namespace marlin 
#endif

#endif




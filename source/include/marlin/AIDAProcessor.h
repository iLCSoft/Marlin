#ifndef AIDAProcessor_h
#define AIDAProcessor_h 1

#include "marlin/MarlinConfig.h"

#ifdef MARLIN_AIDA

#include "marlin/Processor.h"
#include "lcio.h"

namespace AIDA{
  class IAnalysisFactory ;
  class ITreeFactory ;
  class ITree ;
  class IHistogramFactory ;
  class ITupleFactory ;
  class IDataPointSetFactory ;
}

class HistoManager ;

using namespace lcio ;

namespace marlin {


/** Provides access to AIDA histograms, ntuples, etc.
 *  This module  creates an AIDA file and histogram and tuple factories that 
 *  can be used in all processors of a Marlin application.
 * 
 *  <h4>Input - Prerequisites</h4>
 *  No input needed for this processor. Make sure it is included in the list of active processors 
 *  before any processor that books/fills histograms.
 *
 *  <h4>Output</h4> 
 *  none
 * 
 * @param Compress  compression of output file 0: false >0: true (default) 
 * @param FileName  filename without extension
 * @param FileType  type of output file xml (default) or root ( only OpenScientist)
 *
 *  @author F. Gaede, DESY
 *  @version $Id: AIDAProcessor.h,v 1.7 2008-04-15 10:12:28 gaede Exp $ 
 */

class AIDAProcessor : public marlin::Processor {
  
 public:
  
  virtual Processor*  newProcessor() ;

  
  AIDAProcessor() ;
  AIDAProcessor(const marlin::AIDAProcessor&) = delete;
  AIDAProcessor& operator=(const marlin::AIDAProcessor&) = delete;

  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;

  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;

  /** nothing processed here
   */
   virtual void processEvent( LCEvent * evt ) ; 

   /** Fills plot with processing time
    */
   virtual void check( LCEvent * evt ) ; 


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
  
  /** Returns an AIDA dataPointSet factory for the given processor with
   *  the current directory set to the processor's name.
   *  Call right before using the factory to create dataPointSets.
   */
  static AIDA::IDataPointSetFactory* dataPointSetFactory( const Processor* proc ) ;

 /** Returns an AIDA analysis factory for the given processor with
   *  the current directory set to the processor's name.
   *  Call right before using the factory.
   */
  static AIDA::IAnalysisFactory* GetIAnalysisFactory( const Processor* proc );
  

protected:

  AIDA::IAnalysisFactory* _analysisFactory ; 
  AIDA::ITreeFactory* _treeFactory ; 
  AIDA::ITree*  _tree ;
  AIDA::IHistogramFactory* _histoFactory ; 
  AIDA::ITupleFactory* _tupleFactory ; 
  AIDA::IDataPointSetFactory* _dataPointSetFactory ; 

  std::string _fileType ;
  std::string _fileName ;
  int _compress ;
    
private:
  
  static AIDAProcessor* _me ;
} ;

} // end namespace marlin 

#endif // MARLIN_AIDA
#endif // AIDAProcessor_h


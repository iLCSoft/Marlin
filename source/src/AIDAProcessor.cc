#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP / MARLIN_AIDA

#ifdef MARLIN_AIDA

#include "marlin/AIDAProcessor.h"

#include <iostream>
#include <assert.h>
#include <time.h>

#include <AIDA/IAnalysisFactory.h>
#include <AIDA/ITreeFactory.h>
#include <AIDA/ITree.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ITupleFactory.h>
#include <AIDA/IDataPointSetFactory.h>

#include <AIDA/ICloud1D.h>


namespace marlin { 
  
  AIDAProcessor aAIDAProcessor ;
  
  
  AIDAProcessor::AIDAProcessor() : Processor("AIDAProcessor"),
  _analysisFactory(NULL),
  _treeFactory(NULL),
  _tree(NULL),
  _histoFactory(NULL),
  _tupleFactory(NULL),
  _dataPointSetFactory(NULL),
  _fileType(""),
  _fileName(""),
  _compress(1) {
    
    _description = "Processor that handles AIDA files. Creates on directory per processor. "
      " Processors only need to create and fill the histograms, clouds and tuples. Needs to be the first ActiveProcessor" ;

    registerProcessorParameter( "FileType" , 
				" type of output file root (default) or xml )"  ,
				_fileType ,
				std::string("root") ) ;
    
    registerProcessorParameter( "FileName" , 
				" filename without extension"  ,
				_fileName ,
				std::string("aida_file") ) ;
    
    registerProcessorParameter( "Compress" , 
				" compression of output file 0: false >0: true (default) "  ,
				_compress ,
				1 ) ;
    
  }
   
  AIDAProcessor* AIDAProcessor::_me ;

  /** AIDAProcessor needs to be a singleton.
   */
  Processor*  AIDAProcessor::newProcessor() { 
    if( ! _me )
      _me = new AIDAProcessor ;
    return _me ; 
  }
  
  void AIDAProcessor::init() { 
    
    
    printParameters() ;

    //--- steering parameters ---------
    
    bool compress = false ;
    if( _compress !=  0 )
      compress = true ;

    //-------------------------------------------------
    
    _analysisFactory = AIDA_createAnalysisFactory() ;  
    
    assert( _analysisFactory != 0 ) ;
    
    _treeFactory = _analysisFactory->createTreeFactory();
    
    assert( _treeFactory != 0 ) ;
    
    
    std::string sexport("root") ; // root or Lab  - only meaningful for _fileType==root
    
    std::string option("") ;
    
    option += compress ? "compress=yes" : "compress=no";
    
    if( _fileType == "root" ) {
      
      _fileName += ".root" ;
      option += std::string(";export=") + sexport;
      
    } else if( _fileType == "xml" ) {
      
      _fileName += ".aida";
    }
    
    _tree = _treeFactory->create( _fileName, _fileType, false, true , option );
    
    assert( _tree != 0 ) ;
    
    
    _histoFactory = _analysisFactory->createHistogramFactory( *_tree ) ;
    
    assert(  _histoFactory != 0 ) ; 
    
    _tupleFactory = _analysisFactory->createTupleFactory( *_tree ) ;
    
    assert(  _tupleFactory != 0 ) ; 

    _dataPointSetFactory = _analysisFactory->createDataPointSetFactory( *_tree ) ;
    
    assert(  _dataPointSetFactory != 0 ) ; 
    
  } 
  
  
  void AIDAProcessor::processRunHeader( LCRunHeader* ) { 
  } 
  

  void AIDAProcessor::processEvent( LCEvent* ) { 
    // no processing
  }

  void AIDAProcessor::check( LCEvent* ) { 
    
    static AIDA::ICloud1D* hEvtTime ;    
    static clock_t eventTime =  clock () ; 
    
    
    // create directory for this processor
    if( isFirstEvent() ) { 
      _tree->cd( "/" ) ;
      assert( _tree->mkdir( name()  ) ) ; 
      
      _tree->cd( "/" + name()  ) ;
      
      hEvtTime = _histoFactory->createCloud1D( "hEvtProcessingTime", "event processing time [s] ", 100 ) ; 
      
      _tree->cd("..") ;
      
      
    } else { 
      
      clock_t now =  clock () ; 
      
      hEvtTime->fill(  double(now - eventTime) / double(CLOCKS_PER_SEC) ) ;
      eventTime =  now ;
      
    } 
  }
  
  void AIDAProcessor::end(){ 
    
    
    _tree->commit() ;
    
    delete _treeFactory ;
    delete _analysisFactory ; 
    delete _histoFactory ;
    delete _tupleFactory ;
    delete _dataPointSetFactory ;
    
  }
   AIDA::IAnalysisFactory* AIDAProcessor::GetIAnalysisFactory( const Processor* proc )
   {
      tree( proc );
     return _me->_analysisFactory;
     
   }

  AIDA::IHistogramFactory* AIDAProcessor::histogramFactory( const Processor* proc ){
    tree( proc );
//     if( !_me->_tree->cd( "/" + proc->name() ) ) {
//       _me->_tree->cd("/") ;
//       _me->_tree->mkdir( proc->name() ) ; 
//       _me->_tree->cd(    "/" + proc->name() ) ;
//     }
    return  _me->_histoFactory ;
  }
  
  AIDA::ITupleFactory* AIDAProcessor::tupleFactory( const Processor* proc ) {
    tree( proc );
    return  _me->_tupleFactory ;
  }

  AIDA::IDataPointSetFactory* AIDAProcessor::dataPointSetFactory( const Processor* proc ) {
    tree( proc );
    return  _me->_dataPointSetFactory ;
  }
  
  AIDA::ITree* AIDAProcessor::tree( const Processor* proc ) {
    
    if( !_me ) {
      throw Exception(" AIDA is not properly initialized - you need to have the AIDAProcessor as"
		      " first processor in your execute section !" ) ; 
    }

    if( !_me->_tree->cd( "/" + proc->name() ) ) {
      _me->_tree->mkdir( "/" + proc->name() ) ; 
      _me->_tree->cd(    "/" + proc->name() ) ;
    }
    return _me->_tree ;
  }
  
} // namespace

#endif // MARLIN_AIDA


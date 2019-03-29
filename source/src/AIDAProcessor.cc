#include <marlin/MarlinConfig.h>

#ifdef MARLIN_AIDA

#include <marlin/AIDAProcessor.h>

// -- std headers
#include <iostream>
#include <assert.h>
#include <time.h>

// -- AIDA headers
#include <AIDA/IAnalysisFactory.h>
#include <AIDA/ITreeFactory.h>
#include <AIDA/ITree.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ITupleFactory.h>
#include <AIDA/IDataPointSetFactory.h>
#include <AIDA/ICloud1D.h>

namespace marlin { 
  
  AIDAProcessor aAIDAProcessor ;
  AIDAProcessor* AIDAProcessor::_me ;
  
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
  
  //--------------------------------------------------------------------------
   
  Processor* AIDAProcessor::newProcessor() { 
    if( ! _me )
      _me = new AIDAProcessor ;
    return _me ; 
  }
  
  //--------------------------------------------------------------------------
  
  void AIDAProcessor::init() { 
    printParameters() ;
    bool compress = false ;
    if( _compress !=  0 ) {
      compress = true ;      
    }
    
    _analysisFactory = AIDA_createAnalysisFactory() ;  
    assert( _analysisFactory != 0 ) ;
    
    _treeFactory = _analysisFactory->createTreeFactory();
    assert( _treeFactory != 0 ) ;
    
    // construct file name
    std::string sexport("root") ; // root or Lab  - only meaningful for _fileType==root    
    std::string option("") ;
    option += compress ? "compress=yes" : "compress=no";
    if( _fileType == "root" ) {
      _fileName += ".root" ;
      option += std::string(";export=") + sexport ;  
    } 
    else if( _fileType == "xml" ) {
      _fileName += ".aida" ;
    }
    
    _tree = _treeFactory->create( _fileName, _fileType, false, true , option );
    assert( _tree != 0 ) ;
    
    _histoFactory = _analysisFactory->createHistogramFactory( *_tree ) ;
    assert(  _histoFactory != 0 ) ; 
    
    _tupleFactory = _analysisFactory->createTupleFactory( *_tree ) ;
    assert(  _tupleFactory != 0 ) ; 

    _dataPointSetFactory = _analysisFactory->createDataPointSetFactory( *_tree ) ;
    assert(  _dataPointSetFactory != 0 ) ; 
    
    // book general plots
    tree( this ) ;
    _hEvtTime = _histoFactory->createCloud1D( "hEvtProcessingTime", "event processing time [s] ", 100 ) ; 
    _tree->cd("/") ;
  }
  
  //--------------------------------------------------------------------------

  void AIDAProcessor::check( LCEvent* ) {
    if ( isFirstEvent() ) {
      _eventTime = clock() ;
    }
    else {
      clock_t now = clock() ;
      _hEvtTime->fill(  double(now - _eventTime) / double(CLOCKS_PER_SEC) ) ;
      _eventTime =  now ;
    } 
  }
  
  //--------------------------------------------------------------------------
  
  void AIDAProcessor::end(){ 
    _tree->commit() ;
    delete _treeFactory ;
    delete _analysisFactory ; 
    delete _histoFactory ;
    delete _tupleFactory ;
    delete _dataPointSetFactory ;
    delete _tree ;
    delete _hEvtTime ;
  }
  
  //--------------------------------------------------------------------------
  
  AIDA::IAnalysisFactory* AIDAProcessor::GetIAnalysisFactory( const Processor* proc ) {
    tree( proc );
    return _me->_analysisFactory;
  }
  
  //--------------------------------------------------------------------------

  AIDA::IHistogramFactory* AIDAProcessor::histogramFactory( const Processor* proc ){
    tree( proc );
    return  _me->_histoFactory ;
  }
  
  //--------------------------------------------------------------------------
  
  AIDA::ITupleFactory* AIDAProcessor::tupleFactory( const Processor* proc ) {
    tree( proc );
    return  _me->_tupleFactory ;
  }
  
  //--------------------------------------------------------------------------

  AIDA::IDataPointSetFactory* AIDAProcessor::dataPointSetFactory( const Processor* proc ) {
    tree( proc );
    return  _me->_dataPointSetFactory ;
  }
  
  //--------------------------------------------------------------------------
  
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


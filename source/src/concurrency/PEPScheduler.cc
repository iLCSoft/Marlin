#include <marlin/concurrency/PEPScheduler.h>

// -- marlin headers
#include <marlin/Application.h>
#include <marlin/Utils.h>
#include <marlin/EventExtensions.h>
#include <marlin/ProcessorSequence.h>
#include <marlin/PluginManager.h>

// -- std headers
#include <exception>
#include <algorithm>
#include <iomanip>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  WorkerOutput struct
     *  Stores the output of a processor sequence call
     */
    struct WorkerOutput {
      ///< The input event
      std::shared_ptr<EVENT::LCEvent>     _event {nullptr} ;
      ///< An exception potential throw in the worker thread
      std::exception_ptr                  _exception {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  ProcessorSequenceWorker class
     */
    class ProcessorSequenceWorker : public WorkerBase<PEPScheduler::InputType,PEPScheduler::OutputType> {
    public:
      using Base = WorkerBase<PEPScheduler::InputType,PEPScheduler::OutputType>;
      using Input = Base::Input ;
      using Output = Base::Output ;

    public:
      ~ProcessorSequenceWorker() = default ;

    public:
      /**
       *  @brief  Constructor
       *
       *  @param  sequence the processor sequence to execute
       */
      ProcessorSequenceWorker( std::shared_ptr<ProcessorSequence> sequence ) ;

    private:
      // from WorkerBase<IN,OUT>
      Output process( Input && event ) ;

    private:
      ///< The processor sequence to run in the worker thread
      std::shared_ptr<ProcessorSequence>     _sequence {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    ProcessorSequenceWorker::ProcessorSequenceWorker( std::shared_ptr<ProcessorSequence> sequence ) :
      _sequence(sequence) {
      /* nop */
    }

    //--------------------------------------------------------------------------

    ProcessorSequenceWorker::Output ProcessorSequenceWorker::process( Input && event ) {
      Output output {} ;
      output._event = event ;
      try {
        _sequence->modifyEvent( event ) ;
        _sequence->processEvent( event ) ;
      }
      catch(...) {
        output._exception = std::current_exception() ;
      }
      return output ;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    void PEPScheduler::init( const Application &app ) {
      _logger = app.createLogger( "PEPScheduler" ) ;
      preConfigure( app ) ;
      configureProcessors( app ) ;
      configurePool( app ) ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::preConfigure( const Application &app ) {
      auto globals = app.globalParameters() ;
      auto allowModify = ( globals->getStringVal( "AllowToModifyEvent" ) == "true" ) ;
      // warning !!!
      if ( allowModify ) {
        _logger->log<WARNING>()
          << " ******************************************************************************* \n"
          << " *    AllowToModifyEvent is set to 'true'                                      * \n"
          << " *    => all processors can modify the input event in processEvent() !!        * \n"
          << " *        consider setting this flag to 'false'                                * \n"
          << " *        unless you really need it...                                         * \n"
          << " *    - if you need a processor that modifies the input event                  * \n"
          << " *      please implement the EventModifier interface and use the modifyEvent() * \n"
          << " *      method for this                                                        * \n"
          << " ******************************************************************************* \n"
          << std::endl ;
      }
      auto ccyStr = globals->getStringVal( "Concurrency" ) ;
      if ( ccyStr.empty() ) {
        ccyStr = "auto" ;
      }
      // The concurrency read from the steering file
      std::size_t ccy = (ccyStr == "auto" ?
        std::thread::hardware_concurrency() :
        StringUtil::stringToType<std::size_t>(ccyStr) ) ;
      _logger->log<DEBUG5>() << "-- Application concurrency from steering file " << ccy << std::endl ;
      _logger->log<DEBUG5>() << "-- Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl ;
      if ( ccy <= 0 ) {
        _logger->log<ERROR>() << "-- Couldn't determine number of threads to use (computed=" << ccy << ")" << std::endl ;
        throw Exception( "Undefined concurrency level" ) ;
      }
      _logger->log<MESSAGE>() << "-- Application concurrency set to " << ccy << std::endl ;
      if ( ccy > std::thread::hardware_concurrency() ) {
        _logger->log<WARNING>() << "-- Application concurrency higher than the number of supported threads on your machine --" << std::endl ;
        _logger->log<WARNING>() << "---- application: " << ccy << std::endl ;
        _logger->log<WARNING>() << "---- hardware:    " << std::thread::hardware_concurrency() << std::endl ;
      }
      if ( ccy == 1 ) {
        _logger->log<WARNING>() << "-- The program will run on a single thread --" << std::endl ;
        // TODO should we throw here ??
      }
      // create processor sequences
      for( std::size_t i = 0 ; i<ccy ; ++i ) {
        _sequences.push_back( std::make_shared<ProcessorSequence>() ) ;
      }
      auto activeProcessors = app.activeProcessors() ;
      auto processorConditions = app.processorConditions() ;
      const bool useConditions = ( activeProcessors.size() == processorConditions.size() ) ;
      if( useConditions ) {
        for( std::size_t i=0 ; i<activeProcessors.size() ; ++i ) {
          _conditions[ activeProcessors[i] ] = processorConditions[i] ;
        }
      }
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::configureProcessors( const Application &app )  {
      // TODO for the time being, all processors in the sequences
      // are cloned and not locked. Need to find a proper config
      // mechanism to deal with processor instance sharing among
      // sequences and how to lock properly processing for certain
      // processors
      _logger->log<DEBUG5>() << "PEPScheduler configureProcessors ..." << std::endl ;
      // create list of active processors
      auto activeProcessors = app.activeProcessors() ;
      if ( activeProcessors.empty() ) {
        throw Exception( "PEPScheduler::configureProcessors: Active processor list is empty !" ) ;
      }
      // check for duplicates first
      std::set<std::string> duplicateCheck ( activeProcessors.begin() , activeProcessors.end() ) ;
      if ( duplicateCheck.size() != activeProcessors.size() ) {
        _logger->log<ERROR>() << "PEPScheduler::configureProcessors: the following list of active processors are found to be duplicated :" << std::endl ;
        for ( auto procName : activeProcessors ) {
          auto c = std::count( activeProcessors.begin() , activeProcessors.end() , procName ) ;
          if( c > 1 ) {
            _logger->log<ERROR>() << "   * " << procName << " (" << c << " instances)" << std::endl ;
          }
        }
        throw Exception( "PEPScheduler::configureProcessors: duplicated active processors. Check your steering file !" ) ;
      }
      auto &pluginMgr = PluginManager::instance() ;
      // populate processor sequences
      for ( size_t i=0 ; i<activeProcessors.size() ; ++i ) {
        auto procName = activeProcessors[ i ] ;
        _logger->log<DEBUG5>() << "Active processor " << procName << std::endl ;
        auto processorParameters = app.processorParameters( procName ) ;
        if ( nullptr == processorParameters ) {
          throw Exception( "PEPScheduler::configureProcessors: undefined processor '" + procName + "'" ) ;
        }
        auto procType = processorParameters->getStringVal("ProcessorType") ;
        // fill sequences. Create one processor each sequence for the moment
        for( auto sequence : _sequences ) {
          auto processor = pluginMgr.create<Processor>( PluginType::Processor, procType ) ;
          processor->setName( procName ) ;
          if ( nullptr == processor ) {
            throw Exception( "PEPScheduler::configureProcessors: processor '" + procType + "' not registered !" ) ;
          }
          processor->setParameters( processorParameters ) ;
          sequence->add( processor ) ;
          // FIXME: all processor registers to random seed manager
          // for the time being. Need a registration mechanism for processors
          _rdmSeedMgr.addEntry( processor.get() ) ;
          _allProcessors.insert( processor ) ;
        }
      }
      // initialize processors
      for ( auto processor : _allProcessors ) {
        _logger->log<DEBUG5>() << "Init processor " << processor->name() << std::endl ;
        processor->baseInit( app ) ;
      }
      _logger->log<DEBUG5>() << "PEPScheduler configureProcessors ... DONE" << std::endl ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::configurePool( const Application &app ) {
      // create N workers for N processor sequences
      for( auto sequence : _sequences ) {
        _pool.addWorker<ProcessorSequenceWorker>( sequence ) ;
      }
      _pool.start() ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::pushEvent( std::shared_ptr<EVENT::LCEvent> event ) {
      // TODO deal with event runtime extensions here
      // ...
      // TODO need to deal with futures here
      auto f = _pool.push( WorkerPool::PushPolicy::ThrowIfFull, event ) ;
    }

    //--------------------------------------------------------------------------

    std::size_t PEPScheduler::freeSlots() const {
      return _pool.freeSlots() :
    }

  }

} // namespace marlin

#include "marlin/MarlinMTApplication.h"

// -- marlin headers
#include <marlin/Utils.h>
#include <marlin/Scheduler.h>

// -- std headers
#include <thread>
#include <algorithm>

// -- lcio headers
#include <MT/LCReader.h>

namespace marlin {

  namespace detail {
    /**
     *  @brief  MarlinMTLCListener class
     *  Implement LCEvent and LCRunHeader listeners to use
     *  in combination with MT::LCReader class.
     *  Forward calls to the application scheduler
     */
    class MarlinMTLCListener : public MT::LCReaderListener {
    public:
      /**
       *  @brief  Constructor
       *
       *  @param  scheduler the scheduler to forward calls
       */
      MarlinMTLCListener( Scheduler &scheduler ) :
        _scheduler(scheduler) {
        /* nop */
      }

      void processEvent( MT::LCEventPtr event ) {
        _scheduler.processEvent( event.get() ) ;
      }

      void modifyEvent( MT::LCEventPtr event ) {
        _scheduler.modifyEvent( event.get() ) ;
      }

      void processRunHeader( MT::LCRunHeaderPtr hdr ) {
        _scheduler.processRunHeader( hdr.get() ) ;
      }

      void modifyRunHeader( MT::LCRunHeaderPtr hdr ) {
        _scheduler.modifyRunHeader( hdr.get() ) ;
      }

    private:
      /// The application scheduler
      Scheduler          &_scheduler ;
    };
  }



  class Task {
    friend class MarlinMTApplication ;
  public:
    using Logger = Logging::Logger ;

  public:
    Task(const Task &) = delete ;
    Task &operator=(const Task &) = delete ;

    Task( unsigned int tid ) :
      _id(tid) {

    }

    void init( MarlinMTApplication *app ) {
      _application = app ;
      _logger = _application->createLogger( "ThreadTask" + StringUtil::typeToString( id() ) ) ;
      _scheduler.init( app ) ;
      auto globals = _application->globalParameters() ;
      _maxRecord = globals->getValue<int>( "MaxRecordNumber" ) ;
      _skipNEvents = globals->getValue<int>( "SkipNEvents" ) ;
      EVENT::StringVec readCollectionNames {} ;
      readCollectionNames = globals->getValues<std::string>( "LCIOReadCollectionNames" ) ;
      if ( not readCollectionNames.empty() ) {
        _logger->log<WARNING>()
          << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** "
          << std::endl ;
        for( auto collection : readCollectionNames ) {
    	    _logger->log<WARNING>()  << "     " << collection << std::endl ;
    	  }
    	  _logger->log<WARNING>()
          << " *************************************************************************************************** " << std::endl ;
        _lcReader.setReadCollectionNames( readCollectionNames ) ;
      }
      _lcReader.setReadCollectionNames( readCollectionNames ) ;
    }

    void setLCIOInputFiles( const EVENT::StringVec &lcioInputFiles ) {
      _lcioInputFiles = lcioInputFiles ;
    }

    void start() {
      _stopFlag = false ;
      _thread = std::thread( &Task::threadFunction, this ) ;
    }

    void stop() {
      _stopFlag = true ;
    }

    unsigned int id() const {
      return _id ;
    }

    void join() {
      if( _thread.joinable() ) {
        _thread.join() ;
      }
    }

    const Scheduler &scheduler() const {
      return _scheduler ;
    }

    void end() {
      if ( _finishedProperly ) {
        _scheduler.end() ;
      }
    }

  private:
    void threadFunction() {
      _logger->log<MESSAGE9>() << "Starting thread task (id=" << id() << "), thread id " << _thread.get_id() << std::endl ;
      try {
        detail::MarlinMTLCListener listener(_scheduler) ;
        _lcReader.open( _lcioInputFiles ) ;
        // skip events if requested
        if ( _skipNEvents > 0 ) {
          _logger->log<WARNING>() << " --- Will skip first " << _skipNEvents << " event(s)" << std::endl ;
          _lcReader.skipNEvents( _skipNEvents ) ;
        }
        try {
          MT::LCReaderListenerList listeners = {&listener} ;
          if( _maxRecord > 0 ){
            try {
              _lcReader.readStream( listeners, _maxRecord ) ;
            }
            catch( IO::EndOfDataException &e ) {
              _logger->log<WARNING>() << e.what() << std::endl ;
            }
          }
          else {
            _lcReader.readStream( listeners ) ;
          }
        }
        catch( StopProcessingException &e ) {
          _logger->log<ERROR>() << std::endl
              << " **********************************************************" << std::endl
              << " *                                                        *" << std::endl
              << " *   Stop of EventProcessing requested by processor :     *" << std::endl
              << " *                  "  << e.what()                           << std::endl
              << " *     will call end() method of all processors !         *" << std::endl
              << " *                                                        *" << std::endl
              << " **********************************************************" << std::endl
              << std::endl ;
        }
        catch( RewindDataFilesException &e ) {
          _logger->log<WARNING>() << "Files rewind functionality is not available in MarlinMT" << std::endl ;
          _logger->log<WARNING>() << "Application will terminate normally now ..." << std::endl ;
        }
        _finishedProperly = true ;
      }
      // uncaught exceptions at this point means that the
      // task thread should exit with error
      catch( ... ) {
        prepareForExit( true ) ;
        _logger->log<ERROR>() << "Exiting thread task (id=" << id() << "), thread id " << _thread.get_id() << std::endl ;
        return ;
      }
      _logger->log<MESSAGE9>() << "Exiting thread task (id=" << id() << "), thread id " << _thread.get_id() << std::endl ;
      prepareForExit( false ) ;
    }

    void prepareForExit( bool error ) {
      // an error has been raised by another task.
      // The main program will exit in any case, so just return ..
      if ( _application->_taskErrorFlag ) {
        return ;
      }
      {
        // lock and prepare the application the application
        // to exit, with or without error
        std::lock_guard<std::mutex> lock( _application->_mutex ) ;
        _application->_nRunningTasks--;
        if ( error ) {
          _application->_taskErrorFlag = true ;
          auto taskError = std::make_shared<TaskError>() ;
          taskError->_threadId = _thread.get_id() ;
          taskError->_taskId = id() ;
          auto currentException = std::current_exception() ;
          if ( nullptr != currentException ) {
            taskError->_exception = currentException ;
          }
          _application->_taskError = taskError ;
          _logger->log<DEBUG7>() << "Task " << id() << ", thread " << _thread.get_id() << ", set error flag !" << std::endl ;
        }
      }
      _application->_taskConditionVariable.notify_one() ;
    }

  private:
    unsigned int           _id {0} ;
    Scheduler              _scheduler {} ;
    std::thread            _thread {} ;
    MarlinMTApplication   *_application {nullptr} ;
    std::atomic_bool       _stopFlag {true} ;
    Logger                 _logger {nullptr} ;
    EVENT::StringVec       _lcioInputFiles {} ;
    int                    _maxRecord {0} ;
    int                    _skipNEvents {0} ;
    MT::LCReader           _lcReader {MT::LCReader::directAccess} ;
    bool                   _finishedProperly {false} ;
  };




  void MarlinMTApplication::runApplication() {
    // This application should have the possibility to run in
    // single threaded mode without having to create a new task thread
    // if ( _concurrency == 1 ) {
    //   // TODO implement this !
    // }
    // else {
      clock_t startTime = clock() ;
      for ( auto task : _tasks ) {
        _nRunningTasks ++ ;
        task->start() ;
      }
      // wait for task to finish or throw errors
      std::unique_lock<std::mutex> lock( _mutex ) ;
      _taskConditionVariable.wait(lock, [this] {
        // if all tasks finished, exit the application
        if ( _nRunningTasks == 0 ) {
          return true ;
        }
        auto flag = _taskErrorFlag.load() ;
        // std::cout << "Returning flag " << flag << std::endl ;
        return flag ;
      }) ;
      logger()->log<MESSAGE>() << "Terminating application ..." << std::endl ;
      // std::this_thread::sleep_for( std::chrono::seconds(1) ) ;
      // one of the task terminated with an error
      if ( _taskErrorFlag ) {
        // stop all tasks and join them all
        joinTasks( true ) ;
        // handle the error
        if ( nullptr != _taskError ) {
          logger()->log<ERROR>() << "---------------------------------------------------" << std::endl ;
          logger()->log<ERROR>() << "-- One of the task thread finished with error(s) --" << std::endl ;
          logger()->log<ERROR>() << "--- Thread id: " << _taskError->_threadId << std::endl ;
          logger()->log<ERROR>() << "--- Task id:   " << _taskError->_taskId << std::endl ;
          logger()->log<ERROR>() << "---------------------------------------------------" << std::endl ;
          // Rethrow the caught exception if any
          if ( nullptr != _taskError->_exception ) {
            std::rethrow_exception( _taskError->_exception ) ;
          }
        }
        logger()->log<ERROR>() << "One of the task thread finished with error(s)" << std::endl ;
        throw Exception( "One of the task thread finished with error(s)" ) ;
      }
      // normal exit
      else {
        joinTasks( false ) ;
        for ( auto task : _tasks ) {
          task->end() ;
        }
        clock_t endTime = clock() ;
        logger()->log<MESSAGE>() << "---------------------------------------------------" << std::endl ;
        logger()->log<MESSAGE>() << "-- " << program() << " threading summary" << std::endl ;
        const double parallelTime = static_cast<double>( endTime - startTime ) / static_cast<double>(CLOCKS_PER_SEC) ;
        double serialTime {0.0} ;
        for ( auto task : _tasks ) {
          auto summary = task->scheduler().generateTimeSummary() ;
          serialTime += summary._processingTime ;
        }
        const double speedup = serialTime / parallelTime ;
        const double scalingDeviation = ( fabs( speedup - _concurrency ) / _concurrency ) * 100. ;
        logger()->log<MESSAGE>() << "-- N threads:                      " << _concurrency << std::endl ;
        logger()->log<MESSAGE>() << "-- Speedup (serial/parallel):      " << serialTime << " / " << parallelTime << " = " << speedup << std::endl ;
        logger()->log<MESSAGE>() << "-- Deviation from perfect scaling: " << scalingDeviation << " " << '%' << std::endl ;
        logger()->log<MESSAGE>() << "---------------------------------------------------" << std::endl ;
      }
    // }
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::init() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    // get settings from global section
    auto globals = globalParameters() ;
    EVENT::StringVec lcioInputFiles = globals->getValues<std::string>( "LCIOInputFiles" ) ;
    if ( lcioInputFiles.empty() ) {
      logger()->log<ERROR>() << "No LCIO iput files found in </global> section of steering file" << std::endl ;
      throw Exception( "No LCIO iput files found in </global> section of steering file" ) ;
    }
    auto allowModify = ( globals->getValue<std::string>( "AllowToModifyEvent" ) == "true" ) ;
    // warning !!!
    if ( allowModify ) {
      logger()->log<WARNING>()
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
    auto ccyStr = globals->getValue<std::string>( "Concurrency" ) ;
    if ( ccyStr.empty() ) {
      ccyStr = "auto" ;
    }
    // The concurrency read from the steering file
    unsigned int ccySteer = (ccyStr == "auto" ? std::thread::hardware_concurrency() : StringUtil::stringToType<unsigned int>(ccyStr) ) ;
    logger()->log<DEBUG5>() << "-- Application concurrency from steering file " << ccySteer << std::endl ;
    logger()->log<DEBUG5>() << "-- Number of input files: " << lcioInputFiles.size() << std::endl ;
    logger()->log<DEBUG5>() << "-- Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl ;
    _concurrency = ( lcioInputFiles.size() <= ccySteer ? lcioInputFiles.size() : ccySteer ) ;
    if ( _concurrency <= 0 ) {
      logger()->log<ERROR>() << "-- Couldn't determine number of threads to use (computed=" << _concurrency << ")" << std::endl ;
      throw Exception( "Undefined concurrency for MarlinMT application" ) ;
    }
    logger()->log<MESSAGE>() << "-- Application concurrency set to " << _concurrency << std::endl ;
    if ( _concurrency > std::thread::hardware_concurrency() ) {
      logger()->log<WARNING>() << "-- Application concurrency higher than the number of supported threads on your machine --" << std::endl ;
      logger()->log<WARNING>() << "---- application: " << _concurrency << std::endl ;
      logger()->log<WARNING>() << "---- hardware:    " << std::thread::hardware_concurrency() << std::endl ;
    }
    if ( _concurrency == 1 ) {
      logger()->log<WARNING>() << "-- The program will run on a single thread --" << std::endl ;
    }
    else {
      logger()->log<MESSAGE>() << "-- Parallelization strategy:" << std::endl ;
      unsigned int index = 0 ;
      unsigned int nFilesPerThread = lcioInputFiles.size() / _concurrency ;
      unsigned int nRemainingFiles = lcioInputFiles.size() % _concurrency ;
      logger()->log<DEBUG5>() << "-- N files per thread:  " << nFilesPerThread << std::endl ;
      logger()->log<DEBUG5>() << "-- N remaining file(s): " << nRemainingFiles << std::endl ;
      for ( unsigned int th=0 ; th<_concurrency ; th++ ) {
        unsigned int nfiles = nFilesPerThread ;
        if( th < nRemainingFiles ) {
          nfiles++;
        }
        logger()->log<MESSAGE>() << "-- Thread " << th << " will process the following " << nfiles << " file(s):" << std::endl ;
        EVENT::StringVec threadLcioInputFiles {} ;
        for (unsigned int i = 0 ; i<nfiles ; i++ ) {
          logger()->log<MESSAGE>() << "--    " << lcioInputFiles[ index ] << std::endl ;
          threadLcioInputFiles.push_back( lcioInputFiles[ index ] ) ;
          index++;
        }
        auto task = std::make_shared<Task>( th ) ;
        task->setLCIOInputFiles( threadLcioInputFiles ) ;
        _tasks.push_back( task ) ;
      }
      // initialize tasks
      for ( auto task : _tasks ) {
        task->init( this ) ;
      }
    }
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::printUsage() const {

  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::joinTasks( bool stopTasks ) {
    if ( stopTasks ) {
      logger()->log<WARNING>() << "Stopping all task threads !" << std::endl ;
      for ( auto task : _tasks ) {
        task->stop() ;
      }
    }
    logger()->log<DEBUG7>() << "Joining all task threads !" << std::endl ;
    for ( auto task : _tasks ) {
      logger()->log<DEBUG5>() << "Joining task thread " << task->id() << std::endl ;
      task->join() ;
    }
  }

} // namespace marlin

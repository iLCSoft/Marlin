#ifndef MARLIN_CONCURRENCY_PEPSCHEDULER_h
#define MARLIN_CONCURRENCY_PEPSCHEDULER_h 1

// -- marlin headers
#include <marlin/IScheduler.h>
#include <marlin/Logging.h>
#include <marlin/Utils.h>
#include <marlin/concurrency/ThreadPool.h>

// -- std headers
#include <unordered_set>

namespace marlin {

  class SuperSequence ;
  class Sequence ;

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
     *  @brief  PEPScheduler class
     *  Parallel Event Processing Scheduler.
     *  Implements the scheduling of parallel inter-event processing.
     *
     *  A set of N worker threads are allocated at startup within a thread pool.
     *  Every time a new event is pushed in the scheduler, the event is queued
     *  in the thread pool for further processing. Note that this operation can
     *  fail if the thread pool queue is full. Use freeSlots() to know how many
     *  slots are free in the thread pool queue and avoid unexpected exceptions.
     */
    class PEPScheduler : public IScheduler {
    public:
      using ConditionsMap = std::map<std::string, std::string> ;
      using InputType = std::shared_ptr<EVENT::LCEvent> ;
      using OutputType = WorkerOutput ;
      using WorkerPool = ThreadPool<InputType,OutputType> ;
      using Logger = Logging::Logger ;
      using ProcessorSequence = std::shared_ptr<SuperSequence> ;
      using PushResultList = std::vector<WorkerPool::PushResult> ;
      using EventList = std::vector<std::shared_ptr<EVENT::LCEvent>> ;
      using Clock = std::chrono::steady_clock ;
      using TimePoint = std::chrono::steady_clock::time_point ;
      

    public:
      PEPScheduler() = default ;

      // from IScheduler interface
      void init( Application *app ) override ;
      void end() override ;
      void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
      void pushEvent( std::shared_ptr<EVENT::LCEvent> event ) override ;
      void popFinishedEvents( std::vector<std::shared_ptr<EVENT::LCEvent>> &events ) override ;
      std::size_t freeSlots() const override ;

    private:
      void preConfigure( Application *app ) ;
      void configureProcessors( Application *app ) ;
      void configurePool() ;

    private:
      ///< The worker thread pool
      WorkerPool                       _pool {} ;
      ///< The logger instance
      Logger                           _logger {nullptr} ;
      ///< The processor super sequence
      ProcessorSequence                _superSequence {nullptr} ;
      ///< The list of worker output promises
      PushResultList                   _pushResults {} ;
      ///< The start time (end of init() function)
      clock::time_point                _startTime {} ;
      ///< The end time (start of end() function)
      clock::time_point                _endTime {} ;
    };

  }

} // end namespace marlin

#endif

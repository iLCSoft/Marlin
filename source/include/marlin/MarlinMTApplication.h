#ifndef MARLIN_MARLINMTAPPLICATION_h
#define MARLIN_MARLINMTAPPLICATION_h 1

// -- marlin headers
#include "marlin/Application.h"

// -- lcio headers
#include "LCIOSTLTypes.h"

// -- std headers
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace marlin {
  
  class Task ;
  using TaskPtr = std::shared_ptr<Task> ;
  using TaskList = std::vector<TaskPtr> ;
  
  /**
   *  @brief  TaskError struct
   *  An error summary of task running in a separate thread
   */
  struct TaskError {
    /// The thread id 
    std::thread::id        _threadId {0} ;
    /// The task id 
    unsigned int           _taskId {0} ;
    /// A possibly caught exception during processing
    std::exception_ptr     _exception {nullptr} ;
  };
  
  using TaskErrorPtr = std::shared_ptr<TaskError> ;

  /**
   *  @brief  MarlinMTApplication class
   *  Implementation of parallel Marlin processing (multi threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run N processor chains in N threads. The number of threads 
   *  is either automatically by the OS, by the number of input files or set 
   *  manually in the steering file or command arguments (-j N).
   *  Each thread is processing a full processor chain with its own LCReader
   */
  class MarlinMTApplication : public Application {
    friend class Task ;
  public:
    MarlinMTApplication() = default ;
    
  private:
    void runApplication() ;
    void init() ;
    void end() {}
    void printUsage() const ;
    
  private:
    void joinTasks( bool stopTasks ) ;
    
  private:
    /// The concurrency of MT application 
    unsigned int             _concurrency {0} ;
    ///
    TaskList                 _tasks {} ;
    ///
    std::atomic_int          _nRunningTasks {0} ;
    std::atomic_bool         _taskErrorFlag {false} ;
    std::mutex               _mutex {} ;
    std::condition_variable  _taskConditionVariable {} ;
    TaskErrorPtr             _taskError {nullptr} ;
  };

} // end namespace marlin

#endif

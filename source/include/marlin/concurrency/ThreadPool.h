#ifndef MARLIN_CONCURRENCY_THREADPOOL_h
#define MARLIN_CONCURRENCY_THREADPOOL_h 1

// -- std headers
#include <functional>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <utility>
#include <memory>
#include <future>
#include <deque>
#include <condition_variable>

// -- marlin headers
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  ThreadPool class
     *  Implement a thread pool pattern.
     */
    class ThreadPool {
    public:
      /**
       *  @brief  Options struct
       *  Holds the input thread pool options 
       */
      struct Options {
        /// The number of workers in the thread pool
        unsigned int      _nThreads {std::thread::hardware_concurrency()} ;
        /// Whether to wait for a free worker when enqueuing a new task
        bool              _waitForFreeWorker {true} ;
      };
      
    private:
      /**
       *  @brief  Worker class
       *  Implement the thread pool task main loop
       */
      class Worker {
      public:
        friend class ThreadPool ;
        typedef std::function<void()> Function ;
        
      public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pool the parent thread pool
         */
        Worker( ThreadPool &pool ) ;
        
        /**
         *  @brief  Get the worker thread id
         */
        std::thread::id threadId() const ;
        
      private:
        /**
         *  @brief  The worker main loop
         *  Implement the thread pool task main loop
         */
        void loop() ;
        
        /**
         *  @brief  Join the worker thread
         */
        void join() ;
        
      private:
        /// The parent thread pool
        ThreadPool          &_pool ;
        /// The worker thread
        std::thread          _thread {} ;
      };
      
    public:
      typedef std::vector<std::shared_ptr<Worker>>    Workers ;
      typedef std::deque<Worker::Function>            TaskQueue ;
            
    public:
      /**
       *  @brief  Constructor
       *
       *  @param  options the thread pool options
       */
      ThreadPool( const Options &options ) ;
      
      /**
       *  @brief  Destructor
       */
      ~ThreadPool() ;
      
      /**
       *  @brief  Initialize the thread pool (create and start worker threads)
       */
      void init() ;
      
      /**
       *  @brief  Clean the thread pool (stop worker threads)
       *
       *  @param  dropPending whether to drop the pending task and terminate directly
       */
      void terminate( bool dropPending = true ) ;
      
      /**
       *  @brief  Submit a new task to the pool
       *  
       *  @param  func any callable object to execute
       *  @param  args the arguments to pass to the callable object
       *  @return a future containing the result of the task to execute
       */
      template <typename Function, typename ...Args>
      auto submit( Function&& func, Args&& ...args ) -> std::future<decltype(func(args...))> ;
      
      /**
       *  @brief  Get the number of pending tasks
       */
      std::size_t nPendingTasks() const ;
      
      /**
       *  @brief  Get the number of workers actually dealing with a task
       */
      std::size_t nRunningTasks() const ;
      
      /**
       *  @brief  Get the pool size (number of worker threads)
       */
      std::size_t size() const ;
      
      /**
       *  @brief  Whether the thread pool has been initialized
       */
      bool initialized() const ;
      
    private:
      /// Whether the thread pool has been initialized
      std::atomic_bool          _initialized {false} ;
      /// The shutdown flag to stop worker thread on termination
      std::atomic_bool          _shutdown {false} ;
      /// The thread workers
      Workers                   _workers {} ;
      /// The queue of pending tasks to execute
      TaskQueue                 _pendingTasks {} ;
      /// The pending task condition variable
      std::condition_variable   _queueCondition {} ;
      /// The thread pool access mutex
      mutable std::mutex        _queueMutex {} ;
      /// The number of running tasks
      std::atomic<std::size_t>  _nRunningTasks {0} ;
      /// The thread pool options
      Options                   _options {} ;
    };
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    template <typename Function, typename ...Args>
    inline auto ThreadPool::submit( Function&& func, Args&& ...args ) -> std::future<decltype(func(args...))> {
      if ( not _initialized ) {
        throw Exception( "ThreadPool::submit: thread pool not started, can't accept any task submission!" ) ;
      }
      // Create a function with bounded parameters ready to execute
      std::function<decltype(func(args...))()> localFunc = std::bind( std::forward<Function>(func), std::forward<Args>(args)... ) ;
      // Encapsulate it into a shared ptr in order to be able to copy construct / assign 
      auto task_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()>>( localFunc ) ;
      // Wrap packaged task into void function
      Worker::Function wrapper_func = [task_ptr]() {
        (*task_ptr)() ;
      } ;
      // Wait for a free worker if set in the options
      if ( _options._waitForFreeWorker ) {
        while ( _nRunningTasks.load() == _workers.size() ) {
          std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
        }
      }
      {
        // Enqueue generic wrapper function
        // Wake up one thread if its waiting
        std::lock_guard<std::mutex> lock( _queueMutex ) ;
        _pendingTasks.push_back( wrapper_func ) ;
        _queueCondition.notify_one() ;
      }
      // Return future from promise
      return task_ptr->get_future() ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

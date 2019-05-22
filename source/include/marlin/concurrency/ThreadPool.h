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
#include <condition_variable>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/concurrency/Queue.h"
#include "marlin/concurrency/QueueElement.h"

namespace marlin {

  namespace concurrency {

    template <typename IN, typename OUT>
    class Worker ;

    /**
     *  @brief  ThreadPool class
     *  The template parameter T is the type of data to enqueue and process
     *  in worker threads
     */
    template <typename IN, typename OUT>
    class ThreadPool {
    public:
      using QueueType = Queue<QueueElement<IN,OUT>> ;
      using PoolType = std::vector<std::shared_ptr<Worker<IN,OUT>>> ;
      using Promise = std::shared_ptr<std::promise<OUT>> ;
      using Future = std::future<OUT> ;
      using PushResult = std::pair<Promise,Future> ;
      friend class Worker<IN,OUT> ;

    public:
      /**
       *  @brief  PushPolicy enumerator
       */
      enum class PushPolicy {
        Blocking,      ///< Block until a slot is free in the queue
        ThrowIfFull    ///< Throw an exception if the queue is full
      };

    public:
      ThreadPool() = default ;
      ThreadPool(const ThreadPool &) = delete ;
      ThreadPool(ThreadPool &&) = delete ;
      ThreadPool& operator=(const ThreadPool &) = delete ;
      ThreadPool& operator=(ThreadPool &&) = delete ;

      /**
       *  @brief  Destructor
       */
      ~ThreadPool() ;

      /**
       *  @brief  Add a new worker thread
       *
       *  @param  args arguments to pass to worker constructor
       */
      template <typename WORKER, typename ...Args>
      void addWorker(Args &&...args) ;

      /**
       *  @brief  Start the worker threads
       */
      void start() ;

      /**
       *  @brief  Get the thread pool size
       */
      std::size_t size() const ;

      /**
       *  @brief  Get the number of waiting threads
       */
      std::size_t nWaiting() const ;

      /**
       *  @brief  Get the number of threads currently handling a task
       */
      std::size_t nRunning() const ;

      /**
       *  @brief  Get the number of free slots in the task queue
       */
      std::size_t freeSlots() const ;
      
      /**
       *  @brief  Whether the queue is empty
       */
      bool isQueueEmpty() const ;

      /**
       *  @brief  Clear the queue
       */
      void clearQueue() ;

      /**
       *  @brief  Set the maximum queue size
       *
       *  @param  maxQueueSize the maximum queue size
       */
      void setMaxQueueSize( std::size_t maxQueueSize ) ;
      
      /**
       *  @brief  Set whether the thread pool accept data push
       *  
       *  @param  accept whether to accept data push
       */
      void setAcceptPush( bool accept ) ;
      
      /**
       *  @brief  Whether the thread pool accepts data push
       */
      bool acceptPush() const ;
      
      /**
       *  @brief  Whether the thread pool is active, meaning that
       *  the queue is not empty or at least one worker is active
       */
      bool active() const ;

      /**
       *  @brief  Stop the thread pool.
       *  If the flag clear is set to true, the task queue is cleared.
       *  The threads are joined and the pool is clear. As no
       *  threads remains in the pool, the pool is not reusable.
       *  Thus this method must be called for performing a proper
       *  program termination before exiting
       *
       *  @param  clear whether the task queue should be cleared
       */
      void stop( bool clear = true ) ;

      /**
       *  @brief  Push a new task in the task queue.
       *  See PushPolicy for runtime behavior of enqueuing.
       *
       *  @param  policy the push policy
       *  @param
       */
      template <class = typename std::enable_if<not std::is_same<IN,void>::value>::type>
      PushResult push( PushPolicy policy, IN && input ) ;

    private:
      ///< The synchronization mutex
      std::mutex               _mutex {} ;
      ///< The queue enqueuing condition variable
      std::condition_variable  _conditionVariable {} ;
      ///< The actual thread pool
      PoolType                 _pool {} ;
      ///< The input element queue
      QueueType                _queue {} ;
      ///< Runtime flag...
      std::atomic<bool>        _isDone {false} ;
      ///< The thread pool stop flag
      std::atomic<bool>        _isStop {false} ;
      ///< Whether the thread pool is running
      std::atomic<bool>        _isRunning {false} ;
      ///< Whether the thread pool accepts push action
      std::atomic<bool>        _acceptPush {true} ;
    };

  }

}

// template implementation
#include <marlin/concurrency/Worker.h>

namespace marlin {

  namespace concurrency {

    template <typename IN, typename OUT>
    inline ThreadPool<IN,OUT>::~ThreadPool() {
      stop(true) ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    template <typename WORKER, typename ...Args>
    inline void ThreadPool<IN,OUT>::addWorker(Args &&...args) {
      if( _isRunning ) {
        throw Exception( "ThreadPool::addWorker: thread pool is running, can't add a worker!" ) ;
      }
      std::unique_ptr<WORKER> impl( new WORKER(args...) ) ;
      auto worker = std::make_shared<Worker<IN,OUT>>( *this, std::move( impl ) ) ;
      _pool.push_back( worker ) ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void ThreadPool<IN,OUT>::start() {
      if( _isRunning ) {
        throw Exception( "ThreadPool::start: already running!" ) ;
      }
      // Start the worker threads
      for (size_t i=0 ; i<_pool.size() ; i++) {
        _pool[i]->start() ;
      }
      _isRunning = true ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline std::size_t ThreadPool<IN,OUT>::size() const {
      return _pool.size() ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline std::size_t ThreadPool<IN,OUT>::nWaiting() const {
      std::size_t count = 0 ;
      for( auto &worker : _pool ) {
        if( worker->waiting() ) {
          ++count ;
        }
      }
      return count ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline std::size_t ThreadPool<IN,OUT>::nRunning() const {
      return ( _pool.size() - nWaiting() ) ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline std::size_t ThreadPool<IN,OUT>::freeSlots() const {
      return _queue.freeSlots() ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline bool ThreadPool<IN,OUT>::isQueueEmpty() const {
      return _queue.empty() ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline void ThreadPool<IN,OUT>::setMaxQueueSize( std::size_t maxQueueSize ) {
      _queue.setMaxSize( maxQueueSize ) ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void ThreadPool<IN,OUT>::clearQueue() {
      _queue.clear() ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline void ThreadPool<IN,OUT>::setAcceptPush( bool accept ) {
      _acceptPush = accept ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline bool ThreadPool<IN,OUT>::acceptPush() const {
      return _acceptPush.load() ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline bool ThreadPool<IN,OUT>::active() const {
      if( not _queue.empty() ) {
        return true ;
      }
      for( auto &worker : _pool ) {
        if( not worker->waiting() ) {
          return true ;
        }
      }
      return false ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void ThreadPool<IN,OUT>::stop( bool clear ) {
      if ( clear ) {
        if (_isStop) {
          return ;
        }
        _isStop = true ;
        for( auto &worker : _pool ) {
          worker->stop() ;
        }
        _queue.clear() ;
      }
      else {
        if (_isDone or _isStop) {
          return ;
        }
        _isDone = true ;  // give the waiting threads a command to finish
      }
      {
        std::unique_lock<std::mutex> lock(_mutex);
        _conditionVariable.notify_all();  // stop all waiting threads
      }
      for (auto &worker : _pool) {  // wait for the computing threads to finish
        worker->join() ;
      }
      _queue.clear() ;
      _pool.clear() ;
      _isRunning = false ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    template <class>
    inline typename ThreadPool<IN,OUT>::PushResult ThreadPool<IN,OUT>::push(PushPolicy policy, IN && queueData) {
      if( not _isRunning.load() ) {
        throw Exception( "ThreadPool::push: pool not running yet!" ) ;
      }
      if( not _acceptPush.load() ) {
        throw Exception( "ThreadPool::push: not allowed to push in pool!" ) ;
      }
      QueueElement<IN,OUT> element( std::move(queueData) ) ;
      PushResult result ;
      result.first = element.promise() ;
      result.second = result.first->get_future() ;
      if(policy == PushPolicy::Blocking) {
        // this is dirty yet
        // TODO find a proper implementation ...
        while( _queue.isFull() ) {
          std::this_thread::sleep_for( std::chrono::microseconds(10) ) ;
        }
        // this is not really safe
        _queue.push(element) ;
      }
      else {
        if( _queue.isFull() ) {
          throw Exception( "ThreadPool::push: queue is full!" ) ;
        }
        _queue.push(element) ;
      }
      std::unique_lock<std::mutex> lock(_mutex) ;
      _conditionVariable.notify_one() ;
      return std::move(result) ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

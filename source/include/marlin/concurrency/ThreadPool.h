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
#include <queue>
#include <condition_variable>

// -- marlin headers
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {
    
    namespace detail {
      
      /**
       *  @brief  Queue class.
       *  A simplified thread safe queue container.
       *  Support maximum size setting
       */
      template <typename T>
      class Queue {
      public:
        Queue() = default ;
        ~Queue() = default ;
        Queue(const Queue&) = delete ;
        Queue& operator=(const Queue&) = delete ;
        
        /**
         *  @brief  Constructor
         * 
         *  @param  maxsize the maximum queue size
         */
        Queue( std::size_t maxsize ) {
          _maxSize = maxsize ;
        }
        
        /**
         *  @brief  push a value to the queue
         *  
         *  @param  value the value to push
         */
        bool push(const T &value) {
          std::unique_lock<std::mutex> lock(_mutex) ;
          if( _queue.size() >= _maxSize ) {
            return false ;
          }
          _queue.push( value ) ;
          return true ;
        }
        
        /**
         *  @brief  Pop and get the front element in the queue.
         *  The queue type must support move operation
         *  
         *  @param  value the value to receive
         */
        bool pop( T & value ) {
          std::unique_lock<std::mutex> lock(_mutex) ;
          if( _queue.empty() ) {
            return false ;
          }
          value = std::move(_queue.front()) ;
          _queue.pop() ;
          return true ;
        }
        
        /**
         *  @brief  Whether the queue is empty
         */
        bool empty() const {
          std::unique_lock<std::mutex> lock(_mutex) ;
          return _queue.empty() ;
        }
        
        /**
         *  @brief  Get the maximum queue size
         */
        std::size_t maxSize() const {
          std::unique_lock<std::mutex> lock(_mutex) ;
          return _maxSize ;
        }

        /**
         *  @brief  Set the maximum queue size.
         *  Note that the queue is NOT resized if the new value is smaller 
         *  than the old size. The value of the old max size is returned
         * 
         *  @param  maxsize the maximum queue size to set
         */
        std::size_t setMaxSize( std::size_t maxsize ) {
          std::unique_lock<std::mutex> lock(_mutex) ;
          std::swap( _maxSize, maxsize ) ;
          return maxsize ;
        }
        
        /**
         *  @brief  Check whether the queue has reached the maximum allowed size
         */
        bool isFull() const {
          std::unique_lock<std::mutex> lock(_mutex) ;
          return (_queue.size() >= _maxSize) ;
        }
        
        /**
         *  @brief  Clear the queue
         */
        void clear() {
          std::unique_lock<std::mutex> lock(_mutex) ;
          while( not _queue.empty() ) {
            _queue.pop() ;
          }
        }
        
        /**
         *  @brief  Get the number of free slots in the queue
         */
        std::size_t freeSlots() const {
          std::unique_lock<std::mutex> lock(_mutex) ;
          return (_queue.size() >= _maxSize ? 0 : (_maxSize - _queue.size())) ;
        }
        
      private:
        /// The underlying queue object
        std::queue<T>              _queue {} ;
        /// The synchronization mutex
        mutable std::mutex         _mutex {} ;
        /// The maximum size of the queue
        std::size_t                _maxSize {std::numeric_limits<std::size_t>::max()} ;
      };
    }
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  ThreadPool class
     */
    class ThreadPool {
    public:
      using Task = std::function<void()> ;
      using TaskQueue = detail::Queue<Task> ;
      using Pool = std::vector<std::thread> ;
      using Flags = std::vector<std::atomic<bool>> ;
      
    public:
      /**
       *  @brief  Settings struct
       *  Settings to construct a thread pool
       */
      struct Settings {
        /// The thread pool size
        std::size_t     _poolSize = {std::thread::hardware_concurrency()} ;
        /// The maximum queue size
        std::size_t     _maxQueueSize = {10} ;
      };
      
      /**
       *  @brief  PushPolicy enumerator
       */
      enum class PushPolicy {
        Blocking,      ///< Block until a slot is free in the queue
        ThrowIfFull    ///< Throw an exception if the queue is full
      };
      
    public:
      ThreadPool() = delete ;
      ThreadPool(const ThreadPool &) = delete ;
      ThreadPool(ThreadPool &&) = delete ;
      ThreadPool& operator=(const ThreadPool &) = delete ;
      ThreadPool& operator=(ThreadPool &&) = delete ;
      
      /**
       *  @brief  Constructor
       *  
       *  @param  settings the thread pool settings
       */
      ThreadPool( const Settings &settings ) ;
      
      /**
       *  @brief  Destructor
       */
      ~ThreadPool() ;

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
       *  @brief  Clear the queue
       */
      void clearQueue() ;

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
       *  @param  func the task function to enqueue
       *  @param  args the arguments to pass to the task function
       */
      template <typename F, typename... Args>
      auto push( PushPolicy policy, F &&func, Args &&... args ) -> std::future<decltype(func(args...))> ;

    private:
      /**
       *  @brief  Initialize the thread pool
       */
      void init() ;
    
    private:
      ///< The synchronization mutex
      std::mutex               _mutex {} ;
      ///< The queue enqueuing condition variable
      std::condition_variable  _conditionVariable {} ;
      ///< The actual thread pool
      Pool                     _pool {} ;
      ///< The task queue
      TaskQueue                _taskQueue {} ;
      ///< Runtime flag...
      std::atomic<bool>        _isDone {false} ;
      ///< The thread pool stop flag
      std::atomic<bool>        _isStop {false} ;
      ///< The number of waiting tasks
      std::atomic<std::size_t> _nWaiting {0} ;
    };
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    template <typename F, typename... Args>
    inline auto ThreadPool::push(PushPolicy policy, F && func, Args &&... args) -> std::future<decltype(func(args...))> {
      auto pck = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
      ) ;
      Task task = [pck](){ 
        (*pck)(); 
      };
      if(policy == PushPolicy::Blocking) {
        while( not _taskQueue.push(task) ) {
          std::this_thread::sleep_for( std::chrono::milliseconds(10) ) ;
        }
      }
      else {
        if( _taskQueue.isFull() ) {
          throw Exception( "ThreadPool::push: queue is full!" ) ;
        }
      }
      std::unique_lock<std::mutex> lock(_mutex) ;
      _conditionVariable.notify_one() ;
      return pck->get_future() ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

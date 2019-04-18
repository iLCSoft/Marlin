#ifndef MARLIN_TASKMANAGER_h
#define MARLIN_TASKMANAGER_h 1

// -- std headers
#include <memory>
#include <functional>
#include <thread>

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/concurrency/ThreadPool.h>

namespace marlin {
  
  namespace concurrency {
      
    /**
     *  @brief  TaskManager class
     */
    class TaskManager {
    public:
      using TaskFunction = std::function<void()> ;
      
    public:
      /**
       *  @brief  Constructor.
       *  Share the thread pool from an external usage
       */
      TaskManager( std::shared_ptr<ThreadPool> pool ) ;
      
      /**
       *  @brief  Constructor.
       *  
       *  @param  settings the thread pool settings
       */
      TaskManager( const ThreadPool::Settings &settings ) ;
      
      /**
       *  @brief  Get the thread pool
       */
      std::shared_ptr<ThreadPool> threadPool() const ;
      
      /**
       *  @brief  Get the number of waiting tasks
       */
      std::size_t nWaiting() const ;
      
      /**
       *  @brief  Get the number of running tasks
       */
      std::size_t nRunning() const ;
      
      /**
       *  @brief  The number of free slots in the thread pool queue
       */
      std::size_t freeSlots() const ;
      
      /**
       *  @brief  Wait for all task to finish
       *
       *  @param  clearQueue whether to clear the thread pool queue before waiting
       */
      void waitForAll( bool clearQueue = true ) ;
      
      /**
       *  @brief  Post a new task in the queue.
       *  See ThreadPool::PushPolicy for behavior of task queuing
       *
       *  @param  policy the push policy
       *  @param  task the task to enqueue (functor, function, bind expression)
       *  @param  args the arguments to pass to the task function
       */
      template <typename F, typename ...Args>
      void push( ThreadPool::PushPolicy policy, F &&func, Args &&...args ) ;
      
    private:
      /// The actual thread pool implementation
      std::shared_ptr<ThreadPool>        _threadPool {nullptr} ;
    };
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    template <typename F, typename ...Args>
    inline void TaskManager::push( ThreadPool::PushPolicy policy, F &&func, Args &&...args ) {
      _threadPool->push( policy, std::forward<F>(func), std::forward<Args>(args)... ) ;
    }
    
  }

}

#endif

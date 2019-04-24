#ifndef MARLIN_CONCURRENCY_WORKER_h
#define MARLIN_CONCURRENCY_WORKER_h 1

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
#include "marlin/concurrency/QueueElement.h"

namespace marlin {

  namespace concurrency {

    template <typename IN, typename OUT>
    class ThreadPool ;
    template <typename IN, typename OUT>
    class Worker ;

    /**
     *  @brief  WorkerBase class
     *  Base class to implement processing of task data (so
     *  called queued-element) pushed in a thread pool.
     *  The IN and OUT types must be movable.
     *  See also specialization for void IN and OUT types
     */
    template <typename IN, typename OUT>
    class WorkerBase {
      friend class Worker<IN,OUT> ;
    public:
      using Input = IN ;
      using Output = OUT ;

    public:
      virtual ~WorkerBase() = default ;

      /**
       *  @brief  Process a single queued data taken form the thread pool.
       *
       *  @param  data the data value to process
       */
      virtual OUT process( IN && data ) = 0 ;

    protected:
      /**
       *  @brief  Process queued element from the thread pool
       *
       *  @param  element a thread pool queued element
       */
      void processElement( QueueElement<IN,OUT> &element ) ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename OUT>
    class WorkerBase<void,OUT> {
      friend class Worker<void,OUT> ;
    public:
      virtual ~WorkerBase() = default ;
      virtual OUT process() = 0 ;
    protected:
      void processElement( QueueElement<void,OUT> &element ) ;
    };

    //--------------------------------------------------------------------------

    template <typename IN>
    class WorkerBase<IN,void> {
      friend class Worker<IN,void> ;
    public:
      virtual ~WorkerBase() = default ;
      virtual void process( IN && data ) = 0 ;
    protected:
      void processElement( QueueElement<IN,void> &element ) ;
    };

    //--------------------------------------------------------------------------

    template <>
    class WorkerBase<void,void> {
      friend class Worker<void,void> ;
    public:
      virtual ~WorkerBase() = default ;
      virtual void process() = 0 ;
    protected:
      void processElement( QueueElement<void,void> &element ) ;
    };

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void WorkerBase<IN,OUT>::processElement( QueueElement<IN,OUT> &element ) {
      element.setValue( std::move( this->process( std::move(element.takeInput()) ) ) ) ;
    }

    template <typename IN>
    inline void WorkerBase<IN,void>::processElement( QueueElement<IN,void> &element ) {
      this->process( std::move(element.takeInput()) ) ;
      element.setValue() ;
    }

    template <typename OUT>
    inline void WorkerBase<void,OUT>::processElement( QueueElement<void,OUT> &element ) {
      element.setValue( std::move( this->process() ) ) ;
    }

    // template <>
    inline void WorkerBase<void,void>::processElement( QueueElement<void,void> &element ) {
      this->process() ;
      element.setValue() ;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  Worker class
     */
    template <typename IN, typename OUT>
    class Worker {
    public:
      using Input = IN ;
      using Output = OUT ;
      using Pool = ThreadPool<IN, OUT> ;
      using Impl = WorkerBase<IN,OUT> ;

    public:
      Worker() = delete ;
      Worker(const Worker&) = delete ;
      Worker(Worker &&) = delete ;
      Worker &operator=(const Worker&) = delete ;
      Worker &operator=(Worker&&) = delete ;

      /**
       *  @brief  Constructor
       *
       *  @param  pool the parent thread pool
       *  @param  impl the worker implementation consuming IN data, producing OUT data
       */
      template <typename IMPL, class = typename std::enable_if<std::is_base_of<Impl,IMPL>::value>::type>
      Worker( Pool &pool, std::unique_ptr<IMPL> impl ) ;

      /**
       *  @brief  Start the worker thread
       */
      void start() ;

      /**
       *  @brief  The method executing in the worker thread
       */
      void run() ;

      /**
       *  @brief  Switch ON the stop flag, requesting the worker thread to stop.
       *  No join operation is done here. See join() method
       */
      void stop() ;

      /**
       *  @brief  Whether the worker thread is currently running
       */
      bool running() const ;
      
      /**
       *  @brief  Whether the worker is waiting
       */
      bool waiting() const ;

      /**
       *  @brief  Join the worker thread
       */
      void join() ;

    private:
      ///< The parent thread pool
      Pool                        &_threadPool ;
      ///< The worker thread
      std::thread                  _thread {} ;
      ///< The stop flag
      std::atomic<bool>            _stopFlag {false} ;
      ///< Whether the worker thread is waiting for data
      std::atomic<bool>            _waitingFlag {false} ;
      ///< The worker implementation
      std::unique_ptr<Impl>        _impl {nullptr} ;
    };

  }

}

// template implementation
#include <marlin/concurrency/ThreadPool.h>

namespace marlin {

  namespace concurrency {

    template <typename IN, typename OUT>
    template <typename IMPL, class>
    inline Worker<IN,OUT>::Worker( Pool & pool, std::unique_ptr<IMPL> impl ) :
      _threadPool(pool),
      _impl(std::move(impl)) {
      /* nop */
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void Worker<IN,OUT>::start() {
      _thread = std::thread( &Worker::run, this ) ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void Worker<IN,OUT>::run() {
      QueueElement<IN,OUT> element ;
      bool isPop = _threadPool._queue.pop( element ) ;
      while (true) {
        // if there is anything in the queue
        while (isPop) {
          _impl->processElement( element ) ;
          // the thread is wanted to stop, return even if the queue is not empty yet
          if (_stopFlag.load())
            return;
          else
            isPop = _threadPool._queue.pop( element ) ;
        }
        // the queue is empty here, wait for the next command
        std::unique_lock<std::mutex> lock(_threadPool._mutex);
        _waitingFlag = true ;
        _threadPool._conditionVariable.wait(lock, [this, &element, &isPop](){
          isPop = _threadPool._queue.pop( element ) ;
          return isPop || _threadPool._isDone || _stopFlag ;
        }) ;
        _waitingFlag = false ;
        // if the queue is empty and this->isDone == true or *flag then return
        if ( not isPop ) {
          return ;
        }
      }
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void Worker<IN,OUT>::stop() {
      _stopFlag = true ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline bool Worker<IN,OUT>::running() const {
      return ( _thread.get_id() != std::thread::id() ) ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename IN, typename OUT>
    inline bool Worker<IN,OUT>::waiting() const {
      return _waitingFlag.load() ;
    }

    //--------------------------------------------------------------------------

    template <typename IN, typename OUT>
    inline void Worker<IN,OUT>::join() {
      if( _thread.joinable() ) {
        _thread.join() ;
      }
    }

  } // end namespace concurrency

} // end namespace marlin

#endif

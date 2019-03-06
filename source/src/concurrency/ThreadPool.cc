#include "marlin/concurrency/ThreadPool.h"
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {
    
    ThreadPool::Worker::Worker( ThreadPool &pool ) :
      _pool(pool) {
      _thread = std::thread( &ThreadPool::Worker::loop, this ) ;
    }
    
    //--------------------------------------------------------------------------

    void ThreadPool::Worker::loop() {
      Function func {nullptr} ;
      bool dequeued {false} ;
      while ( not _pool._shutdown ) {
        {
          std::unique_lock<std::mutex> lock( _pool._mutex ) ;
          if ( _pool._pendingTasks.empty() ) {
            _pool._condition.wait( lock ) ;
          }
          dequeued = _pool._pendingTasks.pop( func ) ;
        }
        if ( dequeued ) {
          _running = true ;
          func() ;
          _running = false ;
        }
      }
    }
    
    //--------------------------------------------------------------------------
    
    bool ThreadPool::Worker::taskRunning() const {
      return _running ;
    }
    
    //--------------------------------------------------------------------------
    
    std::thread::id ThreadPool::Worker::threadId() const {
      return _thread.get_id() ;
    }
    
    //--------------------------------------------------------------------------
    
    void ThreadPool::Worker::join() {
      if ( _thread.joinable() ) {
        _thread.join() ;
      }
    }
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    ThreadPool::ThreadPool( unsigned int nthreads ) {
      _workers.resize( nthreads ) ;
    }

    //--------------------------------------------------------------------------

    ThreadPool::~ThreadPool() {
      if ( _initialized ) {
        terminate( true ) ;
      }
    }

    //--------------------------------------------------------------------------
    
    void ThreadPool::init() {
      if ( _initialized ) {
        throw Exception( "ThreadPool::init: already initialized!" ) ;
      }
      // start worker threads
      for ( std::size_t i=0 ; i<_workers.size() ; ++i ) {
        _workers[ i ] = std::make_shared<Worker>( *this ) ;
      }
      _initialized = true ;
    }
    
    //--------------------------------------------------------------------------
    
    void ThreadPool::terminate( bool dropPending ) {
      if ( not _initialized ) {
        throw Exception( "ThreadPool::terminate: not initialized!" ) ;
      }
      // drop all pending tasks and terminate properly
      if ( dropPending ) {
        _pendingTasks.clear() ;
      }
      // wait for all pending tasks to be 
      // executed and terminate properly
      else {
        while ( not _pendingTasks.empty() ) {
          std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
        }
      }
      _shutdown = true ;
      _condition.notify_all() ;
      for (unsigned int i=0 ; i<_workers.size() ; i++ ) {
        // join the worker thread and clear
        _workers[ i ]->join() ;
        _workers[ i ] = nullptr ;
      }
      _pendingTasks.clear() ;
      _initialized = false ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nPendingTasks() const {
      return _pendingTasks.size() ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nRunningTasks() const {
      std::size_t count {0} ;
      for ( auto &w : _workers ) {
        if( w->taskRunning() ) {
          ++ count ;
        }
      }
      return count ;
    }
    
    //--------------------------------------------------------------------------
    
    bool ThreadPool::initialized() const {
      return _initialized ;
    }

  } // namespace concurrency

} // namespace marlin

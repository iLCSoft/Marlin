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
      while ( not _pool._shutdown ) {
        {
          func = nullptr ;
          // wait for a task to be queued
          std::unique_lock<std::mutex> ulock( _pool._queueMutex ) ;
          _pool._queueCondition.wait( ulock , [this, &func]() {
            if ( not _pool._pendingTasks.empty() ) {
              func = _pool._pendingTasks.front() ;
              _pool._pendingTasks.pop_front() ;              
            }
            return ( ( nullptr != func ) || _pool._shutdown ) ;
          } ) ;
        }
        if ( func ) {
          _pool._nRunningTasks ++ ;
          func() ;
          _pool._nRunningTasks -- ;
        }
      }
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

    ThreadPool::ThreadPool( const Options &options ) :
    _options(options) {
      _workers.resize( _options._nThreads ) ;
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
      _shutdown = false ;
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
        std::lock_guard<std::mutex> lock( _queueMutex ) ;
        _pendingTasks.clear() ;
        _shutdown = true ;
        _queueCondition.notify_all() ;
      }
      // wait for all pending tasks to be 
      // executed and terminate properly
      else {
        while ( 0 == _nRunningTasks ) {
          {
            std::lock_guard<std::mutex> lock( _queueMutex ) ;
            _shutdown = true ;
            _queueCondition.notify_one() ;
          }
          std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
        }
      }
      std::cout << "joining threads" << std::endl ;
      for (unsigned int i=0 ; i<_workers.size() ; i++ ) {
        // join the worker thread and clear
        _workers[ i ]->join() ;
        _workers[ i ] = nullptr ;
      }
      // no lock needed here, as no worker is running anymore
      _pendingTasks.clear() ;
      _initialized = false ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nPendingTasks() const {
      std::lock_guard<std::mutex> lock( _queueMutex ) ;
      return _pendingTasks.size() ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nRunningTasks() const {
      return _nRunningTasks ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::size() const {
      return _workers.size() ;
    }
    
    //--------------------------------------------------------------------------
    
    bool ThreadPool::initialized() const {
      return _initialized ;
    }

  } // namespace concurrency

} // namespace marlin

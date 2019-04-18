#include "marlin/concurrency/ThreadPool.h"
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {
    
    ThreadPool::ThreadPool( const Settings &settings ) :
      _pool(settings._poolSize),
      _taskQueue(settings._maxQueueSize) {
      init() ;
    }
    
    //--------------------------------------------------------------------------
    
    ThreadPool::~ThreadPool() {
      this->stop(true);
    }
    
    //--------------------------------------------------------------------------
    
    void ThreadPool::init() { 
      for (size_t i=0 ; i<_pool.size() ; i++) {        
        _pool [i] = std::thread([this]() {
          Task task {} ;
          bool isPop = _taskQueue.pop(task) ;
          while (true) {
            // until nothing in the queue, process task
            while (isPop) {
              task() ;
              if (_isStop) {
                return ;
              }
              isPop = _taskQueue.pop(task) ;
            }
            // the queue is empty here, wait for the next command
            std::unique_lock<std::mutex> lock(_mutex) ;
            ++_nWaiting ;
            _conditionVariable.wait(lock, [this, &task, &isPop] () { 
              isPop = _taskQueue.pop(task); 
              return (isPop || _isDone) ; 
            }) ;
            --_nWaiting;
            if ( not isPop ) {
              return;  // if the queue is empty and this->isDone == true then return              
            }
          }
        }) ;
      }
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::size() const { 
      return _pool.size() ; 
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nWaiting() const { 
      return _nWaiting.load() ; 
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::nRunning() const {
      return ( _pool.size() - _nWaiting.load() ) ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t ThreadPool::freeSlots() const {
      return _taskQueue.freeSlots() ;
    }
    
    //--------------------------------------------------------------------------
    
    void ThreadPool::clearQueue() {
      _taskQueue.clear() ;
    }
    
    //--------------------------------------------------------------------------
    
    void ThreadPool::stop( bool clear ) {
      if ( clear ) {
        if (_isStop) {
          return ;
        }
        _isStop = true ;
        _taskQueue.clear() ;
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
      for (auto &thr : _pool) {  // wait for the computing threads to finish
        if ( thr.joinable() ) {
          thr.join() ;
        }
      }
      _taskQueue.clear() ;
      _pool.clear() ;
    }

  } // namespace concurrency

} // namespace marlin

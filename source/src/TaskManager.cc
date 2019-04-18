#include <marlin/TaskManager.h>

namespace marlin {
  
  namespace concurrency {
  
    TaskManager::TaskManager( std::shared_ptr<ThreadPool> pool ) :
      _threadPool(pool) {
      /* nop */
    }
    
    //--------------------------------------------------------------------------
    
    TaskManager::TaskManager( const ThreadPool::Settings &settings ) :
      _threadPool(std::make_shared<ThreadPool>(settings)) {
      
    }
    
    //--------------------------------------------------------------------------
    
    std::shared_ptr<ThreadPool> TaskManager::threadPool() const {
      return _threadPool ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t TaskManager::nWaiting() const {
      return _threadPool->nWaiting() ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t TaskManager::nRunning() const {
      return _threadPool->nRunning() ;
    }
    
    //--------------------------------------------------------------------------
    
    std::size_t TaskManager::freeSlots() const {
      return _threadPool->freeSlots() ;
    }
    
    //--------------------------------------------------------------------------
    
    void TaskManager::waitForAll( bool clearQueue ) {
      // TODO implement me !!!
    }
    
  }

}

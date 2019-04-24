// -- marlin headers
#include <marlin/concurrency/ThreadPool.h>
#include <UnitTesting.h>

using namespace marlin ;
using namespace marlin::test ;
using namespace marlin::concurrency ;

using Function = std::function<void()> ;
using Pool = ThreadPool<Function,void> ;
using PushResultList = std::vector<Pool::PushResult> ;

std::mutex coutMutex ;
#define SAFE_LOG( MESSAGE ) { std::lock_guard<std::mutex> lock( coutMutex ) ;  std::cout << MESSAGE << std::endl ; } 

class TestWorker : public WorkerBase<Function,void> {
public:
  TestWorker(unsigned int id) : _id(id) {}
  
  void process( Function && f ) {
    SAFE_LOG( "TestWorker: thread id " << std::this_thread::get_id() << ", worker id " << _id ) ;
    SAFE_LOG( "TestWorker: Executing function now " ) ;
    f() ;
    SAFE_LOG( "TestWorker: Executing function now ... OK" ) ;
  }
  
private:
  unsigned int _id {0} ;
};

int main( int argc, char **argv ) {
  
  UnitTest test( "ThreadPool" ) ;
  test.test( "obvious", true ) ;
  Pool pool ;
  
  unsigned int nworkers = std::thread::hardware_concurrency() ;
  SAFE_LOG( "N WORKERS: " << nworkers ) ;
  
  for( unsigned int w=0 ; w<nworkers ; ++w ) {
    pool.addWorker<TestWorker>( w ) ;
  }
  
  pool.setMaxQueueSize( 20 ) ;
  pool.start() ;
  
  std::atomic_int counter {0} ;
  PushResultList results ;
  Function f = [&](){
    SAFE_LOG( "Hello from function" ) ;
    SAFE_LOG( "Counter is " << counter ) ;
    std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) ) ;
    counter++ ;
  } ;
  
  Function f1 = f ;
  results.push_back( pool.push( Pool::PushPolicy::Blocking, std::move( f1 ) ) );
  
  Function f2 = f ;
  results.push_back( pool.push( Pool::PushPolicy::Blocking, std::move( f2 ) ) );
  
  Function f3 = f ;
  results.push_back( pool.push( Pool::PushPolicy::Blocking, std::move( f3 ) ) );
  
  std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) ) ;
  
  // wait for termination
  auto i = 0 ;
  for( auto &res : results ) {
    SAFE_LOG( "Getting future ... " << ++i ) ;
    res.second.get() ;
  }
  
  pool.stop(false) ;
  
  test.test( "counter", counter.load() == 3 ) ;
    
  return 0 ;
}

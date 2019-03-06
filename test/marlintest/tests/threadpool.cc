
#include "UnitTesting.h"

#include "marlin/concurrency/ThreadPool.h"

#include <chrono>

using namespace marlin::concurrency ;

int main() {
  
  marlin::test::UnitTest test( "Marlin thread pool" ) ;
  
  ThreadPool pool ;

  try {
    pool.init() ;
    test.test( "init", true ) ;
  }
  catch(...) {
    // dirty way to fail the test ...
    test.test( "init", false ) ;
  }
  
  std::vector<std::future<void>> futures {} ;
  std::mutex printMutex {} ;
  std::atomic<int> counter {0} ;
  
  // submit tasks
  for ( unsigned int i=0 ; i<100 ; i++ ) {
    futures.push_back( pool.submit( [&](int id) {
      printMutex.lock() ;
      std::cout << "Hello from task " << id << std::endl ;
      printMutex.unlock() ;
      counter++ ;
    }, i ) ) ;
  }
  
  // wait for all tasks termination
  for ( auto &f : futures ) {
    f.get() ;
  }
  
  // all must be finished there and counter correct
  test.test( "pending zero", pool.nPendingTasks() == 0 ) ;
  test.test( "running zero", pool.nRunningTasks() == 0 ) ;
  test.test( "counter", counter == 100 ) ;

  try {
    pool.terminate() ;
    test.test( "terminate", true ) ;
  }
  catch(...) {
    // dirty way to fail the test ...
    test.test( "terminate", false ) ;
  }
  
  // submit tasks again but the thread pool is off
  // so normally nothing should be processed
  for ( unsigned int i=0 ; i<100 ; i++ ) {
    futures.push_back( pool.submit( [&](int id) {
      printMutex.lock() ;
      std::cout << "Hello from task " << id << std::endl ;
      printMutex.unlock() ;
      counter++ ;
    }, i ) ) ;
  }
  // wait a bit
  std::this_thread::sleep_for( std::chrono::milliseconds(500) ) ;

  // counter should still be the same
  test.test( "counter", counter == 100 ) ;
  
  return 0;
}
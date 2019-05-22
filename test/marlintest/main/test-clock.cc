// -- marlin headers
#include <marlin/Utils.h>
#include <UnitTesting.h>

#include <future>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>

using namespace marlin::test ;


int main( int argc, char **argv ) {
  
  UnitTest test( "Clock" ) ;
  test.test( "obvious", true ) ;

  std::vector<std::future<marlin::clock::duration_rep>> futures ;
  std::atomic<marlin::clock::duration_rep> timediff {0.f} ;
  auto start = marlin::clock::now() ;
  const marlin::clock::duration_rep crunchTime = 1 ; 
  
  for( auto i=0u ; i<std::thread::hardware_concurrency() ; ++i ) {
    futures.push_back( std::async( std::launch::async, [&](){
      auto localStart = marlin::clock::now() ;
      marlin::clock::crunchFor<marlin::clock::seconds>(crunchTime) ;
      auto localEnd = marlin::clock::now() ;
      auto localDiff = marlin::clock::time_difference<marlin::clock::milliseconds>( localStart, localEnd ) ;
      // the last task to exit set the total time diff
      timediff = marlin::clock::elapsed_since<marlin::clock::milliseconds>( start ) ;
      return localDiff ;
    })) ;
  }
  
  float timetot = 0.f ;
  for( auto &f : futures ) {
    timetot += f.get() ;
  }
  
  std::cout << "Accumulated time: " << timetot << " ms" << std::endl ;
  std::cout << "Local time diff:  " << timediff << " ms" << std::endl ;
    
  return 0 ;
}

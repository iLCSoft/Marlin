#ifndef ProcessorLoader_h
#define ProcessorLoader_h 1
#ifndef MARLIN_NO_DLL

#include "lcio.h"

#include "LCIOSTLTypes.h"

#include <string>
#include <vector>

namespace marlin{
  
  
  
  /** Processor loader - loads shared libraries with marlin processors. 
   *  The shared libraries are loaded in the constructor with dlopen and 
   *  closed in the destructor, i.e. their lifetime is the same as that 
   *  of the ProcessorLoader instance.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: ProcessorLoader.h,v 1.3 2008-03-11 15:17:14 engels Exp $ 
   */
  class ProcessorLoader {
    
    
    typedef std::vector<void*> LibVec ;
    
  public:
    
    ProcessorLoader( lcio::StringVec::const_iterator  first, lcio::StringVec::const_iterator last ) ;
    
    virtual ~ProcessorLoader() ;

    bool failedLoading() { return _loadError; };
    
    
  protected:
    
    LibVec _libs ;

  private:
    bool _loadError;
  };

} // end namespace marlin 
#endif
#endif

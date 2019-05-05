#include <marlin/Exceptions.h>
#include <marlin/Processor.h>

namespace marlin {

  ParseException::ParseException( const std::string &text ) {
    message = "marlin::ParseException: " + text ;
  }

  SkipEventException::SkipEventException( const Processor* proc ) {
    message = proc->name() ;
  }

  StopProcessingException::StopProcessingException( const Processor* proc ) {
    message = proc->name() ;
  }

  RewindDataFilesException::RewindDataFilesException( const Processor* proc ) {
    message = proc->name() ;
  }

}

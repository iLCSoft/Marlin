#ifndef MARLIN_CONCURRENCY_DD4HEPEXTENSIONS_h
#define MARLIN_CONCURRENCY_DD4HEPEXTENSIONS_h 1

// -- lcio headers
#include "LCRTRelations.h"

// -- dd4hep headers
#include "DD4hep/Detector.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  The dd4hep detector extension
     */
    struct DD4hepDetectorExt : public LCExtension<DD4hepDetector, dd4hep::Detector> {};

  } // end namespace concurrency

} // end namespace marlin

#endif

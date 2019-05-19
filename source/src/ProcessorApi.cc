#include <marlin/ProcessorApi.h>

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/Application.h>
#include <marlin/Processor.h>
#include <marlin/EventExtensions.h>

// -- lcio headers
#include <EVENT/LCEvent.h>

// -- dd4hep headers
#ifdef MARLIN_DD4HEP
#include <DD4hep/Detector.h>
#endif

// -- gear headers
#ifdef MARLIN_GEAR
#include <gear/GearMgr.h>
#endif

namespace marlin {

  void ProcessorApi::registerForRandomSeeds( Processor *const proc ) {
    proc->app().randomSeedManager().addEntry( proc ) ;
  }

  //--------------------------------------------------------------------------

  unsigned int ProcessorApi::getRandomSeed( const Processor *const proc, EVENT::LCEvent *event ) {
    auto randomSeeds = event->runtime().ext<RandomSeed>() ;
    if( nullptr == randomSeeds ) {
      throw Exception( "ProcessorApi::getRandomSeed: no random seed extension in event" ) ;
    }
    return randomSeeds->randomSeed( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EVENT::LCEvent *event, bool value ) {
    auto procConds = event->runtime().ext<ProcessorConditions>() ;
    if( nullptr == procConds ) {
      throw Exception( "ProcessorApi::setReturnValue: no processor conditions extension in event" ) ;
    }
    return procConds->set( proc, value ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EVENT::LCEvent *event, const std::string &name, bool value ) {
    auto procConds = event->runtime().ext<ProcessorConditions>() ;
    if( nullptr == procConds ) {
      throw Exception( "ProcessorApi::setReturnValue: no processor conditions extension in event" ) ;
    }
    return procConds->set( proc, name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool ProcessorApi::isFirstEvent( EVENT::LCEvent *event ) {
    return event->runtime().ext<IsFirstEvent>() ;
  }

  //--------------------------------------------------------------------------
#ifdef MARLIN_DD4HEP
  const dd4hep::Detector* ProcessorApi::dd4hepDetector( const Processor *const proc ) {
    return proc->app().geometryManager().geometry<dd4hep::Detector>() ;
  }
#endif
  //--------------------------------------------------------------------------
#ifdef MARLIN_GEAR
  const gear::GearMgr* ProcessorApi::gearDetector( const Processor *const proc ) {
    return proc->app().geometryManager().geometry<gear::GearMgr>() ;
  }
#endif
  //--------------------------------------------------------------------------

  void ProcessorApi::skipCurrentEvent( const Processor *const proc ) {
    proc->log<WARNING>() << "Skipping current event !" << std::endl ;
    throw SkipEventException( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::abort( const Processor *const proc, const std::string &reason ) {
    proc->log<WARNING>() << "Stopping application: " << reason << std::endl ;
    throw StopProcessingException( proc ) ;
  }

}

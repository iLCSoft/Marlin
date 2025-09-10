#include "marlin/EmptyEventCreator.h"

#include "marlin/ProcessorMgr.h"

#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

#include <memory>

namespace marlin {
EmptyEventCreator anEmptyEventCreator;

EmptyEventCreator::EmptyEventCreator() : DataSourceProcessor("EmptyEventCreator") {
  _description = "Creates empty events that can be filled by other processors";
}

void EmptyEventCreator::init() {
  printParameters();  // Do we need to do anything at all?
}

void EmptyEventCreator::readDataSource(int numEvents) {
  while (m_eventNumber < numEvents) {
    if (isFirstEvent()) {
      auto* runHeader = new LCRunHeaderImpl; // who cleans this up?
      runHeader->setDescription("Empty events produced to be filled later");
      runHeader->setRunNumber(0);

      ProcessorMgr::instance()->processRunHeader(runHeader);
      _isFirstEvent = false;
    }

    auto evt = std::make_unique<LCEventImpl>();
    evt->setRunNumber(0);
    evt->setEventNumber(m_eventNumber++);

    ProcessorMgr::instance()->processEvent(evt.get());
  }
}

} // namespace marlin

#ifndef TestEmptyEventCreator_h
#define TestEmptyEventCreator_h 1

#include "marlin/Processor.h"

#include "lcio.h"

/** Test processor to check that the EmptyEventCreator data source produces the
 * expected empty events
 */
class TestEmptyEventCreator : public marlin::Processor {

public:
  TestEmptyEventCreator *newProcessor() override {
    return new TestEmptyEventCreator;
  }

  TestEmptyEventCreator();

  void processRunHeader(lcio::LCRunHeader *run) override;

  void processEvent(lcio::LCEvent *evt) override;

private:
  int m_expectedEvt{0};
};

#endif

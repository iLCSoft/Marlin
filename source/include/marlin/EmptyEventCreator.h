#ifndef EmptyEventCreator_h
#define EmptyEventCreator_h 1

#include "marlin/DataSourceProcessor.h"

namespace marlin {

/** Creates empty LCIO Events that can then be populated by other processors.
 *  Useful for testing purposes and examples, where having a dedicated input
 *  just for "getting the event loop going" is not always practical. This needs
 *  to be the first active processor and additionally requires that there are no
 *  LCIO input collections present (i.e. make sure to not set the
 *  LCIOInputFiles parameter).
 *
 *  <h4>Input</h4>
 *  None
 *
 *  <h4>Output</4>
 *  an empty LCEvent
 *
 *  @author: T. Madlener, DESY
 */
class EmptyEventCreator : public DataSourceProcessor {
public:
  EmptyEventCreator();

  marlin::Processor* newProcessor() override { return new EmptyEventCreator; }

  void readDataSource(int numEvents) override;
  void init() override;

private:
  int m_eventNumber{0};
};

}

#endif

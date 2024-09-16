#include "TestEmptyEventCreator.h"

#include <string>
#include <vector>

TestEmptyEventCreator aTestEmptyEventCreator;

TestEmptyEventCreator::TestEmptyEventCreator()
    : marlin::Processor("TestEmptyEventCreator") {
  _description = "Processor to test whether the EmptyEventCreator data source "
                 "works as expected";
}

void TestEmptyEventCreator::processRunHeader(lcio::LCRunHeader *run) {
  if (run->getRunNumber() != 0) {
    throw lcio::Exception("Run number from EmptyEventCreator is not 0 (" +
                          std::to_string(run->getRunNumber()) + ")");
  }
}

void TestEmptyEventCreator::processEvent(lcio::LCEvent *evt) {
  if (evt->getEventNumber() != m_expectedEvt) {
    throw lcio::Exception(
        "Event number from EmptyEventCreator is not as expected (expected: " +
        std::to_string(m_expectedEvt) +
        ", actual: " + std::to_string(evt->getEventNumber()) + ")");
  }

  if (!evt->getCollectionNames()->empty()) {
    throw lcio::Exception(
        "EmptyEventCreator should create empty events, but LCEvent contains " +
        std::to_string(evt->getCollectionNames()->size()) + " collections");
  }

  const auto &params = evt->getParameters();

  std::vector<std::string> keys;
  params.getIntKeys(keys);
  if (!keys.empty()) {
    throw lcio::Exception(
        "EmptyEventCreator should create empty events, but has int parameters");
  }
  keys.clear();

  params.getFloatKeys(keys);
  if (!keys.empty()) {
    throw lcio::Exception("EmptyEventCreator should create empty events, but "
                          "has float parameters");
  }
  keys.clear();

  params.getStringKeys(keys);
  if (!keys.empty()) {
    throw lcio::Exception("EmptyEventCreator should create empty events, but "
                          "has string parameters");
  }

  m_expectedEvt++;
}

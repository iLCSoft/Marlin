#ifndef TestCommentParsingProcessor_h
#define TestCommentParsingProcessor_h

#include "marlin/Processor.h"

#include "lcio.h"

#include <string>
#include <vector>

/** Test processor for Marlin that can be used to check whether comments in the
 * xml steering files are handled correctly (and as expected).
 *
 */
class TestCommentParsingProcessor : public marlin::Processor {
public:
  Processor *newProcessor() override { return new TestCommentParsingProcessor; }

  TestCommentParsingProcessor();

  void init() override;

private:
  /// Vectors for different parameters that are read from the steering file
  std::vector<std::string> m_leadingComment{};
  std::vector<std::string> m_trailingComment{};
  std::vector<std::string> m_mixedComment{};
  std::vector<std::string> m_noComment{};
  std::vector<std::string> m_onlyComment{};
  std::vector<std::string> m_explicitValue{};
};

#endif

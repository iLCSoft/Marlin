#include "TestCommentParsingProcessor.h"

#include "marlin/VerbosityLevels.h"

TestCommentParsingProcessor aTestCommentParsingProcessor;

TestCommentParsingProcessor::TestCommentParsingProcessor()
    : marlin::Processor("TestCommentParsingProcessor") {
  _description = "TestCommentParsingProcessor can be used to check whether "
                 "comments in the xml steering files are correctly handled";

  std::vector<std::string> defaultEmpty{};

  registerProcessorParameter("leadingComment",
                             "A parameter where we test leading comments",
                             m_leadingComment, defaultEmpty);

  registerProcessorParameter("trailingComment",
                             "A parameter where we test trailing comments",
                             m_trailingComment, defaultEmpty);

  registerProcessorParameter("mixedComment",
                             "A parameter where we test mixed comments",
                             m_mixedComment, defaultEmpty);

  registerProcessorParameter("noComment",
                             "A parameter where we test no comments",
                             m_noComment, defaultEmpty);

  std::vector<std::string> defaultFilled = {"Non", "empty", "string", "vec"};
  registerProcessorParameter("onlyComment",
                             "A parameter where we test only comments",
                             m_onlyComment, defaultFilled);

  registerProcessorParameter(
      "explicitValue", "A parameter that we pass in via an explicit value",
      m_explicitValue, defaultEmpty);
}

std::ostream &operator<<(std::ostream &os, std::vector<std::string> stringVec) {
  os << "{";
  if (!stringVec.empty()) {
    os << stringVec[0];
  }
  for (size_t i = 1; i < stringVec.size(); ++i) {
    os << ", " << stringVec[i];
  }

  return os << "}";
}

void TestCommentParsingProcessor::init() {
  // Dump all values to the screen in a way that can be easily tested via
  // CMake/CTest RegEx
  streamlog_out(MESSAGE) << "LEADING " << m_leadingComment << std::endl;
  streamlog_out(MESSAGE) << "MIXED " << m_mixedComment << std::endl;
  streamlog_out(MESSAGE) << "TRAILING " << m_trailingComment << std::endl;
  streamlog_out(MESSAGE) << "NO " << m_noComment << std::endl;
  streamlog_out(MESSAGE) << "ONLY " << m_onlyComment << std::endl;
  streamlog_out(MESSAGE) << "EXPLICIT " << m_explicitValue << std::endl;
}

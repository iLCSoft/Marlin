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

  registerProcessorParameter("onlyComment",
                             "A parameter where we test only comments",
                             m_onlyComment, defaultEmpty);
}

std::ostream &operator<<(std::ostream &os, std::vector<std::string> stringVec) {
  os << "(";
  if (!stringVec.empty()) {
    os << stringVec[0];
  }
  for (size_t i = 1; i < stringVec.size(); ++i) {
    os << ", " << stringVec[i];
  }

  return os << ")";
}

void TestCommentParsingProcessor::init() {
  streamlog_out(MESSAGE) << "Got the following number of elements (leading | "
                            "trailing | mixed | no): "
                         << m_leadingComment.size() << " | "
                         << m_trailingComment.size() << " | "
                         << m_mixedComment.size() << " | " << m_noComment.size()
                         << std::endl;

  streamlog_out(DEBUG) << "The leading comment parameter contents are: "
                       << m_leadingComment << std::endl;

  streamlog_out(DEBUG) << "The trailing comment parameter contents are: "
                       << m_trailingComment << std::endl;

  streamlog_out(DEBUG) << "The mixed comment parameter contents are: "
                       << m_mixedComment << std::endl;

  streamlog_out(DEBUG) << "The no comment parameter contents are: "
                       << m_noComment << std::endl;

  streamlog_out(DEBUG) << "The only comment parameter contents are: "
                       << m_onlyComment << std::endl;
}

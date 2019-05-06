# Specification and TODO things to get a MT version of Marlin

## TODO list

- Remove source files:
  - AIDAProcessor.h
- Move source files:
  - ErrorOfSigma.h / .cc -> Move to MarlinUtil
  - FastMCParticleType.h -> Move to MarlinUtil
  - IFourVectorSmearer.h -> Move to MarlinUtil
  - IRecoParticleFactory.h -> Move to MarlinUtil
  - SimpleClusterSmearer.h / .cc -> Move to MarlinUtil
  - SimpleParticleFactory.h / .cc -> Move to MarlinUtil
  - SimpleTrackSmearer.h / .cc -> Move to MarlinUtil


- Merge MarlinApplication and MarlinMTApplication in one code
- Deal with Application::printUsage() for normal and MT apps (merge them ?)
- Implement the clock calculation at end of application
- Find a registration mechanism for random seeds for processors
- Make the test random seed processor working again
- Re-implement MarlinSteerCheck again with the option -x to dump processors

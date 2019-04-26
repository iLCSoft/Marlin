#ifndef MARLIN_AIDAPROCESSOR_h
#define MARLIN_AIDAPROCESSOR_h 1

#include <marlin/MarlinConfig.h> // for MARLIN_AIDA

#ifdef MARLIN_AIDA
#error "marlin/AIDAProcessor.h header is deprecated (empty header). \nPlease set MARLIN_AIDA to OFF in Marlin or remove this from your includes"
#else
#warning "marlin/AIDAProcessor.h header is deprecated (empty header)."
#endif

#endif // MARLIN_AIDAPROCESSOR_h


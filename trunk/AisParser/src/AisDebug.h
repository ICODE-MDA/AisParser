#ifndef AisDebug_h
#define AisDebug_h
#include <iostream>

#define AIS_DEBUG

#ifdef AIS_DEBUG
#define aisDebug(x)  std::cerr << x  << std::endl;
#else
#define aisDebug(x)
#endif

#endif

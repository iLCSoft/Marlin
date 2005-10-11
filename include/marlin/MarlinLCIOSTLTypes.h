// -*- C++ -*-

/**Typedefs for std::vectors of basic types used in LCIO - only needed 
 * for older versions of LCIO (pre 1.2)
 * 
 *  @author F. Gaede, DESY
 *  @version $Id: MarlinLCIOSTLTypes.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
 */

#ifndef LCIO_STD_TYPES
#define LCIO_STD_TYPES 1


#include <string>
#include <vector>


namespace EVENT
{
typedef std::vector< std::string > StringVec ;
typedef std::vector< float  > FloatVec ;
typedef std::vector< int > IntVec ;

};

#endif

#ifndef CMPROCESSOR_H
#define CMPROCESSOR_H

#include "marlin/Processor.h"
#include "marlin/ProcessorMgr.h"

namespace marlin{
    typedef std::set< std::string > sSet;
    typedef std::map< std::string, bool > sbMap;
    typedef std::map< std::string, std::string > ssMap;
    typedef std::map< std::string, StringParameters* > sSPMap;
    
 /**
 * This singleton class contains an instance
 * of every available marlin processor type
 * 
 * @author Jan Engels, DESY
 * @version $Id: CMProcessor.h,v 1.8 2007-01-11 16:56:37 engels Exp $
 */
   
    class CMProcessor{

    public:
	// Destructor
	~CMProcessor();
	
	/** return the instance of this class */
	static CMProcessor* instance() ;

	/** returns the instance of the processor with the given type */
	Processor* getProc( const std::string& type );

	/** returns a map with the processor descriptions */
	ssMap getProcDesc(){ return _mpDescriptions; }

	/** returns the parameters for the processor of the given type */
	StringParameters* getSParams( const std::string& type );
	
	/** merges the given parameters with the default ones 
	 *  from the processor with the given type */
	StringParameters* mergeParams( const std::string& type, StringParameters* sp );
	
	/** returns the parameter with the given key
	 *  of the processor with the given type */
	ProcessorParameter* getParam( const std::string& type, const std::string& key );

	/** returns the description of the parameter with the given key
	 *  for the processor with the given type */
	const std::string getParamD( const std::string& type, const std::string& key );
	
	/** returns the type of the parameter with the given key
	 *  for the processor with the given type */
	const std::string getParamT( const std::string& type, const std::string& key );
	
	/** returns the set_size of the parameter with the given key
	 *  for the processor with the given type */
	int getParamSetSize( const std::string& type, const std::string& key );
	
	/** returns true if the parameter with the given key
	 *  for the processor with the given type is a vector */
	bool isParamVec( const std::string& type, const std::string& key );
	
	/** returns true if the parameter with the given key
	 *  for the processor with the given type is optional*/
	bool isParamOpt( const std::string& type, const std::string& key );
	
	/** returns true if the processor with the given type is installed */
	bool isInstalled( const std::string& type );

	//utility function to tokenize strings
	void tokenize( const std::string str, StringVec& tokens, const std::string& delimiters = " " );

    private:
	// Constructor
	CMProcessor();

	static CMProcessor*  _me;	//singleton instance
	ProcessorMap _mProcs;		//processor instances
	ssMap _mpDescriptions;		//processor descriptions
	sbMap _mpStatus;		//processor status ( INSTALLED=true ; NOT_INSTALLED=false )
	sSPMap _mpSParameters;		//processor string parameters

	sSet _mpTypes;			//all processor types
	
    }; // end class CMProcessor
    
} // end namespace marlin
#endif

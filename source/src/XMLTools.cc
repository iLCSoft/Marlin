#include <marlin/XMLTools.h>

// -- marlin headers
#include <marlin/PluginManager.h>
#include <marlin/Processor.h>

// -- std headers
#include <fstream>

namespace marlin {

  void XMLTools::dumpRegisteredProcessors( std::ostream &stream ) {
    stream << "<!--##########################################" << std::endl
           << "    #                                        #" << std::endl
           << "    #     Example steering file for marlin   #" << std::endl
           << "    #                                        #" << std::endl
           << "    ##########################################-->" << std::endl
           <<  std::endl ;

    stream << std::endl
           << "<marlin xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
           << "xsi:noNamespaceSchemaLocation=\"http://ilcsoft.desy.de/marlin/marlin.xsd\">"
           <<  std::endl ;

    stream <<  " <execute>" << std::endl
    		   << "    <processor name=\"MyEventSelector\"/> " << std::endl
    		   << "    <if condition=\"MyEventSelector\">    " << std::endl
    		   <<  "     <processor name=\"MyTestProcessor\"/>  " << std::endl
    		   <<  "     <processor name=\"MyLCIOOutputProcessor\"/>  " << std::endl
    		   << "    </if>                                 " << std::endl
    		   <<  " </execute>" << std::endl
    		   << std::endl ;

    stream <<  " <global>" << std::endl
           <<  "   <parameter name=\"ColoredConsole\"> true </parameter>" << std::endl
           <<  "   <!--parameter name=\"LogFileName\"> marlin.log </parameter-->" << std::endl
           <<  "   <!-- For parallel application, this parameter specifies the number of cores to use -->" << std::endl
           <<  "   <parameter name=\"Concurrency\"> auto </parameter>" << std::endl
    		   <<  "   <parameter name=\"Verbosity\" options=\"DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT\"> DEBUG  </parameter> " << std::endl
    		   <<  "   <parameter name=\"RandomSeed\" value=\"1234567890\" />" << std::endl
           <<  "   <!-- Turn on this parameter to output the full steering file with processed includes -->"
           <<  "   <!--parameter name=\"OutputSteeringFile\"> parsed.xml </parameter-->" << std::endl
    		   <<  " </global>" << std::endl
    		   << std::endl ;

    stream <<  " <datasource type=\"LCIO\">" << std::endl
           <<  "   <parameter name=\"LCIOInputFiles\"> simjob.slcio </parameter>" << std::endl
           <<  "   <!-- limit the number of processed records (run+evt): -->  " << std::endl
           <<  "   <parameter name=\"MaxRecordNumber\" value=\"5001\" />  " << std::endl
           <<  "   <parameter name=\"SkipNEvents\" value=\"0\" />  " << std::endl
           <<  "   <!-- optionally limit the collections that are read from the input file: -->  " << std::endl
           <<  "   <!--parameter name=\"LCIOReadCollectionNames\">MCParticle PandoraPFOs</parameter-->" << std::endl
           <<  " </datasource>" << std::endl
           << std::endl ;

    stream << " <geometry type=\"DD4hep\">" << std::endl
           << "   <parameter name=\"CompactFile\"> /path/to/lcgeo/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml </parameter>" << std::endl
           << "   <parameter name=\"DumpGeometry\"> false </parameter>" << std::endl
           << " </geometry>" << std::endl
           << std::endl ;

    auto &mgr = PluginManager::instance() ;
    auto processorTypes = mgr.pluginNames( PluginType::Processor ) ;

    for( auto processorType : processorTypes ) {
      auto processor = mgr.create<Processor>( PluginType::Processor, processorType ) ;
      processor->printDescriptionXML( stream ) ;
    }

    stream << std::endl
           << "</marlin>"
           <<  std::endl ;
  }

  //--------------------------------------------------------------------------

  void XMLTools::dumpRegisteredProcessors( const std::string &fname ) {
    std::ofstream stream ;
    stream.open( fname ) ;
    if( not stream.is_open() ) {
      throw Exception( "XMLTools::dumpRegisteredProcessors: couldn't open output file: " + fname ) ;
    }
    XMLTools::dumpRegisteredProcessors( stream ) ;
  }

}

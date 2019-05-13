// -- lcio headers
#include <MT/LCReader.h>
#include <MT/LCReaderListener.h>
#include <Exceptions.h>

// -- marlin headers
#include <marlin/Utils.h>

// -- root headers
#include <TF2.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TH1.h>
#include <TLatex.h>

/**
 *  @brief  Small utility to calculate from a list
 *  of LCIO input files, the mean event reading time
 *  and compute the maximum number of cores to use to
 *  run a parallel application and given a single event
 *  processing time. The output is a root file with
 *  a ROOT TCanvas and a TF1 showing the IO/CPU limit.
 *  A text is also output in the console explaning how
 *  to interpret the root file content.
 *
 *  @author  Remi Ete, DESY
 *  @date    05/2019
 */
int main( int argc, char **argv ) {

  if( argc < 2 ) {
    std::cout << "Usage: MarlinMT_IOCPUBounds <input-files>" << std::endl;
    return 1 ;
  }
  // get lcio file names from argv
  std::vector<std::string> filenames ;
  for( int i=1 ; i<argc ; ++i ) {
    filenames.push_back( argv[i] ) ;
  }
  MT::LCReader reader( 0 ) ;
  reader.open( filenames ) ;

  float totalReadTime = 0.f ; // unit ms
  unsigned int nReadEvents = 0 ;
  MT::LCReaderListenerList empty ;

  while(1) {
    if( nReadEvents >= 1000 ) {
      break;
    }
    try {
      auto start = marlin::clock::now() ;
      reader.readNextRecord( empty ) ;
      auto end = marlin::clock::now() ;
      totalReadTime += marlin::clock::time_difference<marlin::clock::milliseconds>( start, end ) ;
      ++nReadEvents ;
    }
    catch( IO::EndOfDataException &e ) {
      // end of file
      break ;
    }
  }
  if( nReadEvents == 0 ) {
    std::cout << "Couldn't extract a single event from input file(s)!" << std::endl ;
    return 1 ;
  }

  const float meanReadTime = totalReadTime / nReadEvents ;
  const float nMaxCores = 20 ;
  const float xmin = 0, xmax = nMaxCores ;
  std::string title = "IO = CPU band (read time = " + marlin::StringUtil::typeToString(meanReadTime) + " ms);Number of cores;Single event processing time (ms);#Delta T (ms)";

  TF1 *fboundsLimits = new TF1("BoundsLimits", "[0]*x", xmin, xmax) ;
  fboundsLimits->SetTitle( title.c_str() ) ;
  fboundsLimits->SetParameter( 0, meanReadTime ) ;
  fboundsLimits->SetLineStyle( 7 ) ;
  fboundsLimits->SetLineColor( kBlack ) ;

  TCanvas *canvas = new TCanvas() ;
  canvas->cd() ;
  canvas->SetGridx();
  canvas->SetGridy();
  fboundsLimits->Draw("") ;

  auto file = TFile::Open( "marlin_bounds_diagnostic.root", "RECREATE" ) ;
  file->WriteObject( canvas, "BoundsCanvas" ) ;
  file->WriteObject( fboundsLimits, "BoundsLimits" ) ;
  file->Close() ;

  delete fboundsLimits ;
  delete canvas ;
  delete file ;

  std::cout << "Written marlin_bounds_diagnostic.root ..." << std::endl ;
  std::cout << "-----------------------------------------------" << std::endl ;
  std::cout << "-- Read statistics:" << std::endl ;
  std::cout << "-     Mean read time: " << meanReadTime << " ms" << std::endl ;
  std::cout << "-     Number of events: " << nReadEvents << std::endl ;
  std::cout << "-----------------------------------------------" << std::endl ;
  std::cout << "-- IO/CPU bound limit: T = Te - N*" << meanReadTime << std::endl ;
  std::cout << "-  with:" << std::endl ;
  std::cout << "-     Te     mean time to process a single event" << std::endl ;
  std::cout << "-     N      number of cores used in parallel processing" << std::endl ;
  std::cout << "-----------------------------------------------" << std::endl ;
  std::cout << "-- HOW TO READ THE GENERATED PLOT (BoundsCanvas)" << std::endl ;
  std::cout << "-     The mean read of your input files has been " << std::endl ;
  std::cout << "-     estimated to " << meanReadTime << " ms. In a parallel application" << std::endl ;
  std::cout << "-     there is a balance between the time you spend reading" << std::endl ;
  std::cout << "-     an event and the time the application spend to process" << std::endl ;
  std::cout << "-     event in parallel. If your application spends too" << std::endl ;
  std::cout << "-     much time reading events and your event processing" << std::endl ;
  std::cout << "-     is fast, then you are limited by IO reading. The" << std::endl ;
  std::cout << "-     generated plot helps you understanding where your" << std::endl ;
  std::cout << "-     limit is given the processing time of a single event" << std::endl ;
  std::cout << "-     event and the number of available cores." << std::endl ;
  std::cout << "-" << std::endl ;
  std::cout << "-- Example:" << std::endl ;
  std::cout << "-     If you spend " << meanReadTime*2 << " ms processing an event in sequential" << std::endl ;
  std::cout << "-     mode, then using more than 2 cores is not useful (IO bound)" << std::endl ;
  std::cout << "-     In other words, open the plot from the generated root" << std::endl ;
  std::cout << "-     file. If you know your mean processing time (y axis)," << std::endl ;
  std::cout << "-     then the corresponding value on the dashed line tells" << std::endl ;
  std::cout << "-     you the maximum number of cores you should use to be" << std::endl ;
  std::cout << "-     efficient..." << std::endl ;
  std::cout << "-----------------------------------------------" << std::endl ;

  return 0 ;
}

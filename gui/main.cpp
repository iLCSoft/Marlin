#include <QApplication>

#include "marlin/Parser.h"
#include "marlin/ProcessorLoader.h"

#include "mainwindow.h"
#include <algorithm>

int main(int argc, char *argv[])
{
    std::cout << "\nLoading GUI...\n";

#ifndef MARLIN_NO_DLL

  //------ load shared libraries with processors ------
                                                                                                                                                            
  StringVec libs ;
  LCTokenizer t( libs, ':' ) ;
                                                                                                                                                            
  std::string marlinProcs("") ;
                                                                                                                                                            
  char * var =  getenv("MARLIN_DLL" ) ;
                                                                                                                                                            
  if( var != 0 ) {
    marlinProcs = var ;
  } else {
    std::cout << std::endl << " You have no MARLIN_DLL variable in your environment "
      " - so no processors will be loaded. ! " << std::endl << std::endl ;
  }
                                                                                                                                                            
  std::for_each( marlinProcs.begin(), marlinProcs.end(), t ) ;
                                                                                                                                                            
  ProcessorLoader loader( libs.begin() , libs.end()  ) ;
                                                                                                                                                            
  //------- end processor libs -------------------------
                                                                                                                                                            
#endif
	
    QApplication app(argc, argv);

    //set the selection color to blue instead of black ;)
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Highlight, QColor(0,90,126, 128) );
    QApplication::setPalette(pal);

    MainWindow window;

    if( argc == 2 ){ 
	window.setMarlinSteerCheck(argv[1]);
    }
    
    window.show();

    return app.exec();
}

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    std::cout << "\nPlease be patient, loading GUI...\n";
	
    QApplication app(argc, argv);
    
    app.setStyle("plastique");
    //app.setStyle("windowsxp");
    
    MainWindow window;

    if( argc == 2 ){ 
	window.setMarlinSteerCheck(argv[1]);
    }
    
    window.show();

    return app.exec();
}

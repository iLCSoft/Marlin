#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    std::cout << "\nPlease be patient, loading GUI...\n";
	
    QApplication app(argc, argv);

    //set the selection color to blue instead of black ;)
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Highlight, QColor(0,90,126) );
    QApplication::setPalette(pal);

    MainWindow window;

    if( argc == 2 ){ 
	window.setMarlinSteerCheck(argv[1]);
    }
    
    window.show();

    return app.exec();
}

#ifndef GUIHELP_H
#define GUIHELP_H

/**@ class
 * Small class for displaying Marlin GUI Help
 * 
 * @author Jan Engels, DESY
 * @version $Id: guihelp.h,v 1.1 2006-12-08 15:51:37 engels Exp $
 */

#include <QWidget>

class QTextBrowser;
class QString;

class GUIHelp : public QWidget
{
    Q_OBJECT

public:
    GUIHelp(QString path="/gui/help/index.html", QWidget* parent=0, Qt::WFlags f=0 );
    
private:
    QTextBrowser *browser;
};

#endif

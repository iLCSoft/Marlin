#ifndef ADDPROCDIALOG_H
#define ADDPROCDIALOG_H

/**@class
 * This class just shows a small dialog for adding a new processor
 *
 * @author Jan Engels, DESY
 * @version $Id: addprocdialog.h,v 1.4 2006-12-08 15:51:37 engels Exp $
 */


#include <QDialog>

#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QComboBox;
class QLineEdit;
class QLabel;
class QMainWindow;

using namespace marlin;

class APDialog : public QDialog
{
    Q_OBJECT

public:
    APDialog(MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

signals:
    void editProcessor(int);
    
    //apply changes
    void apply();
    
private slots:
    void addProcessor();
    void changeLabel(const QString& text);

private:
    //Variables
    ssMap procTypes;
    QMainWindow* _parent;
    QComboBox *cb;
    QLabel *procLabel;
    QLineEdit *le;
    QVBoxLayout *mainLayout; 
    
    MarlinSteerCheck* _msc;
};

#endif

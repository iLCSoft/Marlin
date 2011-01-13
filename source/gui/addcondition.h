#ifndef ADDCONDITION_H
#define ADDCONDITION_H

/**@class
 * This class just shows a small dialog for adding new processor conditions
 *
 * @author Jan Engels, DESY
 * @version $Id: addcondition.h,v 1.2 2006-12-08 15:51:37 engels Exp $
 */

#include <QDialog>

#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QLineEdit;
class QMainWindow;

using namespace marlin;

class ACDialog : public QDialog
{
    Q_OBJECT

public:
    ACDialog(MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

signals:
    void apply();
    
private slots:
    void addCondition();

private:
    //Variables
    QMainWindow* _parent;
    QLineEdit *le;
    QVBoxLayout *mainLayout; 
    
    MarlinSteerCheck* _msc;
};

#endif

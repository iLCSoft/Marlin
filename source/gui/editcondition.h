#ifndef EDITCONDITION_H
#define EDITCONDITION_H

/**@class
 * This class just shows a small dialog for editingprocessor conditions
 *
 * @author Jan Engels, DESY
 * @version $Id: editcondition.h,v 1.1 2006-12-12 10:39:30 engels Exp $
 */

#include <QDialog>

#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QLineEdit;
class QMainWindow;

using namespace marlin;

class ECDialog : public QDialog
{
    Q_OBJECT

public:
    ECDialog(MarlinSteerCheck* msc, const std::string& oldCond, QWidget *parent = 0, Qt::WFlags f = 0);

signals:
    void apply();
    
private slots:
    void editCondition();

private:
    //Variables
    QMainWindow* _parent;
    QLineEdit *le;
    QVBoxLayout *mainLayout; 
    
    MarlinSteerCheck* _msc;
    std::string _oldCond;
};

#endif

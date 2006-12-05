#ifndef ADDCONDITION_H
#define ADDCONDITION_H

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

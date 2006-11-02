#ifndef ADDPROCDIALOG_H
#define ADDPROCDIALOG_H

#include <QDialog>

#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QComboBox;
class QLineEdit;
class QMainWindow;

using namespace marlin;

class APDialog : public QDialog
{
    Q_OBJECT

public:
    APDialog(MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

signals:
    void editProcessor(int);
    
private slots:
    void addProcessor();

private:
    //Variables
    QMainWindow* _parent;
    QComboBox *cb;
    QLineEdit *le;
    QVBoxLayout *mainLayout; 
    
    MarlinSteerCheck* _msc;
};

#endif

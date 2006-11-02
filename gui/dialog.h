#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "marlin/CCProcessor.h"
#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;

using namespace marlin;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(CCProcessor* p, MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

private:
    void setupViews();
    
    //Variables
    QVBoxLayout *mainLayout; 
    
    CCProcessor* _p;
    MarlinSteerCheck* _msc;
};

#endif

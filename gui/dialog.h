#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "marlin/CCProcessor.h"
#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QTableWidget;

using namespace marlin;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(CCProcessor* p, MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

private slots:
    void apply();
    void cancel();
    void optParamChanged();
    
private:
    void setupViews();
    
    //Variables
    QVBoxLayout *mainLayout;
    QTableWidget *optParamTable;
    
    CCProcessor* _p;
    MarlinSteerCheck* _msc;
};

#endif

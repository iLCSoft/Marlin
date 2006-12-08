#ifndef DIALOG_H
#define DIALOG_H

/**@class
 * This class is responsible for editing a processor
 * 
 * @author Jan Engels, DESY
 * @version $Id: dialog.h,v 1.6 2006-12-08 15:51:37 engels Exp $
 */

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
    void help();
    void optParamChanged();
    
private:
    //init view
    void setupViews();
    
    //variables
    QVBoxLayout *mainLayout;
    QTableWidget *optParamTable;
    
    CCProcessor* _p;
    MarlinSteerCheck* _msc;
};

#endif

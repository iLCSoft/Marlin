#ifndef NPARAMVECSET_H
#define NPARAMVECSET_H

/**@class
 * This widget is for editing "normal" processor parameter values
 * of type vector with set_size != 0
 *
 * @author Jan Engels, DESY
 * @version $Id: nparamvecset.h,v 1.1 2006-12-14 14:39:23 engels Exp $
 */

#include "marlin/MarlinSteerCheck.h"
#include "marlin/CCProcessor.h"
#include "marlin/CMProcessor.h"

#include <QWidget>

class QVBoxLayout;
class QTableWidget;
class QDialog;

using namespace marlin;

class NParamVecSet : public QWidget
{
    Q_OBJECT

public:
    //constructor
    NParamVecSet(
	    MarlinSteerCheck* msc,
	    CCProcessor* p,
	    QTableWidget *parent,
	    QDialog *dialog
    );

public slots:
    void updateTable();

private slots:
    void addValSet();
    void remValSet();

private:
    //Variables
    MarlinSteerCheck* _msc;
    CCProcessor* _p;
    QTableWidget* _parent;
    QTableWidget* valTable;
    QDialog* _dialog;
    
    QVBoxLayout *mainLayout; 
};

#endif

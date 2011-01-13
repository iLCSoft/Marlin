#ifndef NPARAMVECSETD_H
#define NPARAMVECSETD_H

/**@ class
 * Small delegate class for managing "normal" processor parameter values
 * of type vector with set_size != 0
 *
 * @author Jan Engels, DESY
 * @version $Id: nparamvecsetd.h,v 1.1 2006-12-14 14:39:23 engels Exp $
 */

#include "marlin/MarlinSteerCheck.h"
#include "marlin/CCProcessor.h"

#include <QItemDelegate>

class QTableWidget;
class QWidget;

using namespace marlin;

class NParamVecSetD : public QItemDelegate
{
    Q_OBJECT

public:
    //constructor
    NParamVecSetD(
	    MarlinSteerCheck *msc,
	    CCProcessor *p,
	    QTableWidget* paramTable,
	    QObject* parent = 0
    );

    QWidget *createEditor(QWidget *parent) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    MarlinSteerCheck* _msc;
    CCProcessor* _p;
    QTableWidget* _paramTable;
};

#endif

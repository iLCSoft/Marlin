#ifndef PARAMDELEGATE_H
#define PARAMDELEGATE_H

/**@ class
 * Small delegate class for managing processor's parameters
 *
 * @author Jan Engels, DESY
 * @version $Id: paramdelegate.h,v 1.2 2006-12-08 15:51:37 engels Exp $
 */

#include <QItemDelegate>

#include "marlin/CCProcessor.h"

class QTableWidget;

using namespace marlin;

class ParamDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ParamDelegate(CCProcessor* p, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    CCProcessor* _p;
    QTableWidget* _parent;
};

#endif

#ifndef GPARAMDELEGATE_H
#define GPARAMDELEGATE_H

/**@ class
 * Small delegate class for changing global parameters
 *
 * @author Jan Engels, DESY
 * @version $Id: gparamdelegate.h,v 1.2 2006-12-08 15:51:37 engels Exp $
 */

#include <QItemDelegate>

#include "marlin/StringParameters.h"

class QTableWidget;
class QMainWindow;

using namespace marlin;

class GParamDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    GParamDelegate(StringParameters* p, QMainWindow* mw, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    StringParameters* _p;
    QMainWindow* _mw;
    QTableWidget* _parent;
};

#endif

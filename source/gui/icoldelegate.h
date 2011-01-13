#ifndef ICOLDELEGATE_H
#define ICOLDELEGATE_H

/**@ class
 * Delegate class for managing input collections of type StringVec
 *
 * @author Jan Engels, DESY
 * @version $Id: icoldelegate.h,v 1.2 2006-12-08 15:51:37 engels Exp $
 */

#include <QItemDelegate>

#include "marlin/CCProcessor.h"
#include "marlin/MarlinSteerCheck.h"

class QTableWidget;

using namespace marlin;

class IColDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    IColDelegate(CCProcessor* p, MarlinSteerCheck* msc, const QString& name, const QString& type, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

public slots:
    void addCollection();
    void remCollection();

private:
    std::string _name;
    std::string _type;
    CCProcessor* _p;
    MarlinSteerCheck* _msc;
    QTableWidget* _parent;
};

#endif

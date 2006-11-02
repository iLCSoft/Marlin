#ifndef ICOLTDELEGATE_H
#define ICOLTDELEGATE_H

#include <QItemDelegate>

#include "marlin/CCProcessor.h"
#include "marlin/MarlinSteerCheck.h"

class QTableWidget;

using namespace marlin;

class IColTDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    IColTDelegate(CCProcessor* p, MarlinSteerCheck* msc, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    CCProcessor* _p;
    MarlinSteerCheck* _msc;
    QTableWidget* _parent;
};

#endif

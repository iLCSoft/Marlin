#ifndef PARAMDELEGATE_H
#define PARAMDELEGATE_H

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

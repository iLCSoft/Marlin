#ifndef APROCDELEGATE_H
#define APROCDELEGATE_H

#include <QItemDelegate>

class AProcDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    AProcDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    QObject* _parent;
};

#endif

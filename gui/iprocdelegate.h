#ifndef IPROCDELEGATE_H
#define IPROCDELEGATE_H

#include <QItemDelegate>

class IProcDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    IProcDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    QObject* _parent;
};

#endif

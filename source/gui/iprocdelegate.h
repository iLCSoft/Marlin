#ifndef IPROCDELEGATE_H
#define IPROCDELEGATE_H

/**@ class
 * Small delegate class for changing inactive processor's names
 *
 * @author Jan Engels, DESY
 * @version $Id: iprocdelegate.h,v 1.2 2006-12-08 15:51:37 engels Exp $
 */

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

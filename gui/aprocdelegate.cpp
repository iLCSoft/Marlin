#include <QtGui>

#include "aprocdelegate.h"

AProcDelegate::AProcDelegate(QObject *parent) : QItemDelegate(parent){
    _parent=parent;
}

QWidget *AProcDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    
    QLineEdit *edit = new QLineEdit(parent);

    if( index.column() == 1 ){
	edit->setReadOnly(true);
    }
    else{
	connect(edit, SIGNAL(editingFinished()), _parent, SLOT(aProcNameChanged()));
    }

    return edit;
}

void AProcDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(index.model()->data(index).toString());
}

void AProcDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    model->setData(index, edit->displayText());
}

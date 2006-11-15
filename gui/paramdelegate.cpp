#include <QtGui>

#include "paramdelegate.h"

ParamDelegate::ParamDelegate(CCProcessor* p, QObject *parent) : QItemDelegate(parent){
    _parent = qobject_cast<QTableWidget *>(parent);
    _p=p;
}

QWidget *ParamDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    QLineEdit *edit = new QLineEdit(parent);
    
    if( index.column() == 0 ){
	edit->setReadOnly(true);
	return edit;
    }
    return edit;
}

void ParamDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(index.model()->data(index).toString());
}

void ParamDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    model->setData(index, edit->displayText());
    
    //update the processor parameter's
    StringVec value;
    //save the value
    value.push_back(edit->displayText().toStdString());
    //erase the old parameter
    _p->getParameters()->erase( _parent->item( index.row(), 0)->text().toStdString() );
    //add a new parameter with updated values
    _p->getParameters()->add( _parent->item( index.row(), 0)->text().toStdString(), value );
}


#include <QtGui>

#include "gparamdelegate.h"

GParamDelegate::GParamDelegate(StringParameters* p, QMainWindow *mw, QObject *parent) : QItemDelegate(parent), _mw(mw) {
    _parent = qobject_cast<QTableWidget *>(parent);
    _p=p;
}

QWidget *GParamDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    QLineEdit *edit = new QLineEdit(parent);
    
    if( index.column() == 0 ){
	edit->setReadOnly(true);
    }
    else{
	connect(edit, SIGNAL(editingFinished()), _mw, SIGNAL(modifiedContent()));
    }
   
    return edit;
}

void GParamDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(index.model()->data(index).toString());
}

void GParamDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    model->setData(index, edit->displayText());

    //update the parameter's
    StringVec value;
    //save the value
    value.push_back(edit->displayText().toStdString());
    //erase the old parameter
    _p->erase( _parent->item( index.row(), 0)->text().toStdString() );
    //add a new parameter with updated values
    _p->add( _parent->item( index.row(), 0)->text().toStdString(), value );

}


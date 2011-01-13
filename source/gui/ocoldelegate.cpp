#include <QtGui>

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "ocoldelegate.h"

OColDelegate::OColDelegate(CCProcessor* p, MarlinSteerCheck *msc, QObject *parent) : QItemDelegate(parent){
    _parent = qobject_cast<QTableWidget *>(parent);
    _p=p;
    _msc=msc;
}

QWidget *OColDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    
    QLineEdit *edit = new QLineEdit(parent);
    if( index.column() != 2){
	edit->setReadOnly(true);
    }
    return edit;
}

void OColDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(index.model()->data(index).toString());
    
    if( _p->isActive() ){
	//set background color
	_parent->currentItem()->setBackgroundColor(
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), edit->text().toStdString() ) ?
		    QColor(184,16,0,180) : QColor(32,140,64,180) );
    }
}

void OColDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    model->setData(index, edit->displayText());
    
    //update the processor
    _p->getCols( OUTPUT, _parent->item(index.row(),0)->text().toStdString() )[ 0 ]->setValue(edit->displayText().toStdString());
    _msc->consistencyCheck();

    if( _p->isActive() ){
	//set background color
	_parent->currentItem()->setBackgroundColor(
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), edit->text().toStdString() ) ?
		    QColor(184,16,0,180) : QColor(32,140,64,180) );
    }
}


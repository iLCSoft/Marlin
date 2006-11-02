#include <QtGui>

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "icoldelegate.h"

IColDelegate::IColDelegate(CCProcessor* p, MarlinSteerCheck* msc, const QString& name, const QString& type, QObject *parent) : QItemDelegate(parent){
    _name=name.toStdString();
    _type=type.toStdString();
    _parent = qobject_cast<QTableWidget *>(parent);
    _p=p;
    _msc=msc;
}

QWidget *IColDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    QComboBox *comboBox = new QComboBox(parent);
    
    sSet colsSet=_msc->getColsSet( _type, _p );
    
    for( sSet::const_iterator p=colsSet.begin(); p != colsSet.end(); p++ ){
	comboBox->addItem((*p).c_str());
    }
    comboBox->setEditable(true);
    comboBox->setAutoCompletion(true);

    return comboBox;
}

void IColDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;
                                                                                                                                                             
    int pos = comboBox->findText(index.model()->data(index).toString(), Qt::MatchExactly);
    if( pos != -1){
	comboBox->setCurrentIndex(pos);
    }
    else{
	comboBox->addItem(index.model()->data(index).toString());
	int pos = comboBox->findText(index.model()->data(index).toString(), Qt::MatchExactly);
	comboBox->setCurrentIndex(pos);
    }

    //set background color
    _parent->currentItem()->setBackgroundColor( _p->isErrorCol( comboBox->currentText().toStdString() ) ? QColor(184,16,0,180) : QColor(32,140,64,180) );
}

void IColDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;
																			 
    model->setData(index, comboBox->currentText());

    //update the processor
    _p->getCols( INPUT, _name )[ index.row() ]->setValue( comboBox->currentText().toStdString() );
    _msc->consistencyCheck();
    
    _parent->currentItem()->setBackgroundColor( _p->isErrorCol( comboBox->currentText().toStdString() ) ? QColor(184,16,0,180) : QColor(32,140,64,180) );
}

void IColDelegate::addCollection(){

    //update the table
    int row = _parent->rowCount();
    _parent->setRowCount(row + 1);
    
    QTableWidgetItem *item0 = new QTableWidgetItem( _msc->getColsSet( _type, _p ).size() != 0 ? 
			(* _msc->getColsSet( _type, _p ).begin()).c_str() : "No Suitable Collection Found" );
    
    //update the processor
    _p->addCol( INPUT, _name, _type, _msc->getColsSet( _type, _p ).size() != 0 ?
		(* _msc->getColsSet( _type, _p ).begin()).c_str() : "No Suitable Collection Found" );
    _msc->consistencyCheck();

    //set background color
    item0->setBackgroundColor( _p->isErrorCol( item0->text().toStdString() ) ? QColor(184,16,0,180) : QColor(32,140,64,180) );

    _parent->setItem(row, 0, item0);
}

void IColDelegate::remCollection(){

    int row = _parent->currentRow();
    if( row >= 0 && row < _parent->rowCount() ){
	
	//update the table
	_parent->removeRow( row );

	//update the processor
        _p->remCol( INPUT, _name, row );
    }
}


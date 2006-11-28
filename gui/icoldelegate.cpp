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
    if( index.column() == 0 ){
	QComboBox *comboBox = new QComboBox(parent);
	
	sSet colsSet=_msc->getColsSet( _type, _name, _p );
	
	for( sSet::const_iterator p=colsSet.begin(); p != colsSet.end(); p++ ){
	    comboBox->addItem((*p).c_str());
	}
	comboBox->setEditable(true);
	comboBox->setAutoCompletion(true);

	return comboBox;
    }
    return NULL;
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
    if( _p->isActive() ){
	//set background color
	_parent->currentItem()->setBackgroundColor( _p->isErrorCol( _type, comboBox->currentText().toStdString() ) ?
		QColor(184,16,0,180) : QColor(32,140,64,180) );

	//set background color from (combobox)
	QPalette pal = comboBox->palette();
	pal.setColor(QPalette::Base, _p->isErrorCol( _type, comboBox->currentText().toStdString() ) ?
		QColor(164,32,16) : QColor(32,140,64) );
	pal.setColor(QPalette::Text, QColor(255,255,255) );
	comboBox->setPalette(pal);
    }
}

void IColDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox) return;
																			 
    model->setData(index, comboBox->currentText());

    //update the processor
    _p->getCols( INPUT, _name )[ index.row() ]->setValue( comboBox->currentText().toStdString() );
    _msc->consistencyCheck();
    
    if( _p->isActive() ){
	//set background color
	_parent->currentItem()->setBackgroundColor( _p->isErrorCol( _type, comboBox->currentText().toStdString() ) ?
		QColor(184,16,0,180) : QColor(32,140,64,180) );

	//set background color from (combobox)
	QPalette pal = comboBox->palette();
	pal.setColor(QPalette::Base, _p->isErrorCol( _type, comboBox->currentText().toStdString() ) ?
		QColor(164,32,16) : QColor(32,140,64) );
	pal.setColor(QPalette::Text, QColor(255,255,255) );
	comboBox->setPalette(pal);
    }
}

void IColDelegate::addCollection(){

    //update the table
    int row = _parent->rowCount();
    _parent->setRowCount(row + 1);
    
    QTableWidgetItem *item0 = new QTableWidgetItem( _msc->getColsSet( _type, _name, _p ).size() != 0 ? 
			(* _msc->getColsSet( _type, _name, _p ).begin()).c_str() : "No_Suitable_Collection_Found" );
    
    //update the processor
    _p->addCol( INPUT, _name, _type, _msc->getColsSet( _type, _name, _p ).size() != 0 ?
		(* _msc->getColsSet( _type, _name, _p ).begin()).c_str() : "No_Suitable_Collection_Found" );
    _msc->consistencyCheck();

    if( _p->isActive() ){
	//set background color
	item0->setBackgroundColor( _p->isErrorCol( _type, item0->text().toStdString() ) ?
		QColor(184,16,0,180) : QColor(32,140,64,180) );
    }
    _parent->setItem(row, 0, item0);
    _parent->setCurrentItem(item0);
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

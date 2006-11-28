#include <QtGui>

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "icoltdelegate.h"

IColTDelegate::IColTDelegate(CCProcessor* p, MarlinSteerCheck* msc, QObject *parent) : QItemDelegate(parent){
    _parent = qobject_cast<QTableWidget *>(parent);
    _p=p;
    _msc=msc;
}

QWidget *IColTDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const
{
    QLineEdit *edit = new QLineEdit(parent);
    QComboBox *comboBox = new QComboBox(parent);
    
    if( index.column() != 2 ){
        edit->setReadOnly(true);
	return edit;
    }
    else{
	sSet colsSet=_msc->getColsSet( _parent->item(index.row(),1)->text().toStdString(),
		_parent->item(index.row(),0)->text().toStdString() , _p );
	
	for( sSet::const_iterator p=colsSet.begin(); p != colsSet.end(); p++ ){
	    comboBox->addItem((*p).c_str());
	}
	comboBox->setEditable(true);
	comboBox->setAutoCompletion(true);

	return comboBox;
    }
    return NULL;
}

void IColTDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    if( index.column() != 2 ){
	QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    	edit->setText(index.model()->data(index).toString());
    }
    else{
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
	
	if(_p->isActive()){
	    //set background color
	    _parent->currentItem()->setBackgroundColor( 
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), comboBox->currentText().toStdString() ) ?
		    QColor(184,16,0,180) : QColor(32,140,64,180) );
	    
	    //set background color (combobox)
	    QPalette pal = comboBox->palette();
	    pal.setColor(QPalette::Base, 
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), comboBox->currentText().toStdString() ) ?
		    QColor(164,32,16) : QColor(32,140,64) );
	    pal.setColor(QPalette::Text, QColor(255,255,255) );
	    comboBox->setPalette(pal);
	}
    }
}

void IColTDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if( index.column() == 2 ){

	QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
	if (!comboBox) return;
																			     
	model->setData(index, comboBox->currentText());

	//update the processor
	_p->getCols( INPUT, _parent->item(index.row(),0)->text().toStdString() )[ 0 ]->setValue( comboBox->currentText().toStdString() );
	_msc->consistencyCheck();

	if(_p->isActive()){
	    //set background color
	    _parent->currentItem()->setBackgroundColor( 
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), comboBox->currentText().toStdString() ) ? 
		    QColor(184,16,0,180) : QColor(32,140,64,180) );

	    //set background color from (combobox)
	    QPalette pal = comboBox->palette();
	    pal.setColor(QPalette::Base, 
		    _p->isErrorCol( _parent->item(index.row(),1)->text().toStdString(), comboBox->currentText().toStdString() ) ? 
		    QColor(164,32,16) : QColor(32,140,64) );
	    pal.setColor(QPalette::Text, QColor(255,255,255) );
	    comboBox->setPalette(pal);
	}
    }
}


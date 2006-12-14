#include <QtGui>

#include "marlin/CMProcessor.h"
#include "nparamvecsetd.h"

NParamVecSetD::NParamVecSetD(
	MarlinSteerCheck* msc,
	CCProcessor* p,
	QTableWidget* paramTable,
	QObject *parent
	):
	QItemDelegate(parent),
	_msc(msc),
	_p(p),
	_paramTable(paramTable)
{
}

QWidget *NParamVecSetD::createEditor(QWidget *parent) const
{
    QLineEdit *edit = new QLineEdit(parent);

    return edit;
}

void NParamVecSetD::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if( !index.model()->data(index).toString().isEmpty() ){
	edit->setText(index.model()->data(index).toString());
    }
    else{
	edit->setText("0");
    }
}

void NParamVecSetD::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if( !edit->displayText().isEmpty() ){
	model->setData(index, edit->displayText());
    }
    else{
	model->setData(index, "0");
    }
    
    //get the parameter's values
    std::string key=_paramTable->item(_paramTable->currentRow(), 0)->text().toStdString();
    StringVec values;
    _p->getParameters()->getStringVals( key, values );

    //get the set size
    int ssize= _msc->getMProcs()->getParamSetSize(_p->getType(), key );

    //update the changed value
    if( !edit->displayText().isEmpty() ){
	values[ (index.row()*ssize)+index.column() ]=edit->displayText().toStdString();
    }
    else{
	values[ (index.row()*ssize)+index.column() ]="0";
	edit->setText("0");
    }
    
    //erase the parameter
    _p->getParameters()->erase( key );
     
    //add the parameter with updated values
    _p->getParameters()->add( key, values );

    //update param table
    QString val;
    for( unsigned int i=0; i<values.size(); i++ ){
	val += values[i].c_str();
	val += " ";
    }
	
    _paramTable->item(_paramTable->currentRow(), 1)->setText(val);
}


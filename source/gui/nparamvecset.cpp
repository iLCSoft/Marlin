#include <QtGui>

#include "nparamvecset.h"
#include "nparamvecsetd.h"

using namespace std;

//constructor
NParamVecSet::NParamVecSet(
	MarlinSteerCheck* msc,
	CCProcessor* p,
	QTableWidget *parent,
	QDialog *dialog
    ):
	_msc(msc),
	_p(p),
	_parent(parent),
	_dialog(dialog)
{
    valTable = new QTableWidget;

    valTable->verticalHeader()->hide();
    valTable->setSelectionMode(QAbstractItemView::NoSelection);
    valTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

    QPushButton *addButton = new QPushButton(tr("Add"));
    QPushButton *remButton = new QPushButton(tr("Rem"));

    addButton->setAutoDefault( false );
    remButton->setAutoDefault( false );

    addButton->setToolTip(tr("Add New Row"));
    remButton->setToolTip(tr("Remove Selected Row"));
																		 
    connect(addButton, SIGNAL(clicked()), this, SLOT(addValSet()));
    connect(remButton, SIGNAL(clicked()), this, SLOT(remValSet()));

    QVBoxLayout *buttonsLO = new QVBoxLayout;
    buttonsLO->addWidget(addButton);
    buttonsLO->addWidget(remButton);
    
    QWidget *buttons = new QWidget;
    buttons->setLayout(buttonsLO);

    //layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget( valTable, 0, 0 );
    layout->addWidget( buttons, 0, 1, Qt::AlignTop | Qt::AlignRight );

    //group box
    QGroupBox *groupBox = new QGroupBox(tr("Edit Parameter Values"));
    groupBox->setLayout(layout);
    groupBox->setMinimumWidth(550);
    groupBox->setMaximumHeight(600);
    
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget( groupBox );

    setLayout( mainLayout );
    this->hide();
}

void NParamVecSet::updateTable(){
    string _key = _parent->item(_parent->currentRow(), 0)->text().toStdString();
    string _val = _parent->item(_parent->currentRow(), 1)->text().toStdString();
    int _ssize = _msc->getMProcs()->getParamSetSize(_p->getType(), _key);
    
    valTable->setColumnCount(0);
    valTable->setRowCount(0);
    if( _ssize != 0 ){
	this->show();
	
	valTable->setColumnCount(_ssize);

	for( int i=0; i<_ssize; i++ ){
	    valTable->horizontalHeader()->resizeSection(i, 90);
	}
    

	//initialize table
	StringVec tokens;
	_msc->getMProcs()->tokenize( _val, tokens );

	if(( tokens.size() % _ssize ) != 0 ){
	    cerr << "Parameter [" << _key << "] from processor [" << _p->getName() << "] [" << _p->getType() << 
		"]... Has an error: Number of parameters do not fit for the Set Size specified!!\n";
	}
	
	for( unsigned int i=0; i<tokens.size()/_ssize; i++ ){
	    int row = valTable->rowCount();
	    valTable->setRowCount(row + 1);
	    for( int j=0; j<_ssize; j++ ){
		QTableWidgetItem *item = new QTableWidgetItem( tokens[ (i*_ssize)+j ].c_str() );

		item->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), _key ).c_str() ));
		valTable->setItem(row, j, item);
	    }
	}
	
	//delegate
	QItemDelegate *d = new NParamVecSetD( _msc, _p, _parent, valTable );
	valTable->setItemDelegate(d);
    }
    else{
	this->hide();
    }
}

void NParamVecSet::addValSet(){

    string _key = _parent->item(_parent->currentRow(), 0)->text().toStdString();
    int _ssize = _msc->getMProcs()->getParamSetSize(_p->getType(), _key);

    int row = valTable->rowCount();
    valTable->setRowCount(row + 1);
    
    StringVec newVals;
    
    for( int i=0; i<_ssize; i++ ){
	QTableWidgetItem *item = new QTableWidgetItem( "0" );
	valTable->setItem(row, i, item);
	newVals.push_back("0");
    }
    
    _p->getParameters()->add( _key, newVals );
}

void NParamVecSet::remValSet(){

    if(valTable->currentRow() != -1 ){

	string _key = _parent->item(_parent->currentRow(), 0)->text().toStdString();
	int _ssize = _msc->getMProcs()->getParamSetSize(_p->getType(), _key);

	StringVec vals, newVals;
	string text;
	_p->getParameters()->getStringVals( _key, vals );

	//if it is the last parameter being removed just deactivate the optional parameter
	if( (int)vals.size() == _ssize && _msc->getMProcs()->isParamOpt(_p->getType(), _key) ){
	    _p->setOptionalParam( _key, true );
	    _parent->item(_parent->currentRow(), 2)->setCheckState( Qt::Unchecked );
	    return;
	}
	//prevent the last parameter to be removed
	if( (int)vals.size() == _ssize && !_msc->getMProcs()->isParamOpt(_p->getType(), _key) ){
	    return;
	}

	int ret = QMessageBox::warning(this, tr("Remove Row"),
		tr("Do you want to delete the entire row?"),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No);
                                                                                                                                                             
	if( ret == QMessageBox::No ){
	    return;
	}

	for( int i=0; i<(int)vals.size(); i++ ){
	    if( i < (valTable->currentRow()*_ssize) || i > ((valTable->currentRow()*_ssize)+(_ssize-1)) ){
		newVals.push_back(vals[i]);
		text+=vals[i];
		text+=" ";
	    }
	}
	
	_p->getParameters()->erase( _key );
	_p->getParameters()->add( _key, newVals );

	_parent->item(_parent->currentRow(), 1)->setText( text.c_str() );
	updateTable();
    }
}

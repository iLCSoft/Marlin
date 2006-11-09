#include <QtGui>

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "dialog.h"
#include "paramdelegate.h"
#include "icoldelegate.h"
#include "icoltdelegate.h"
#include "ocoldelegate.h"

#include "flowlayout.h"

using namespace std;

Dialog::Dialog( CCProcessor *p, MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QDialog(parent,f), _p(p), _msc(msc)
{
    mainLayout = new QVBoxLayout;
    
    setupViews();
 
    setLayout(mainLayout);

    QString title;
    title+=tr("Edit Processor - Name: ");
    title+=p->getName().c_str();
    title+=tr(" - Type: ");
    title+=p->getType().c_str();
    setWindowTitle(title);
}

void Dialog::setupViews()
{
    //PROCESSOR DESCRIPTION
    QLabel *desc = new QLabel(_p->getDescription().c_str());
    desc->setFont( QFont("Helvetica", 10, QFont::Bold) );
    
    QPalette pal = desc->palette();
    pal.setColor(QPalette::WindowText, QColor(32,64,140,220) );
    desc->setPalette(pal);
		
    desc->setWordWrap(true);

    QVBoxLayout *descLO = new QVBoxLayout;
    descLO->addWidget( desc );
                                                                                                                                                             
    QGroupBox *descGB = new QGroupBox(tr("Processor Description "), this);
    descGB->setLayout( descLO );

    mainLayout->addWidget(descGB);
    
    //////////////////////////////
    //INTPUT COLLECTIONS TABLE
    //////////////////////////////
    ssMap colHeaders = _p->getColHeaders( INPUT );
  
    if( colHeaders.size() != 0 ){
	
	QTableWidget *colTable = new QTableWidget;
		
	QStringList labels;
	labels << tr("Name") << tr("Type") << tr("Value");
	colTable->setColumnCount(3);
	colTable->verticalHeader()->hide();
	colTable->setHorizontalHeaderLabels(labels);
	colTable->horizontalHeader()->resizeSection(0, 300);
	colTable->horizontalHeader()->resizeSection(1, 300);
	colTable->horizontalHeader()->resizeSection(2, 300);
	colTable->setSelectionMode(QAbstractItemView::NoSelection);
	colTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

	bool found=false;
	
	for( ssMap::const_iterator p=colHeaders.begin(); p!=colHeaders.end(); p++ ){
	    //initialize table
	    ColVec cols = _p->getCols( INPUT , (*p).first );
	    for( unsigned int i=0; i<cols.size(); i++ ){
		if( _p->getParamType( (*p).first ) == "string" ){

		    found=true;
		    
		    int row = colTable->rowCount();
		    colTable->setRowCount(row + 1);
		    
		    QTableWidgetItem *item0 = new QTableWidgetItem( (*p).first.c_str() );
		    QTableWidgetItem *item1 = new QTableWidgetItem( (*p).second.c_str() );
		    QTableWidgetItem *item2 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		    
		    item0->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		    item1->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		    item2->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		    
		    item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
		    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
		    
		    item2->setBackgroundColor( _p->isErrorCol( cols[i]->getValue().c_str() ) ? QColor(184,16,0,180) : QColor(32,140,64,180) );
	   
		    colTable->setItem(row, 0, item0);
		    colTable->setItem(row, 1, item1);
		    colTable->setItem(row, 2, item2);
		}
	    }
	}
	if(found){
	    //delegate
	    QItemDelegate *delegate = new IColTDelegate(_p, _msc, colTable);
	    colTable->setItemDelegate(delegate);
	    
	    //Layout
	    QVBoxLayout *colsTLayout = new QVBoxLayout;
	    colsTLayout->addWidget(colTable);
	    
	    //create group box for all collections
	    QGroupBox *colsTGroupBox = new QGroupBox(tr("INPUT COLLECTIONS - SINGLE VALUE"));
	    colsTGroupBox->setLayout(colsTLayout);
	    colsTGroupBox->setMaximumHeight(160);
	    
	    //add group box to main layout
	    mainLayout->addWidget(colsTGroupBox);
	}
    }
    
    //////////////////////////////
    //INPUT COLLECTIONS GROUPS
    //////////////////////////////

    colHeaders = _p->getColHeaders( INPUT );
  
    if( colHeaders.size() != 0 ){
	
	FlowLayout *fLayout = new FlowLayout;

	bool found = false;
	
	for( ssMap::const_iterator p=colHeaders.begin(); p!=colHeaders.end(); p++ ){
	    
	    if( _p->getParamType( (*p).first ) == "StringVec" ){

		found = true;
		
		QTableWidget *colTable = new QTableWidget;
		
		colTable->setColumnCount(1);
		colTable->horizontalHeader()->resizeSection(0, 290);
		colTable->horizontalHeader()->hide();
		colTable->verticalHeader()->hide();
		//colTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		colTable->setSelectionMode(QAbstractItemView::NoSelection);
		colTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

		//initialize table
		ColVec cols = _p->getCols( INPUT , (*p).first );
		
		for( unsigned int i=0; i<cols.size(); i++ ){
		    int row = colTable->rowCount();
		    colTable->setRowCount(row + 1);
		    
		    QTableWidgetItem *item0 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		    
		    item0->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		   
		    item0->setBackgroundColor( _p->isErrorCol( cols[i]->getValue().c_str() ) ? QColor(184,16,0,180) : QColor(32,140,64,180) );

		    //item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
		    colTable->setItem(row, 0, item0);
		    //colTable->openPersistentEditor(item0);
		}

		//delegate
		QItemDelegate *delegate = new IColDelegate(_p, _msc, (*p).first.c_str(), (*p).second.c_str(), colTable);
		colTable->setItemDelegate(delegate);
	  
		//////////////////////////////
		//COLLECTIONS BUTTONS
		//////////////////////////////
	     
		QPushButton *addColButton = new QPushButton(tr("Add New Collection"));
		QPushButton *remColButton = new QPushButton(tr("Remove Selected Collection"));

		connect(addColButton, SIGNAL(clicked()), delegate, SLOT(addCollection()));
		connect(remColButton, SIGNAL(clicked()), delegate, SLOT(remCollection()));
		addColButton->setAutoDefault( false );
		remColButton->setAutoDefault( false );
		    
		//Buttons Layout
		QHBoxLayout *colButtonsLayout = new QHBoxLayout;
		colButtonsLayout->addWidget(addColButton);
		colButtonsLayout->addWidget(remColButton);
		
		//GroupBox
		QWidget *colButtons = new QWidget;
		colButtons->setLayout(colButtonsLayout);
		 
		//Layout
		QVBoxLayout *colLayout = new QVBoxLayout;
		colLayout->addWidget(colTable);
		colLayout->addWidget(colButtons);

		//set the title for this collection group
		QString colTitle("Name: [");
		colTitle+= (*p).first.c_str();
		colTitle+= "] - Type: [";
		colTitle+= (*p).second.c_str();
		colTitle+= "]";
		
		QGroupBox *colGroupBox = new QGroupBox( colTitle );
		colGroupBox->setLayout( colLayout );
		colGroupBox->setMinimumWidth(350);
		colGroupBox->setMaximumHeight(180);

		//add this collection to the group
		fLayout->addWidget(colGroupBox);
	    }
	}
	if(found){
	    //create group box for all collections
	    QGroupBox *colsGroupBox = new QGroupBox(tr("INPUT COLLECTIONS - MULTIPLE VALUES"));
	    colsGroupBox->setLayout(fLayout);
	    
	    //add group box to main layout
	    mainLayout->addWidget(colsGroupBox);
	}
    }


    //////////////////////////////
    //OUTPUT COLLECTIONS TABLE
    //////////////////////////////
    colHeaders = _p->getColHeaders( OUTPUT );
  
    if( colHeaders.size() != 0 ){
	
	QTableWidget *colTable = new QTableWidget;
		
	QStringList labels;
	labels << tr("Name") << tr("Type") << tr("Value");
	colTable->setColumnCount(3);
	colTable->verticalHeader()->hide();
	colTable->setHorizontalHeaderLabels(labels);
	colTable->horizontalHeader()->resizeSection(0, 300);
	colTable->horizontalHeader()->resizeSection(1, 300);
	colTable->horizontalHeader()->resizeSection(2, 300);
	colTable->setSelectionMode(QAbstractItemView::NoSelection);
	colTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

	for( ssMap::const_iterator p=colHeaders.begin(); p!=colHeaders.end(); p++ ){
	    //initialize table
	    ColVec cols = _p->getCols( OUTPUT , (*p).first );
	    for( unsigned int i=0; i<cols.size(); i++ ){
		int row = colTable->rowCount();
		colTable->setRowCount(row + 1);
		
		QTableWidgetItem *item0 = new QTableWidgetItem( (*p).first.c_str() );
		QTableWidgetItem *item1 = new QTableWidgetItem( (*p).second.c_str() );
		QTableWidgetItem *item2 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		
		item0->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		item1->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		item2->setToolTip( QString(_p->getParamDesc( (*p).first ).c_str() ) );
		
		item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);	
		item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);	
	
		colTable->setItem(row, 0, item0);
		colTable->setItem(row, 1, item1);
		colTable->setItem(row, 2, item2);
		
		//colTable->openPersistentEditor(item2);
	    }
	}		
	//delegate
	QItemDelegate *delegate = new OColDelegate(_p, colTable);
	colTable->setItemDelegate(delegate);
	
	//Layout
	QVBoxLayout *colsLayout = new QVBoxLayout;
	colsLayout->addWidget(colTable);
	
	//create group box for all collections
	QGroupBox *colsGroupBox = new QGroupBox(tr("OUTPUT COLLECTIONS"));
	colsGroupBox->setLayout(colsLayout);
	colsGroupBox->setMaximumHeight(160);
	
	//add group box to main layout
	mainLayout->addWidget(colsGroupBox);
    }
   

    //////////////////////////////
    //PARAMETERS TABLE
    //////////////////////////////
    if(_p->hasParameters()){
	QTableWidget *paramTable = new QTableWidget;
																				 
	QStringList labels;
	labels << tr("Parameter Name") << tr("Parameter Value");
	paramTable->setColumnCount(2);
	paramTable->verticalHeader()->hide();
	paramTable->setHorizontalHeaderLabels(labels);
	paramTable->horizontalHeader()->resizeSection(0, 400);
	paramTable->horizontalHeader()->resizeSection(1, 500);
	paramTable->setSelectionMode(QAbstractItemView::NoSelection);
	paramTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

	//initialize table
	StringVec paramKeys;
	_p->getParameters()->getStringKeys(paramKeys);
	for( unsigned int i=0; i<paramKeys.size(); i++ ){
	
	    int row = paramTable->rowCount();
	    paramTable->setRowCount(row + 1);
																				 
	    StringVec paramValues;
	    _p->getParameters()->getStringVals(paramKeys[i], paramValues);
	    
	    QString str;
	    
	    for( unsigned int j=0; j<paramValues.size(); j++ ){
		str+=paramValues[j].c_str();
		str+=" ";
	    }
	    
	    QTableWidgetItem *item0 = new QTableWidgetItem( paramKeys[i].c_str() );
	    QTableWidgetItem *item1 = new QTableWidgetItem( str );
	    
	    item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);

	    item0->setToolTip( QString(_p->getParamDesc( paramKeys[i] ).c_str() ) );
	    item1->setToolTip( QString(_p->getParamDesc( paramKeys[i] ).c_str() ) );
	    
	    paramTable->setItem(row, 0, item0);
	    paramTable->setItem(row, 1, item1);

	    //paramTable->openPersistentEditor(item1);
	}
	
	//Delegate
	ParamDelegate *pDelegate = new ParamDelegate(_p, paramTable);
	paramTable->setItemDelegate( pDelegate );
																				 
	//Layout
	QVBoxLayout *paramLayout = new QVBoxLayout;
	paramLayout->addWidget(paramTable);

	//GroupBox
	QGroupBox *paramGroupBox = new QGroupBox(tr("Processor Parameters"));
	paramGroupBox->setLayout(paramLayout);
	
	mainLayout->addWidget(paramGroupBox);
    }

    //////////////////////////////
    // APPLY, CANCEL BUTTONS
    //////////////////////////////

    QPushButton *applyButton = new QPushButton(tr("&Apply Changes"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    applyButton->setAutoDefault( false );
    cancelButton->setAutoDefault( false );
	
    //Buttons Layout
    QHBoxLayout *mainButtonsLayout = new QHBoxLayout;
    mainButtonsLayout->addWidget(applyButton);
    mainButtonsLayout->addWidget(cancelButton);
    
    //GroupBox
    QWidget *mainButtons = new QWidget;
    mainButtons->setLayout(mainButtonsLayout);
    
    mainLayout->addWidget(mainButtons);
}

void Dialog::apply(){
    accept();
}

void Dialog::cancel(){
    reject();
}

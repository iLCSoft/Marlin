#include <QtGui>

#include "marlin/CCProcessor.h"
#include "marlin/CMProcessor.h"
#include "marlin/CCCollection.h"

#include "dialog.h"
#include "nparamvecset.h"
#include "paramdelegate.h"
#include "icoldelegate.h"
#include "icoltdelegate.h"
#include "ocoldelegate.h"
#include "guihelp.h"

#include "flowlayout.h"

using namespace std;

Dialog::Dialog( CCProcessor *p, MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QDialog(parent,f), _p(p), _msc(msc)
{
    QShortcut *helpF1 = new QShortcut(Qt::Key_F1, this);
    connect(helpF1, SIGNAL(activated()), this, SLOT(help()));
    
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

void Dialog::help(){
    QWidget *help= new GUIHelp("/gui/help/html/editprocessor.html",this, Qt::Dialog );
    help->show();
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
    descGB->setMaximumHeight( 80 );
    descGB->setLayout( descLO );

    mainLayout->addWidget(descGB, Qt::AlignTop );
    
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
		if( !_msc->getMProcs()->isParamVec( _p->getType(), (*p).first )){

		    found=true;
		    
		    int row = colTable->rowCount();
		    colTable->setRowCount(row + 1);
		    
		    QTableWidgetItem *item0 = new QTableWidgetItem( (*p).first.c_str() );
		    QTableWidgetItem *item1 = new QTableWidgetItem( (*p).second.c_str() );
		    QTableWidgetItem *item2 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		    
		    item0->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		    item1->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		    item2->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		    
		    item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
		    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
		    
		    if( _p->isActive() ){
			item2->setBackgroundColor( _p->isErrorCol( p->second.c_str(), cols[i]->getValue().c_str() ) ?
				QColor(184,16,0,180) : QColor(32,140,64,180) );
		    }
	   
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
	    colsTGroupBox->setMaximumHeight(220);
	    
	    //add group box to main layout
	    mainLayout->addWidget(colsTGroupBox, Qt::AlignTop);
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
	    
	    if( _msc->getMProcs()->isParamVec( _p->getType(), (*p).first )){

		found = true;
		
		QTableWidget *colTable = new QTableWidget;
		
		colTable->setColumnCount(1);
		colTable->horizontalHeader()->resizeSection(0, 240);
		colTable->horizontalHeader()->hide();
		colTable->verticalHeader()->hide();
		colTable->setSelectionMode(QAbstractItemView::NoSelection);
		colTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

		//initialize table
		ColVec cols = _p->getCols( INPUT , (*p).first );
		
		for( unsigned int i=0; i<cols.size(); i++ ){
		    int row = colTable->rowCount();
		    colTable->setRowCount(row + 1);
		    
		    QTableWidgetItem *item0 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		    
		    item0->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		   
		    if( _p->isActive() ){
			item0->setBackgroundColor( _p->isErrorCol( p->second.c_str(), cols[i]->getValue().c_str() ) ? 
				QColor(184,16,0,180) : QColor(32,140,64,180) );
		    }

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
	     
		QPushButton *addColButton = new QPushButton(tr("Add"));
		QPushButton *remColButton = new QPushButton(tr("Rem"));

		addColButton->setToolTip(tr("Add New Collection"));
		remColButton->setToolTip(tr("Remove Selected Collection"));

		connect(addColButton, SIGNAL(clicked()), delegate, SLOT(addCollection()));
		connect(remColButton, SIGNAL(clicked()), delegate, SLOT(remCollection()));
		addColButton->setAutoDefault( false );
		remColButton->setAutoDefault( false );
		    
		//Buttons Layout
		QVBoxLayout *colButtonsLayout = new QVBoxLayout;
		colButtonsLayout->addWidget(addColButton);
		colButtonsLayout->addWidget(remColButton);
		
		//GroupBox
		QWidget *colButtons = new QWidget;
		colButtons->setFixedWidth(55);
		colButtons->setLayout(colButtonsLayout);
		 
		//Layout
		QGridLayout *colLayout = new QGridLayout;
    		colLayout->addWidget(colTable,0,0);
    		colLayout->addWidget(colButtons,0,1);

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
	    colsGroupBox->setMinimumWidth(1300);
	    //colsGroupBox->setMaximumHeight(400);
	    
	    //add group box to main layout
	    QScrollArea *scroll = new QScrollArea;
	    scroll->setWidget(colsGroupBox);
	    
	    mainLayout->addWidget(scroll, Qt::AlignTop);
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
	
	bool found = false;
	
	for( ssMap::const_iterator p=colHeaders.begin(); p!=colHeaders.end(); p++ ){
	    found=true;

	    //initialize table
	    ColVec cols = _p->getCols( OUTPUT , (*p).first );
	    for( unsigned int i=0; i<cols.size(); i++ ){
		int row = colTable->rowCount();
		colTable->setRowCount(row + 1);
		
		QTableWidgetItem *item0 = new QTableWidgetItem( (*p).first.c_str() );
		QTableWidgetItem *item1 = new QTableWidgetItem( (*p).second.c_str() );
		QTableWidgetItem *item2 = new QTableWidgetItem( cols[i]->getValue().c_str() );
		
		item0->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		item1->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		item2->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), (*p).first ).c_str() ) );
		
		if( _p->isActive() ){
		    item2->setBackgroundColor( _p->isErrorCol( p->second.c_str(), cols[i]->getValue().c_str() ) ? 
				QColor(184,16,0,180) : QColor(32,140,64,180) );
		}
		
		item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);	
		item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);	
	
		colTable->setItem(row, 0, item0);
		colTable->setItem(row, 1, item1);
		colTable->setItem(row, 2, item2);
	    }
	}
	if(found){
	    //delegate
	    QItemDelegate *delegate = new OColDelegate(_p, _msc, colTable);
	    colTable->setItemDelegate(delegate);
	    
	    //Layout
	    QVBoxLayout *colsLayout = new QVBoxLayout;
	    colsLayout->addWidget(colTable);
	    
	    //create group box for all collections
	    QGroupBox *colsGroupBox = new QGroupBox(tr("OUTPUT COLLECTIONS"));
	    colsGroupBox->setLayout(colsLayout);
	    colsGroupBox->setMaximumHeight(240);
	    
	    //add group box to main layout
	    mainLayout->addWidget(colsGroupBox, Qt::AlignTop);
	}
    }
   

    //////////////////////////////
    //PARAMETERS TABLE
    //////////////////////////////
    if(_p->hasParameters()){
	paramTable = new QTableWidget;
																				 
	QStringList labels;
	labels << tr("Parameter Name") << tr("Parameter Value");
	paramTable->setColumnCount(2);
	paramTable->verticalHeader()->hide();
	paramTable->setHorizontalHeaderLabels(labels);
	paramTable->horizontalHeader()->resizeSection(0, 300);
	paramTable->horizontalHeader()->resizeSection(1, 300);
	paramTable->setSelectionMode(QAbstractItemView::SingleSelection);
	paramTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	paramTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

    
	//initialize table
	StringVec paramKeys;
	_p->getParameters()->getStringKeys(paramKeys);
	
	bool found=false;
	
	for( unsigned int i=0; i<paramKeys.size(); i++ ){
	
	    if( !_msc->getMProcs()->isParamOpt( _p->getType(), paramKeys[i] )){
		found = true;
		updateParam();
	    }
	}
	if(found){
	    //Delegate
	    ParamDelegate *pDelegate = new ParamDelegate( _p, paramTable);
	    paramTable->setItemDelegate( pDelegate );

	    QWidget *nparamvecset = new NParamVecSet( _msc, _p, paramTable, this );
	    connect(paramTable, SIGNAL(cellClicked(int,int)), nparamvecset, SLOT(updateTable()));
	    
	    //Layout
	    QGridLayout *paramLayout = new QGridLayout;
	    paramLayout->addWidget(paramTable,0,0);
	    paramLayout->addWidget(nparamvecset,0,1, Qt::AlignTop | Qt::AlignRight);
	    
	    //GroupBox
	    QGroupBox *paramGroupBox = new QGroupBox(tr("Processor Parameters"));
	    paramGroupBox->setLayout(paramLayout);
	    
	    mainLayout->addWidget(paramGroupBox, Qt::AlignTop);
	}
    }

    //////////////////////////////
    //PARAMETERS TABLE (Optional)
    //////////////////////////////
    if(_p->hasParameters()){
	optParamTable = new QTableWidget;

	QStringList labels;
	labels << tr("Parameter Name") << tr("Parameter Value") << tr("Active");
	optParamTable->setColumnCount(3);
	optParamTable->verticalHeader()->hide();
	optParamTable->setHorizontalHeaderLabels(labels);
	optParamTable->horizontalHeader()->resizeSection(0, 300);
	optParamTable->horizontalHeader()->resizeSection(1, 300);
	optParamTable->horizontalHeader()->resizeSection(2, 50);
	optParamTable->setSelectionMode(QAbstractItemView::SingleSelection);
	optParamTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	optParamTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

	
	bool found = false;
	//initialize table
	StringVec paramKeys;
	_p->getParameters()->getStringKeys(paramKeys);
	for( unsigned int i=0; i<paramKeys.size(); i++ ){

	    if( _msc->getMProcs()->isParamOpt( _p->getType(), paramKeys[i] )){
		found = true;
	    
		int row = optParamTable->rowCount();
		optParamTable->setRowCount(row + 1);
																				     
		StringVec paramValues;
		_p->getParameters()->getStringVals(paramKeys[i], paramValues);
		
		QString str;
		
		for( unsigned int j=0; j<paramValues.size(); j++ ){
		    str+=paramValues[j].c_str();
		    str+=" ";
		}
		
		QTableWidgetItem *item0 = new QTableWidgetItem( paramKeys[i].c_str() );
		QTableWidgetItem *item1 = new QTableWidgetItem( str );
		QTableWidgetItem *item2 = new QTableWidgetItem;

		item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
            	item2->setFlags(item0->flags() & ~Qt::ItemIsEditable);

		if( _msc->getMProcs()->getParamSetSize( _p->getType(), paramKeys[i] ) > 1 ){
		    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
		}

		item0->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), paramKeys[i] ).c_str() ));
		item1->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), paramKeys[i] ).c_str() ));
		item2->setToolTip( QString(
			    tr( "Activate this checkbox to write the parameter as a normal parameter in the xml file.\n" 
				"Please note that if you don't want to use this parameter and leave this option unchecked\n"
				"the value of the parameter will still be written as a comment in the xml file but\n"
				"the next time you open the xml file in the GUI the value will get lost."
			    )));
		optParamTable->setItem(row, 0, item0);
		optParamTable->setItem(row, 1, item1);
		optParamTable->setItem(row, 2, item2);

		item2->setCheckState( _p->isParamOptional( paramKeys[i] ) ? Qt::Unchecked : Qt::Checked );
	    }
	}
	if(found){
	    
	    //Delegate
	    ParamDelegate *pDelegate = new ParamDelegate(_p, optParamTable);
	    optParamTable->setItemDelegate( pDelegate );

	    QWidget *nparamvecset = new NParamVecSet( _msc, _p, optParamTable, this );
	    connect(optParamTable, SIGNAL(cellClicked(int,int)), nparamvecset, SLOT(updateTable()));
	    
	    //Layout
	    QGridLayout *paramLayout = new QGridLayout;
	    paramLayout->addWidget(optParamTable,0,0);
	    paramLayout->addWidget(nparamvecset,0,1, Qt::AlignTop | Qt::AlignRight);
	
	    //GroupBox
	    QGroupBox *paramGroupBox = new QGroupBox(tr("Optional Processor Parameters"));
	    paramGroupBox->setLayout(paramLayout);
	    
	    mainLayout->addWidget(paramGroupBox, Qt::AlignTop);

	    connect(optParamTable, SIGNAL(cellClicked(int, int)), this, SLOT(optParamChanged()));
	}
    }


    //////////////////////////////
    // APPLY, CANCEL BUTTONS
    //////////////////////////////

    QPushButton *applyButton = new QPushButton(tr("&Apply Changes"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    connect(applyButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    
    applyButton->setAutoDefault( false );
    cancelButton->setAutoDefault( false );
    applyButton->setFixedHeight( 40 );
    cancelButton->setFixedHeight( 40 );
    applyButton->setFixedWidth( 150 );
    cancelButton->setFixedWidth( 150 );
	
    //Buttons Layout
    QGridLayout *mainButtonsLayout = new QGridLayout;
    mainButtonsLayout->addWidget(cancelButton, 0, 1, 0, 1, Qt::AlignLeft | Qt::AlignTop);
    mainButtonsLayout->addWidget(applyButton, 0, 2, 0, 2, Qt::AlignRight | Qt::AlignTop);
    
    //GroupBox
    QWidget *mainButtons = new QWidget;
    mainButtons->setFixedHeight( 50 );
    mainButtons->setLayout(mainButtonsLayout);
    
    mainLayout->addWidget(mainButtons, Qt::AlignTop);
}

void Dialog::updateParam(){
    //initialize table
    StringVec paramKeys;
    _p->getParameters()->getStringKeys(paramKeys);

    paramTable->setRowCount(0);    
    for( unsigned int i=0; i<paramKeys.size(); i++ ){
    
	if( !_msc->getMProcs()->isParamOpt( _p->getType(), paramKeys[i] )){
	    
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

	    if( _msc->getMProcs()->getParamSetSize( _p->getType(), paramKeys[i] ) > 1 ){
		item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
	    }

	    item0->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), paramKeys[i] ).c_str() ) );
	    item1->setToolTip( QString( _msc->getMProcs()->getParamD( _p->getType(), paramKeys[i] ).c_str() ) );
		
	    paramTable->setItem(row, 0, item0);
	    paramTable->setItem(row, 1, item1);
	}
    }
}

void Dialog::optParamChanged(){
    for( int row = 0; row < optParamTable->rowCount(); row++ ){
        QTableWidgetItem *item0 = optParamTable->item(row, 0);
        QTableWidgetItem *item2 = optParamTable->item(row, 2);
                                                                                                                                                             
        if(item2->checkState() == Qt::Checked){
	    _p->setOptionalParam( item0->text().toStdString(), false );
        }
	else{
	    _p->setOptionalParam( item0->text().toStdString() );
	}
    }
}


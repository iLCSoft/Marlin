#include <QtGui>

#include "mainwindow.h"
#include "aprocdelegate.h"
#include "iprocdelegate.h"
#include "gparamdelegate.h"
#include "dialog.h"
#include "addprocdialog.h"
#include "addcondition.h"
#include "editcondition.h"
#include "guihelp.h"

#include "marlin/CMProcessor.h"

#include <string>
#include <sstream>
#include <stdlib.h>

MainWindow::MainWindow() : _modified(false), _saved(false), _file(""), msc(NULL)
{
    //key shortcut for help
    QShortcut *helpF1 = new QShortcut(Qt::Key_F1, this);
    connect(helpF1, SIGNAL(activated()), this, SLOT(help())); 
    
    //signal if modifications were made
    connect(this, SIGNAL(modifiedContent()), this, SLOT(madeChanges())); 

    //init views
    setupViews();
    
    //Left
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(lcioColsGBox);
    leftLayout->addWidget(globalSectionGBox);
    leftLayout->addWidget(viewButtonsGBox);
    
    QWidget *left = new QWidget;
    left->setLayout(leftLayout);

    //Vertical Splitter
    vSplitter = new QSplitter;
    vSplitter->setOrientation( Qt::Vertical );
    vSplitter->addWidget(aProcGBox);
    vSplitter->addWidget(condGBox);
    vSplitter->addWidget(aProcErrorsGBox);
    vSplitter->addWidget(iProcGBox);

    QList<int> vSizes;
    vSizes << 650 << 500 << 450 << 400;
    vSplitter->setSizes( vSizes );
   
    //Horizontal Splitter
    hSplitter = new QSplitter; 
    hSplitter->addWidget(left);
    hSplitter->addWidget(vSplitter);
    
    hSizes << 600 << 800;
    hSplitter->setSizes( hSizes );
 
    //about Marlin GUI message
    aboutGUIMsg = QString(tr(
	"<font size=5><b>Marlin - Graphical User Interface</b></font><br><br>"
	"<font size=4>This application lets you \"repair\" or create "
	"new Steering Files for Marlin.</font><br><br><br>"
	"<b>WARNING:</b><br>"
	"<font color=#FB0000>Please be aware that comments made in the original steering files will get lost in the saving process.<br>"
    	"Processors that are not installed in your Marlin binary are not editable in this application and loose "
	"their parameter's descriptions and types.<br>"
    	"Extra parameters that aren't categorized as default in a processor also loose their description and type.</font><br><br><br>"
    	
	"In order for this application to work correctly you should first check if all processors found in the "
	"steering file are installed in your Marlin binary.<br>This can easily be check by running \"Marlin -c SteeringFile.xml\"<br><br><br>"
	"If you have comments or suggestions please take a look at the ILC Forum<br>under the Marlin section at "
	"<a href=\"http://forum.linearcollider.org\">http://forum.linearcollider.org</a>. Thanks <br><br>"
	"Author: Jan Engels @ DESY -IT-"
    ));
    
    //save changes message
    saveChangesMsg = QString(tr( 
	"You made changes that will get lost.\nDo you want to save your changes?\n\n"
	"WARNING:\n"
	"Please be aware that comments made in the original steering files will get lost in the saving process.\n"
	"Processors that are not installed in your Marlin binary will loose their parameter's descriptions and types as well.\n"
	"Extra parameters that aren't categorized as default in a processor also loose their description and type.\n\n"
    ));
   
    //set central widget
    QLabel *defLab = new QLabel(aboutGUIMsg);

    defLab->setAlignment(Qt::AlignCenter);
    setCentralWidget(defLab);
    
    //Menu & Status Bars
    createMenus();
    statusBar();
    
    //Window Title
    setWindowTitle(tr("Marlin GUI"));
    
    checkCurrentStyle();

    //Window Size
    showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    // bug fix: when editing global parameters the values only get saved
    // after the edit box has lost focus
    this->setFocus();

    if( !_modified ){
	e->accept();
	return;
    }

    int ret = QMessageBox::warning(this, tr("Exit Marlin GUI"), saveChangesMsg,
	    QMessageBox::Yes,
	    QMessageBox::No | QMessageBox::Default,
	    QMessageBox::Cancel | QMessageBox::Escape);

    if( ret == QMessageBox::Yes ){
	saveXMLFile();
	e->accept();
    }
    else if( ret == QMessageBox::No ){
	e->accept();
    }
    else{
        e->ignore();
    }
}

////////////////////////////////////////////////////////////////////////////////
//INIT METHODS
////////////////////////////////////////////////////////////////////////////////

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAction = fileMenu->addAction(tr("&New..."));
    newAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
    QAction *saveAction = fileMenu->addAction(tr("&Save..."));
    saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    QAction *saveasAction = fileMenu->addAction(tr("Save &As..."));
    saveasAction->setShortcut(QKeySequence(tr("Ctrl+A")));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(QKeySequence(tr("Ctrl+X")));
    
    connect(newAction, SIGNAL(triggered()), this, SLOT(newXMLFile()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openXMLFile()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveXMLFile()));
    connect(saveasAction, SIGNAL(triggered()), this, SLOT(saveAsXMLFile()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    styleActionGroup = new QActionGroup(this);
    foreach (QString styleName, QStyleFactory::keys()) {
        QAction *action = new QAction(styleActionGroup);
        action->setText(tr("%1 Style").arg(styleName));
        action->setData(styleName);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle(bool)));
    }
    
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    foreach( QAction *action, styleActionGroup->actions() )
    viewMenu->addAction(action);
    
    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutGUIAction = aboutMenu->addAction(tr("About &Marlin GUI..."));
    QAction *aboutQTAction = aboutMenu->addAction(tr("About &QT..."));
    QAction *helpAction = aboutMenu->addAction(tr("&Help..."));
    
    connect(aboutGUIAction, SIGNAL(triggered()), this, SLOT(aboutGUI()));
    connect(aboutQTAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
}

void MainWindow::aboutGUI(){
    QMessageBox::about(this, tr("About Marlin GUI"), aboutGUIMsg);
}

void MainWindow::help(){
    QWidget *help = NULL;
    if( !showCond->isChecked() ){
	help= new GUIHelp;
    }
    else{
	help= new GUIHelp("/gui/help/html/conditions.html",this, Qt::Dialog );
    }
    help->show();
}

void MainWindow::setupViews()
{

    //////////////////////////////
    //CONDITIONS TABLE
    //////////////////////////////

    condTable = new QTableWidget;
                                                                                                                                                             
    QStringList labels;
    labels << tr("Condition");
    condTable->setColumnCount(1);
    condTable->setHorizontalHeaderLabels(labels);
    condTable->horizontalHeader()->resizeSection(0, 500);
    condTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    condTable->setSelectionMode(QAbstractItemView::SingleSelection);
    condTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
                                                                                                                                                             
    connect(condTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(editCondition()));
    
    //Buttons
    QPushButton *addCond = new QPushButton(tr("&Insert"));
    QPushButton *editCond = new QPushButton(tr("Edit"));
    QPushButton *remCond = new QPushButton(tr("Remove"));
                                                                                                                                                             
    addCond->setToolTip(tr("Insert New Condition"));
    editCond->setToolTip(tr("Edit selected Condition"));
    remCond->setToolTip(tr("Remove Selected Condition"));
                                                                                                                                                             
    connect(addCond, SIGNAL(clicked()), this, SLOT(addCondition()));
    connect(editCond, SIGNAL(clicked()), this, SLOT(editCondition()));
    connect(remCond, SIGNAL(clicked()), this, SLOT(remCondition()));
                                                                                                                                                             
    //Buttons Layout
    QVBoxLayout *condButtonsLayout = new QVBoxLayout;
    condButtonsLayout->addWidget(addCond);
    condButtonsLayout->addWidget(editCond);
    condButtonsLayout->addWidget(remCond);
                                                                                                                                                             
    QGroupBox *condButtons = new QGroupBox(tr("Operations"));
    condButtons->setLayout( condButtonsLayout );
    condButtons->setFixedWidth( 120 );
                                                                                                                                                             
    //Layout
    QGridLayout *condLayout = new QGridLayout;
    condLayout->addWidget(condTable,0,0);
    condLayout->addWidget(condButtons,0,1,Qt::AlignTop);
                                                                                                                                                             
    //GroupBox
    condGBox = new QGroupBox(tr("Edit Conditions List"));
    condGBox->setLayout(condLayout);

    //hide conditions by default when starting the GUI
    condGBox->hide();
                                                                                                                                                             
    //////////////////////////////
    //ACTIVE PROCESSORS
    //////////////////////////////
    aProcTable = new QTableWidget;
    
    labels.clear();
    labels << tr("Name") << tr("Type");
    aProcTable->setColumnCount(2);
    aProcTable->setHorizontalHeaderLabels(labels);
    aProcTable->horizontalHeader()->setClickable(true);
    aProcTable->horizontalHeader()->resizeSection(0, 220);
    aProcTable->horizontalHeader()->resizeSection(1, 220);
    aProcTable->verticalHeader()->hide();
    aProcTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    aProcTable->setSelectionMode(QAbstractItemView::SingleSelection);
    //aProcTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
    aProcTable->setItemDelegate(new AProcDelegate(this));

    connect(aProcTable->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(selectColumn(int)));
    connect(aProcTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(procTypeDC(int, int)));
    connect(aProcTable, SIGNAL(cellPressed(int, int)), this, SLOT(selectionChanged(int)));
    connect(aProcTable, SIGNAL(cellClicked(int, int)), this, SLOT(conditionChanged(int, int)));
    
    //Buttons
    QPushButton *addAProc = new QPushButton(tr("&Add"));
    QPushButton *editAProc = new QPushButton(tr("&Edit"));
    QPushButton *remAProc = new QPushButton(tr("Delete"));
    QPushButton *deactProc = new QPushButton(tr("Dea&ctivate"));
    QPushButton *mvAProcUp = new QPushButton(tr("Move &Up"));
    QPushButton *mvAProcDn = new QPushButton(tr("Move &Down"));
    showCond = new QPushButton(tr("&Show Cond."));

    showCond->setCheckable( true );
    
    addAProc->setToolTip(tr("Add New Processor"));
    editAProc->setToolTip(tr("Edit Selected Processor"));
    remAProc->setToolTip(tr("Delete Selected Processor"));
    deactProc->setToolTip(tr("Deactivate Selected Processor"));
    mvAProcUp->setToolTip(tr("Move Selected Processor Up"));
    mvAProcDn->setToolTip(tr("Move Selected Processor Down"));
    showCond->setToolTip(tr("Show/Hide Conditions"));
    
    connect(addAProc, SIGNAL(clicked()), this, SLOT(addAProcessor()));
    connect(editAProc, SIGNAL(clicked()), this, SLOT(editAProcessor()));
    connect(remAProc, SIGNAL(clicked()), this, SLOT(remAProcessor()));
    connect(deactProc, SIGNAL(clicked()), this, SLOT(deactivateProcessor()));
    connect(mvAProcUp, SIGNAL(clicked()), this, SLOT(moveProcessorUp()));
    connect(mvAProcDn, SIGNAL(clicked()), this, SLOT(moveProcessorDown()));
    connect(showCond, SIGNAL(toggled(bool)), this, SLOT(showConditions(bool)));
    
    //Buttons Layout
    QVBoxLayout *aProcButtonsLayout = new QVBoxLayout;
    aProcButtonsLayout->addWidget(addAProc);
    aProcButtonsLayout->addWidget(editAProc);
    aProcButtonsLayout->addWidget(remAProc);
    aProcButtonsLayout->addWidget(deactProc);
    aProcButtonsLayout->addWidget(mvAProcUp);
    aProcButtonsLayout->addWidget(mvAProcDn);
    aProcButtonsLayout->addWidget(showCond);
    
    //Buttons GroupBox
    aProcButtonsGBox = new QGroupBox(tr("Operations"));
    aProcButtonsGBox->setLayout(aProcButtonsLayout);
    aProcButtonsGBox->setFixedWidth(120);

   
    //Layout
    QGridLayout *aProcLayout = new QGridLayout;
    aProcLayout->addWidget(aProcTable,0,0);
    aProcLayout->addWidget(aProcButtonsGBox,0,1,Qt::AlignTop);
     
    //GroupBox
    aProcGBox = new QGroupBox(tr("Active Processors"));
    aProcGBox->setLayout(aProcLayout);
   
    //////////////////////////////
    //INACTIVE PROCESSORS
    //////////////////////////////
    iProcTable = new QTableWidget;
    
    labels.clear();
    labels << tr("Name") << tr("Type");
    iProcTable->setColumnCount(2);
    iProcTable->setHorizontalHeaderLabels(labels);
    iProcTable->horizontalHeader()->resizeSection(0, 220);
    iProcTable->horizontalHeader()->resizeSection(1, 220);
    iProcTable->verticalHeader()->hide();
    iProcTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    iProcTable->setSelectionMode(QAbstractItemView::SingleSelection);
    //iProcTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
    iProcTable->setItemDelegate(new IProcDelegate(this));
    
    //Buttons
    QPushButton *editIProc = new QPushButton(tr("Edit"));
    QPushButton *remIProc = new QPushButton(tr("Delete"));
    QPushButton *actProc = new QPushButton(tr("Activate"));
    
    editIProc->setToolTip(tr("Edit Selected Processor"));
    remIProc->setToolTip(tr("Delete Selected Processor"));
    actProc->setToolTip(tr("Activate Selected Processor"));
    
    connect(editIProc, SIGNAL(clicked()), this, SLOT(editIProcessor()));
    connect(remIProc, SIGNAL(clicked()), this, SLOT(remIProcessor()));
    connect(actProc, SIGNAL(clicked()), this, SLOT(activateProcessor()));
    
    //Buttons Layout
    QVBoxLayout *iProcButtonsLayout = new QVBoxLayout;
    iProcButtonsLayout->addWidget(actProc);
    iProcButtonsLayout->addWidget(editIProc);
    iProcButtonsLayout->addWidget(remIProc);
    
    //Buttons GroupBox
    iProcButtonsGBox = new QGroupBox(tr("Operations"));
    iProcButtonsGBox->setLayout(iProcButtonsLayout);
    iProcButtonsGBox->setFixedWidth(120);

    //Layout
    QGridLayout *iProcLayout = new QGridLayout;
    iProcLayout->addWidget(iProcTable,0,0);
    iProcLayout->addWidget(iProcButtonsGBox,0,1,Qt::AlignTop);
    
    //GroupBox
    iProcGBox = new QGroupBox(tr("Inactive Processors"));
    iProcGBox->setLayout(iProcLayout);

    //////////////////////////////
    //LCIO COLLECTIONS LIST
    //////////////////////////////
    lcioColsTable = new QTableWidget;
    
    labels.clear();
    labels << tr("Name") << tr("Type") << tr("LCIO File");
    lcioColsTable->setColumnCount(3);
    lcioColsTable->setHorizontalHeaderLabels(labels);
    lcioColsTable->setSelectionMode(QAbstractItemView::NoSelection);
    lcioColsTable->horizontalHeader()->resizeSection(0, 160);
    lcioColsTable->horizontalHeader()->resizeSection(1, 160);
    lcioColsTable->horizontalHeader()->resizeSection(2, 160);
    lcioColsTable->verticalHeader()->hide();
    //lcioColsTable->setSortingEnabled( true );
    //lcioColsTable->sortItems( 1 );
    lcioColsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
    //Layout
    QVBoxLayout *lcioColsLayout = new QVBoxLayout;
    lcioColsLayout->addWidget(lcioColsTable);
     
    //GroupBox
    lcioColsGBox = new QGroupBox(tr("List of all Collections Found in LCIO Files"));
    lcioColsGBox->setLayout(lcioColsLayout);

    //////////////////////////////
    //LCIO FILES LIST
    //////////////////////////////

    lcioFilesList = new QListWidget;
    
    //Buttons
    QPushButton *mvFileUp = new QPushButton(tr("Move Up"));
    QPushButton *mvFileDn = new QPushButton(tr("Move Down"));
    QPushButton *addFile = new QPushButton(tr("Add"));
    QPushButton *remFile = new QPushButton(tr("Remove"));
    
    addFile->setToolTip(tr("Add New LCIO File"));
    remFile->setToolTip(tr("Remove Selected LCIO File"));
    mvFileUp->setToolTip(tr("Move Selected File Up"));
    mvFileDn->setToolTip(tr("Move Selected File Down"));
    
    connect(addFile, SIGNAL(clicked()), this, SLOT(addLCIOFile()));
    connect(remFile, SIGNAL(clicked()), this, SLOT(remLCIOFile()));
    connect(mvFileUp, SIGNAL(clicked()), this, SLOT(moveLCIOFileUp()));
    connect(mvFileDn, SIGNAL(clicked()), this, SLOT(moveLCIOFileDown()));
    
    //Layout
    QVBoxLayout *lcioButtonsLayout = new QVBoxLayout;
    lcioButtonsLayout->addWidget(addFile);
    lcioButtonsLayout->addWidget(remFile);
    lcioButtonsLayout->addWidget(mvFileUp);
    lcioButtonsLayout->addWidget(mvFileDn);
    
    QWidget *lcioButtons = new QWidget;
    lcioButtons->setLayout( lcioButtonsLayout );
    
    //Label
	QLabel *cwdl = new QLabel( "CWD: " );
    QLineEdit *cwdle = new QLineEdit(QDir::currentPath());
	cwdle->setReadOnly( true );
	QWidget *cwdWdg = new QWidget;
	
	QHBoxLayout *cwdLayout = new QHBoxLayout;
	cwdLayout->addWidget(cwdl);
	cwdLayout->addWidget(cwdle);

	cwdWdg->setLayout( cwdLayout );

    cwdWdg->setToolTip(tr("Current Working Directory: LCIO Files in the same directory branch of this path are added with relative paths"));
    
    //Layout
    QGridLayout *lcioFilesLayout = new QGridLayout;
    lcioFilesLayout->addWidget(cwdWdg,0,0);
    lcioFilesLayout->addWidget(lcioFilesList,1,0);
    lcioFilesLayout->addWidget(lcioButtons,1,1,Qt::AlignTop);
 
    //GroupBox
    lcioFilesGBox = new QGroupBox(tr("Global Section LCIO Files"));
    lcioFilesGBox->setLayout(lcioFilesLayout);
    lcioFilesGBox->setMaximumHeight(180);

    //////////////////////////////
    //GLOBAL SECTION TABLE
    //////////////////////////////

    globalSectionTable = new QTableWidget;
    
    labels.clear();
    labels << tr("Parameter Name") << tr("Parameter Value");
    globalSectionTable->setColumnCount(2);
    globalSectionTable->setHorizontalHeaderLabels(labels);
    globalSectionTable->horizontalHeader()->resizeSection(0, 240);
    globalSectionTable->horizontalHeader()->resizeSection(1, 240);
    globalSectionTable->verticalHeader()->hide();
    globalSectionTable->setSelectionMode(QAbstractItemView::NoSelection);
    globalSectionTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

    //GEAR Button
    QPushButton *gearButton = new QPushButton(tr("Browse for GEAR File"));
    connect(gearButton, SIGNAL(clicked()), this, SLOT(changeGearFile()));
    
    //Layout
    QVBoxLayout *globalParametersLayout = new QVBoxLayout;
    globalParametersLayout->addWidget(globalSectionTable);
    globalParametersLayout->addWidget(gearButton);
     
    //GroupBox
    QGroupBox *globalParametersGBox = new QGroupBox(tr("Global Section Parameters"));
    globalParametersGBox->setLayout(globalParametersLayout);
    
    //Layout
    QVBoxLayout *globalSectionLayout = new QVBoxLayout;
    globalSectionLayout->addWidget(lcioFilesGBox);
    globalSectionLayout->addWidget(globalParametersGBox);
     
    //GroupBox
    globalSectionGBox = new QGroupBox(tr("Global Section"));
    globalSectionGBox->setLayout(globalSectionLayout);


    //////////////////////////////
    //VIEW OPTION BUTTONS
    //////////////////////////////
    hideProcs = new QPushButton(tr("Hide Inactive Processors"));
    hideErrors = new QPushButton(tr("Hide Active Processor Errors"));
    hideProcs->setCheckable(true);
    hideErrors->setCheckable(true);
    
    hideProcs->setToolTip(tr("Hide/Show Inactive Processors"));
    hideErrors->setToolTip(tr("Hide/Show Active Processor Errors"));
    
    connect(hideProcs, SIGNAL(toggled(bool)), this, SLOT(hideIProcessors(bool)));
    connect(hideErrors, SIGNAL(toggled(bool)), this, SLOT(hideAProcErrors(bool)));
    
    //Layout
    QHBoxLayout *viewButtonsLayout = new QHBoxLayout;
    viewButtonsLayout->addWidget(hideProcs);
    viewButtonsLayout->addWidget(hideErrors);
    
    //GroupBox
    viewButtonsGBox = new QGroupBox(tr("View Options"));
    viewButtonsGBox->setLayout(viewButtonsLayout);
    
    //////////////////////////////
    //ERRORS
    //////////////////////////////
    aProcErrors = new QTextEdit;
    aProcErrors->setReadOnly(true);
    
    //Layout
    QVBoxLayout *aProcErrorsLayout = new QVBoxLayout;
    aProcErrorsLayout->addWidget(aProcErrors);
    
    //GroupBox
    aProcErrorsGBox = new QGroupBox(tr("Error Description for selected Active Processor"));
    aProcErrorsGBox->setLayout(aProcErrorsLayout);

}

void MainWindow::setMarlinSteerCheck( const char* filename )
{

    if( msc != NULL ){
	delete msc;
    }

    msc = new MarlinSteerCheck(filename);

    if( msc->getErrors().size()!=0 ){
	QString errors;
	for( sSet::const_iterator p=msc->getErrors().begin(); p!=msc->getErrors().end(); p++){
	    errors+=(*p).c_str();
	    errors+="\n";
	}
	QMessageBox::warning(this,
	    tr("Errors Occured loading Steering File"), 
	    errors
	);
	if( msc->getErrors().find("XML File parsing error") != msc->getErrors().end() ){
	    return;
	}
    }

    std::string filen(filename);

    //if it is an old steering file append .xml at the end
    if( filen.find(".xml",0) == std::string::npos ){
	filen+=".xml";
    }
 
    _file=filen;
    _modified=false;
    _saved=false;
    std::cout << "Marlin steering file [" << filen << "] loaded successfully into the GUI\n";
    //set the window title
    QString title= "Marlin GUI - ";
    QFileInfo xmlFile(filen.c_str());
    title+=xmlFile.absoluteFilePath();
    setWindowTitle(title);

    setCentralWidget(hSplitter);
    
    //create backup file
    std::string cmd= "ls ";
    cmd+=filename;
    cmd+="~";
    cmd+= " >/dev/null 2>/dev/null";
    //if backup file doesn't exist create backup
    if( system( cmd.c_str() ) ){
	cmd= "cp -f ";
	cmd+=filename;
	cmd+=" ";
	cmd+=filename;
	cmd+="~";
	if( system( cmd.c_str() ) ){
	    std::cerr << "Marlin GUI::setMarlinSteerCheck: Error creating backup file!!\n";
	}
    }

    updateGlobalSection();
    updateFiles();
    updateAProcessors();
    updateIProcessors();
    updateConds();

    //Delegate
    GParamDelegate *gpDelegate = new GParamDelegate( msc->getGlobalParameters(), this, globalSectionTable );
    globalSectionTable->setItemDelegate( gpDelegate );
}


////////////////////////////////////////////////////////////////////////////////
//UPDATE VIEWS METHODS
////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateGlobalSection()
{
    //initialize global parameters 
    StringVec paramKeys;
    msc->getGlobalParameters()->getStringKeys(paramKeys);

    globalSectionTable->setRowCount(0);
    for( unsigned int i=0; i<paramKeys.size(); i++ ){
                                                                                                                                                             
        int row = globalSectionTable->rowCount();
        globalSectionTable->setRowCount(row + 1);
                                                                                                                                                             
        StringVec paramValues;
        msc->getGlobalParameters()->getStringVals(paramKeys[i], paramValues);

        QString str;
                                                                                                                                                             
        for( unsigned int j=0; j<paramValues.size(); j++ ){
            str+=paramValues[j].c_str();
            str+=" ";
        }
                                                                                                                                                             
        QTableWidgetItem *item0 = new QTableWidgetItem( paramKeys[i].c_str() );
        QTableWidgetItem *item1 = new QTableWidgetItem( str );

	if(paramKeys[i]=="GearXMLFile"){
	    QFileInfo gearInfo(paramValues[0].c_str());
	    item1->setBackgroundColor( ( !gearInfo.exists() ? QColor(184,16,0,180) : QColor(32,140,64,180)) );
	    item1->setFlags(item0->flags() & ~Qt::ItemIsEditable);
	}
                                                                                                                                                             
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
                                                                                                                                                             
        globalSectionTable->setItem(row, 0, item0);
        globalSectionTable->setItem(row, 1, item1);
                                                                                                                                                             
        //globalSectionTable->openPersistentEditor(item1);
    }
}

void MainWindow::updateConds( int pos ){
    condTable->setRowCount(0);
    for( sSet::const_iterator p=msc->getPConditions().begin(); p!=msc->getPConditions().end(); p++ ){
        int row = condTable->rowCount();
        condTable->setRowCount(row + 1);
                                                                                                                                                             
        QTableWidgetItem *item = new QTableWidgetItem((*p).c_str());
        condTable->setItem(row, 0, item);
    }
    if( pos == -1 ){
        selectRow(condTable, condTable->rowCount());
    }
    else{
        selectRow(condTable, pos);
    }
}

void MainWindow::updateFiles(int pos)
{
    //initialize LCIO collections table
    lcioColsTable->setRowCount(0);
    for( unsigned int i=0; i<msc->getLCIOCols().size(); i++ ){
	
	int row = lcioColsTable->rowCount();
        lcioColsTable->setRowCount(row + 1);
	
	QTableWidgetItem *item0 = new QTableWidgetItem(msc->getLCIOCols()[i]->getValue().c_str());
	QTableWidgetItem *item1 = new QTableWidgetItem(msc->getLCIOCols()[i]->getType().c_str());
	QTableWidgetItem *item2 = new QTableWidgetItem(msc->getLCIOCols()[i]->getName().c_str());
	
	lcioColsTable->setItem(row, 0, item0);
	lcioColsTable->setItem(row, 1, item1);
	lcioColsTable->setItem(row, 2, item2);
    }

    //initialize LCIO files list
    lcioFilesList->clear();
    for( unsigned int i=0; i<msc->getLCIOFiles().size(); i++ ){
	new QListWidgetItem(msc->getLCIOFiles()[i].c_str(), lcioFilesList);
    }

    if( pos == -1 ){
	selectLCIORow( lcioFilesList, lcioFilesList->count() );
    }
    else{
	selectLCIORow( lcioFilesList, pos );
    }
}

void MainWindow::selectLCIORow(QListWidget* t, int row ){
    if( row>=0 && row<=t->count() ){
	if( row==t->count() ){ row--; }
 	t->setCurrentRow( row );
        t->setItemSelected( t->item(row), true );
    }
}

void MainWindow::updateAProcessors( int pos )
{
    //clear errors
    aProcErrors->clear();
    
    //initialize active processors table
    int numCond = msc->getPConditions().size();
    aProcTable->setColumnCount( 0 );
    aProcTable->setColumnCount( numCond+2 );
    
    QStringList labels;
    for( int i=0; i<numCond; i++ ){
	QString num;
	
	if( (i+1) >= 10){
	    num=((i+1)/10)+48;
	    num+=((i+1)%10)+48;
	}
	else{
	    num=(i+1)+48;
	}
	//append condition to the header
	sSet::const_iterator p=msc->getPConditions().begin();
	advance(p,i);
	num+="    ";
	num+=(*p).c_str();

	labels << num;
	aProcTable->horizontalHeader()->resizeSection(i, 25);
	aProcTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	aProcTable->setColumnHidden(i, !showCond->isChecked() );
    }

    labels << tr("Name") << tr("Type");
    aProcTable->setHorizontalHeaderLabels(labels);
    aProcTable->horizontalHeader()->resizeSection( numCond, 220);
    aProcTable->horizontalHeader()->resizeSection( numCond+1, 220);
	
    aProcTable->setRowCount(0);
    for( unsigned int i=0; i<msc->getAProcs().size(); i++ ){
	int row = aProcTable->rowCount();
        aProcTable->setRowCount(row + 1);
	
	QTableWidgetItem *item0 = new QTableWidgetItem(msc->getAProcs()[i]->getName().c_str());
	QTableWidgetItem *item1 = new QTableWidgetItem(msc->getAProcs()[i]->getType().c_str());
	
	item0->setToolTip( QString(msc->getAProcs()[i]->getDescription().c_str() ));
	item1->setToolTip( QString(msc->getAProcs()[i]->getDescription().c_str() ));

	item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
	
	aProcTable->setItem(row, numCond, item0);
	aProcTable->setItem(row, numCond+1, item1);
	
	item0->setBackgroundColor( (msc->getAProcs()[i]->hasErrors() ? QColor(184,16,0,180) : QColor(32,140,64,180)) );
	item1->setBackgroundColor( (msc->getAProcs()[i]->hasErrors() ? QColor(184,16,0,180) : QColor(32,140,64,180)) );
	
	for( int j=0; j<numCond; j++ ){
	    QTableWidgetItem *item = new QTableWidgetItem;
	    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	    
	    item->setCheckState( msc->getAProcs()[i]->hasCondition( msc->getCondition(j) ) ? Qt::Checked : Qt::Unchecked );
	    item->setToolTip( msc->getCondition(j).c_str() );
	    aProcTable->setItem(row, j, item);
	}
    }
    if( pos == -1 ){
	selectRow(aProcTable, aProcTable->rowCount(), true);
    }
    else{
	selectRow(aProcTable, pos, true);
    }
}

void MainWindow::updateIProcessors( int pos )
{
    //initialize inactive processors table
    iProcTable->setRowCount(0);
    for( unsigned int i=0; i<msc->getIProcs().size(); i++ ){
	int row = iProcTable->rowCount();
        iProcTable->setRowCount(row + 1);
	
	QTableWidgetItem *item0 = new QTableWidgetItem(msc->getIProcs()[i]->getName().c_str());
	QTableWidgetItem *item1 = new QTableWidgetItem(msc->getIProcs()[i]->getType().c_str());
	
	item0->setToolTip( QString(msc->getIProcs()[i]->getDescription().c_str() ));
	item1->setToolTip( QString(msc->getIProcs()[i]->getDescription().c_str() ));
	
	item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
	
	iProcTable->setItem(row, 0, item0);
	iProcTable->setItem(row, 1, item1);
	
	item0->setBackgroundColor( (msc->getIProcs()[i]->isInstalled() ? QColor(255,255,255) : QColor(128,128,128)) );
	item1->setBackgroundColor( (msc->getIProcs()[i]->isInstalled() ? QColor(255,255,255) : QColor(128,128,128)) );
	item0->setTextColor( (msc->getIProcs()[i]->isInstalled() ? QColor(0,0,0) : QColor(200,200,200)) );
	item1->setTextColor( (msc->getIProcs()[i]->isInstalled() ? QColor(0,0,0) : QColor(200,200,200)) );
    }
    if( pos == -1 ){
	selectRow(iProcTable, iProcTable->rowCount());
    }
    else{
	selectRow(iProcTable, pos);
    }
}

void MainWindow::conditionChanged( int row, int col ){
    if( col < aProcTable->columnCount()-2){
	QTableWidgetItem *item = aProcTable->item(row, col);
	if(item->checkState() == Qt::Checked){
	    msc->getAProcs()[row]->getConditions().insert( msc->getCondition( col ));
	}
	else{
	    msc->getAProcs()[row]->getConditions().erase( msc->getCondition( col ));
	}
	emit modifiedContent();
    }
}

void MainWindow::selectColumn( int col ){
    if( col < aProcTable->columnCount()-2 && msc->getAProcs().size() > 0 ){
	//selection based on the select state from the first item in the table
	bool select=msc->getAProcs()[0]->hasCondition( msc->getCondition( col ));
	
	for( unsigned int i=0; i<msc->getAProcs().size(); i++ ){
	    if( !select ){
		msc->getAProcs()[i]->getConditions().insert( msc->getCondition( col ));
	    }
	    else{
		msc->getAProcs()[i]->getConditions().erase( msc->getCondition( col ));
	    }
	}
	updateAProcessors( aProcTable->currentRow() );
	emit modifiedContent();
    }
}


void MainWindow::selectRow(QTableWidget* t, int row, bool colors ){
    if( row>=0 && row<=t->rowCount() ){
	if( row==t->rowCount() ){ row--; }
 	t->setCurrentCell( row, 0 );
        t->setItemSelected( t->currentItem(), true );
	if( colors ){ emit selectionChanged( row ); }
    }
}

void MainWindow::procTypeDC( int row, int col ){
    if( row>=0 && col == aProcTable->columnCount() - 1 ){
	editAProcessor();
    } 
}

////////////////////////////////////////////////////////////////////////////////
//PUBLIC SLOTS
////////////////////////////////////////////////////////////////////////////////

void MainWindow::aProcNameChanged(){
    //update the name of the processor in the vector
    msc->getAProcs()[aProcTable->currentRow()]->setName(aProcTable->currentItem()->text().toStdString());
    emit modifiedContent();
}

void MainWindow::iProcNameChanged(){
    //update the name of the processor in the vector
    msc->getIProcs()[iProcTable->currentRow()]->setName(iProcTable->currentItem()->text().toStdString());
    emit modifiedContent();
}


////////////////////////////////////////////////////////////////////////////////
//PRIVATE SLOTS
////////////////////////////////////////////////////////////////////////////////

void MainWindow::showConditions(bool checked)
{
    int numCond = msc->getPConditions().size();
    
    //show conditions
    if(checked){
	for( int i=0; i<numCond; i++ ){
	    aProcTable->setColumnHidden(i, false);
	}

	showCond->setText(tr("&Hide Cond."));

	hSizes = hSplitter->sizes();
	hSplitterSize = hSizes[0];
	hSizes[0] = 0;
	
	vSplitter->widget(1)->show();
	vSplitter->widget(2)->hide();
	vSplitter->widget(3)->hide();
    }
    //hide conditions
    else{
	for( int i=0; i<numCond; i++ ){
	    aProcTable->setColumnHidden(i, true);
	}
	
	showCond->setText(tr("&Show Cond."));
	
	hSizes[0] = hSplitterSize;

	vSplitter->widget(1)->hide();
	if( !hideErrors->isChecked() ){
	    vSplitter->widget(2)->show();
	}
	if( !hideProcs->isChecked() ){
	    vSplitter->widget(3)->show();
	}
    }
    hSplitter->setSizes( hSizes );
}

void MainWindow::hideIProcessors(bool checked)
{
    //hide inactive processors
    if(checked){
	hideProcs->setText(tr("Show Inactive Processors"));
	vSplitter->widget(3)->hide();
    }
    //show inactive processors
    else{
	hideProcs->setText(tr("Hide Inactive Processors"));
	vSplitter->widget(3)->show();
    }
}

void MainWindow::hideAProcErrors(bool checked)
{
    //hide active processors
    if(checked){
	hideErrors->setText(tr("Show Active Processor Errors"));
	vSplitter->widget(2)->hide();
    }
    //show active processors
    else{
	hideErrors->setText(tr("Hide Active Processor Errors"));
	vSplitter->widget(2)->show();
    }
}

void MainWindow::selectionChanged(int row)
{
    if( row >= 0 && ((unsigned)row) < msc->getAProcs().size() ){
	
	//set the selection color to red or green acoording to the active processor's errors
	QPalette pal = aProcTable->palette();
	pal.setColor(QPalette::Highlight, msc->getAProcs()[row]->hasErrors() ? QColor(184,16,0,180) : QColor(32,140,64,220) );
	aProcTable->setPalette(pal);

	//display the error description of active processors
	if( msc->getAProcs()[row]->hasErrors() ){
	    std::ostringstream ss;
	    ss << msc->getAProcs()[row]->getError();
	    
	    if( msc->getAProcs()[row]->hasErrorCols() ){
		ss << std::endl;
		msc->dumpColErrors(row, ss, true);
	    }
	    aProcErrors->setPlainText( ss.str().c_str() );
	}
	else{
	    aProcErrors->clear();
	}
    }
}

void MainWindow::addCondition(){
    ACDialog dg( msc, this, Qt::Dialog );
    dg.resize(400,150);
    if( dg.exec() ){
        updateConds();
        updateAProcessors(aProcTable->currentRow());
	emit modifiedContent();
    }
}

void MainWindow::editCondition(){
    int row=condTable->currentRow();
    if( row >= 0 && row < condTable->rowCount() ){
	ECDialog ec( msc, condTable->currentItem()->text().toStdString(), this, Qt::Dialog );
	ec.resize(400,150);
	if( ec.exec() ){
	    updateConds();
	    updateAProcessors(aProcTable->currentRow());
	    emit modifiedContent();
	}
    }
}                                                                                                                                                            

void MainWindow::remCondition(){
    if( msc->getPConditions().size() > 0 ){
        int ret = QMessageBox::warning(this, tr("Delete Condition"),
                tr( "Delete selected condition?\nThe condition will also be removed from every processor afected by it!!\n"),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No | QMessageBox::Escape);
                                                                                                                                                             
        if( ret == QMessageBox::Yes ){
            msc->remCondition( condTable->currentItem()->text().toStdString() );
            updateConds( condTable->currentRow() );
	    updateAProcessors(aProcTable->currentRow());
	    emit modifiedContent();
        }
    }
}

void MainWindow::addAProcessor()
{
    APDialog dg( msc, this, Qt::Dialog );
    dg.resize(700,300);
    dg.exec();
    emit modifiedContent();
}

void MainWindow::remAProcessor()
{   
    int pos = aProcTable->currentRow();
    if( pos >= 0 && pos < aProcTable->rowCount() ){
    
	int ret = QMessageBox::warning(this, tr("Delete Processor"),
		tr( "Delete selected processor?\nAll parameter values for this processor will also be deleted!!\n"
		    "If you don't want this processor to be executed, but still keep it's parameter values saved, "
		    "try the \"Deactivate\" button instead."),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No | QMessageBox::Escape);

	if( ret == QMessageBox::No ){
	    return;
	}

	msc->remProcessor( pos, ACTIVE );
	updateAProcessors(pos);
	emit modifiedContent();
    }
}

void MainWindow::remIProcessor()
{
    int pos = iProcTable->currentRow();
    if( pos >= 0 && pos < iProcTable->rowCount() ){
	
	int ret = QMessageBox::warning(this, tr("Delete Processor"),
		tr("Delete selected processor?\nAll parameter values for this processor will also be deleted!!"),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No | QMessageBox::Escape);

	if( ret == QMessageBox::No ){
	    return;
	}

	msc->remProcessor( pos, INACTIVE );
	updateIProcessors(pos);
	emit modifiedContent();
    }
}

void MainWindow::editAProcessor(int row)
{
    int pos;
   
    //if new processor
    if(row != -1 ){
	updateAProcessors(row);
	pos=row;
    }
    //existing processor
    else{
	pos = aProcTable->currentRow();
    }
    
    if( pos >= 0 && msc->getAProcs()[pos]->isInstalled() ){

	//save the current processor before editing it
	CCProcessor *p = new CCProcessor( *msc->getAProcs()[pos] );
	
	Dialog dg( msc->getAProcs()[pos], msc, this, Qt::Window );
	//dg.resize(1100,900);
	dg.setWindowState( Qt::WindowMaximized);

	if(dg.exec()){
	    delete p;
	}
	else{
	    delete msc->getAProcs()[pos];
	    msc->getAProcs()[pos]=p;
	}
	msc->consistencyCheck();
	updateAProcessors(pos);
    }	    
}

void MainWindow::editIProcessor()
{
    int pos = iProcTable->currentRow();
    if( pos >= 0 ){
	if( msc->getIProcs()[pos]->isInstalled() ){
	    //save the current processor before editing it
	    CCProcessor *p = new CCProcessor( *msc->getIProcs()[pos] );
	    
	    Dialog dg( msc->getIProcs()[pos], msc, this, Qt::Window );
	    //dg.resize(1100,900);
	    dg.setWindowState( Qt::WindowMaximized);
	    
	    if(dg.exec()){
		delete p;
	    }
	    else{
		delete msc->getIProcs()[pos];
		msc->getIProcs()[pos]=p;
	    }
	}
	else{
	    QMessageBox::warning(this, tr("Activate Processor"),
		tr( "Sorry, you cannot edit this processor because it is not installed in your Marlin binary.\n"
		    "Install it first and then try again")
	    );
	}
    }	    
}

void MainWindow::activateProcessor()
{
    int pos = iProcTable->currentRow();
    if( pos >= 0 ){
	if(msc->getIProcs()[pos]->isInstalled()){
	    msc->activateProcessor( pos );
	    updateIProcessors(pos);
	    updateAProcessors();
	    emit modifiedContent();
	}
	else{
	    QMessageBox::warning(this, tr("Activate Processor"),
		tr( "Sorry, you cannot activate this processor because it is not installed in your Marlin binary.\n"
		    "Install it first and then try again")
	    );
	}
    }
}

void MainWindow::deactivateProcessor()
{
    int pos = aProcTable->currentRow();
    if( pos >= 0 ){
	msc->deactivateProcessor( pos );
	updateAProcessors(pos);
	updateIProcessors();
	emit modifiedContent();
    }
}

void MainWindow::moveProcessorUp()
{
    int pos = aProcTable->currentRow();
    if( pos > 0 ){
	msc->changeProcessorPos( pos, pos - 1 );
	updateAProcessors(pos - 1);
	emit modifiedContent();
    }
}

void MainWindow::moveProcessorDown()
{
    int pos = aProcTable->currentRow();
    if( pos < aProcTable->rowCount() - 1 ){
	msc->changeProcessorPos( pos, pos + 1 );
	updateAProcessors(pos + 1);
	emit modifiedContent();
    }
}

void MainWindow::moveLCIOFileUp()
{
    int pos = lcioFilesList->currentRow();
    if( pos > 0 ){
	msc->changeLCIOFilePos( pos, pos - 1 );
	updateFiles(pos -1);
	emit modifiedContent();
    }
}

void MainWindow::moveLCIOFileDown()
{
    int pos = lcioFilesList->currentRow();
    if( pos < lcioFilesList->count() - 1 ){
	msc->changeLCIOFilePos( pos, pos + 1 );
	updateFiles(pos + 1);
	emit modifiedContent();
    }
}

void MainWindow::changeGearFile()
{
    QString absFileName = QFileDialog::getOpenFileName(this, tr("Choose a Gear file"), "", "*.xml", 0, QFileDialog::DontResolveSymlinks);
   
    if( !absFileName.isEmpty() ){
	//get the current Path
	QDir dir;

	//test if the file is on the same directory-branch from the working directory
	StringVec currentPath, filePath;
	msc->getMProcs()->tokenize(dir.absolutePath().toStdString(),currentPath,"/");
	msc->getMProcs()->tokenize(absFileName.toStdString(),filePath,"/");

	QString fileName;
	//if file is on the same directory-branch from the working directory take relative path
	if(currentPath[0]==filePath[0]){
	    fileName = dir.relativeFilePath(absFileName);
	}
	//else take absolute path
	else{
	    fileName = absFileName;
	}

    //if( !absFileName.isEmpty() ){
	//QFileInfo xmlFile(msc->getXMLFile().c_str());
	////extract the absolute path from the xml file
	//QDir dir(xmlFile.absolutePath());
	////get the relative path for the GEAR file
	//QString fileName = dir.relativeFilePath(absFileName);
     
    statusBar()->showMessage(tr("Changed GEAR File to %1").arg(fileName), 2000);
	msc->getGlobalParameters()->erase( "GearXMLFile" );
	StringVec file;
	file.push_back( fileName.toStdString() );
	msc->getGlobalParameters()->add( "GearXMLFile", file );
	updateGlobalSection();
	emit modifiedContent();
    }
}

void MainWindow::addLCIOFile()
{
    QString absFileName = QFileDialog::getOpenFileName(this, tr("Choose an LCIO file"), "", "*.slcio", 0, QFileDialog::DontResolveSymlinks);
/*    
    QString fileName;
    
    if( _file != "Untitled.xml" ){
	QFileInfo xmlFile(msc->getXMLFile().c_str());
	//extract the absolute path from the xml file
	QDir dir(xmlFile.absolutePath());
	//get the relative path for the LCIO file
	fileName = dir.relativeFilePath(absFileName);
    }
    else{
	//get the current Path
	QDir dir;
	//get the relative path for the LCIO file
	fileName = dir.relativeFilePath(absFileName);
    }
*/
    if( !absFileName.isEmpty() ){
	//get the current Path
	QDir dir;

	//test if the file is on the same directory-branch from the working directory
	StringVec currentPath, filePath;
	msc->getMProcs()->tokenize(dir.absolutePath().toStdString(),currentPath,"/");
	msc->getMProcs()->tokenize(absFileName.toStdString(),filePath,"/");

	QString fileName;
	//if file is on the same directory-branch from the working directory take relative path
	if(currentPath[0]==filePath[0]){
	    fileName = dir.relativeFilePath(absFileName);
	}
	//else take absolute path
	else{
	    fileName = absFileName;
	}

	QDir newFile(absFileName);
	QFileInfo newFileI(absFileName);
	for( unsigned int i=0; i<msc->getLCIOFiles().size(); i++){
	    QDir existingFile(msc->getLCIOFiles()[i].c_str());
	    QFileInfo existingFileI(msc->getLCIOFiles()[i].c_str());
	    if((newFile.canonicalPath() == existingFile.canonicalPath()) || 
		    (newFileI.fileName() == existingFileI.fileName()) ){
		QString error="Error opening LCIO file [";
		error+=absFileName;
		error+="]. This file has already been opened!!";
		QMessageBox::critical(this, tr("Error Opening LCIO File"), error );
		return;
	    }
	}
	msc->addLCIOFile( fileName.toStdString().c_str() );
        statusBar()->showMessage(tr("Added LCIO File %1").arg(fileName), 2000);
	updateFiles();
	updateAProcessors();
	updateIProcessors();
	emit modifiedContent();
    }
}

void MainWindow::remLCIOFile()
{
    if( lcioFilesList->currentRow() != -1 ){
	msc->remLCIOFile(lcioFilesList->currentItem()->text().toStdString() );
	statusBar()->showMessage(tr("Removed LCIO File %1").arg(lcioFilesList->currentItem()->text()), 2000);
	updateFiles();
	updateAProcessors();
	updateIProcessors();
	emit modifiedContent();
    }
}

void MainWindow::openXMLFile()
{
    if( _modified ){
	int ret = QMessageBox::warning(this, tr("Exit Marlin GUI"), saveChangesMsg,
	    QMessageBox::Yes,
	    QMessageBox::No | QMessageBox::Default,
	    QMessageBox::Cancel | QMessageBox::Escape);

	if( ret == QMessageBox::Yes ){
	    saveXMLFile();
	}
	else if( ret == QMessageBox::Cancel ){
	    return;
	}
    }

    QString fileName = QFileDialog::getOpenFileName(this,
	    tr("Choose a Marlin Steering File to open..."),
	    QDir::currentPath(),
	    "*.xml\n*.steer"
    );
    
    if( !fileName.isEmpty() ){
	QFileInfo xmlFile(fileName);
	//get the current path
	QDir dir(QDir::currentPath());
	//get the relative path for the LCIO file
	fileName = dir.relativeFilePath(xmlFile.absoluteFilePath());

	setMarlinSteerCheck(fileName.toStdString().c_str());
        statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
    }
}

void MainWindow::newXMLFile(){
  
    // bug fix: when editing global parameters the values only get saved
    // after the edit box has lost focus
    this->setFocus();

    if( _modified ){
        int ret = QMessageBox::warning(this, tr("Exit Marlin GUI"), saveChangesMsg,
            QMessageBox::Yes,
            QMessageBox::No | QMessageBox::Default,
            QMessageBox::Cancel | QMessageBox::Escape);

        if( ret == QMessageBox::Yes ){
            saveXMLFile();
        }
        else if( ret == QMessageBox::Cancel ){
            return;
        }
    }


    _file="Untitled.xml";
    _modified=false;
    _saved=false;
    setWindowTitle(tr("Marlin GUI - Untitled.xml"));

    setCentralWidget(hSplitter);
  
    if( msc != NULL ){
	delete msc;
    }
    
    msc = new MarlinSteerCheck();
    
    updateGlobalSection();
    updateFiles();
    updateAProcessors();
    updateIProcessors();
}

void MainWindow::saveXMLFile(){

    // bug fix: when editing global parameters the values only get saved
    // after the edit box has lost focus
    this->setFocus();

    //create backup file
    std::string cmd= "ls ";
    cmd+=_file;
    cmd+= " >/dev/null 2>/dev/null";
    //if file already exists show overwrite warning
    if( !_saved && !system( cmd.c_str()) ){
        QString msg=_file.c_str();
        msg+=" already exists. Do you want to replace it?";
        int ret = QMessageBox::warning(this, tr("Save"), msg,
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Default);
        if( ret == QMessageBox::No ){
            saveAsXMLFile();
            return;
        }
    }

    if( !_saved && _file == "Untitled.xml" ){
        saveAsXMLFile();
    }
    else{
        if(!msc->saveAsXMLFile(_file)){
            QMessageBox::critical(this,
                    tr("Error Saving File"),
                    tr("Sorry, there was an error saving the file. Please choose another file.")
                    );
            saveAsXMLFile();
            return;
        }
        _modified=false;
        _saved=true;
        statusBar()->showMessage(tr("Saved %1").arg(QString(_file.c_str())), 2000);
    }
}

void MainWindow::saveAsXMLFile(){

    // bug fix: when editing global parameters the values only get saved
    // after the edit box has lost focus
    this->setFocus();

    QFileDialog *fd = new QFileDialog(this, tr("Save file as"), QDir::currentPath(), "*.xml");

    fd->setDefaultSuffix( "xml" );
    fd->setAcceptMode( QFileDialog::AcceptSave );
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setResolveSymlinks( false );
    fd->setFilter( "*.xml" );

    QString fileName;

    if( fd->exec() ){
        fileName=fd->selectedFiles().at(0);
    }

    if( !fileName.isEmpty() ){

        //if it is an old steering file append .xml at the end
        if( !fileName.contains(".xml", Qt::CaseInsensitive ) ){
            QString error;
            error="Sorry, you have tried to save the file with an extension different than \".xml\"\n"
                "Marlin GUI does not support other extensions than the xml one.. \nThe filename you've chosen (";
            error+=fileName;
            error+=") was therefore renamed to (";
            error+=fileName;
            error+=".xml)";

            QMessageBox::critical(this,
                    tr( "Error Saving File"),
                    error
                    );
            fileName+=".xml";
            _saved=false;
        }
        else{
            _saved=true;
        }

        //update the window title bar
        QString title= "Marlin GUI - ";
        title+=fileName;
        setWindowTitle(title);

        _file=fileName.toStdString();
        saveXMLFile();
    }
}

void MainWindow::madeChanges(){
    _modified=true;
}

void MainWindow::checkCurrentStyle()
{
    foreach (QAction *action, styleActionGroup->actions()){
        QString styleName = action->data().toString();
        QStyle *candidate = QStyleFactory::create(styleName);
        Q_ASSERT(candidate);
        if(candidate->metaObject()->className() == QApplication::style()->metaObject()->className()){
            action->trigger();
            return;
        }
        delete candidate;
    }
}

void MainWindow::changeStyle(bool checked)
{
    if (!checked){ return; }
    QAction *action = qobject_cast<QAction *>(sender());
    QStyle *style = QStyleFactory::create(action->data().toString());
    Q_ASSERT(style);
    QApplication::setStyle(style);
}


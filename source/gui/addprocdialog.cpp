#include <QtGui>

#include "addprocdialog.h"

#include "marlin/MarlinSteerCheck.h"
#include "marlin/CMProcessor.h"

using namespace std;

APDialog::APDialog( MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QDialog(parent,f), _msc(msc)
{
    _parent = qobject_cast<QMainWindow *>(parent);

    connect(this, SIGNAL(editProcessor(int)), _parent, SLOT(editAProcessor(int)));
    
    mainLayout = new QVBoxLayout;
    
    //Processor Description
    procLabel = new QLabel;
    procLabel->setWordWrap(true);
    
    QVBoxLayout *descLO = new QVBoxLayout;
    descLO->addWidget( procLabel );
    
    QGroupBox *descGB = new QGroupBox(tr("Processor Description "), this);
    descGB->setLayout( descLO );
    descGB->setMinimumHeight( 80 );
    
    //Processor Type
    cb = new QComboBox;
    procTypes=msc->getMProcs()->getProcDesc();
    
    for( ssMap::const_iterator p=procTypes.begin(); p != procTypes.end(); p++ ){
	if( (p->first =="AIDAProcessor" && _msc->existsProcessor("AIDAProcessor")==1 ) || !_msc->getMProcs()->isInstalled( p->first ) ){
	    continue;
	}
        cb->addItem((*p).first.c_str());
    }
    
    connect(cb, SIGNAL(highlighted(const QString&)), this, SLOT(changeLabel(const QString&)));
    
    QVBoxLayout *typeLO = new QVBoxLayout;
    typeLO->addWidget( descGB );
    typeLO->addWidget( cb );
    
    QGroupBox *typeGB = new QGroupBox(tr("Processor Type "), this);
    typeGB->setLayout( typeLO );
    
   
    //Processor Name
    le = new QLineEdit;
    
    QVBoxLayout *nameLO = new QVBoxLayout;
    nameLO->addWidget( le );
    
    QGroupBox *nameGB = new QGroupBox(tr("Processor Name "), this);
    nameGB->setLayout( nameLO );
    
    //Buttons
    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    okButton->setAutoDefault(true);

    connect(okButton, SIGNAL(clicked()), this, SLOT(addProcessor()));
    connect(this, SIGNAL(apply()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    
    QHBoxLayout *buttonsLO = new QHBoxLayout;
    buttonsLO->addWidget( okButton );
    buttonsLO->addWidget( cancelButton );
    
    QWidget *buttonsWG = new QWidget(this);
    buttonsWG->setLayout( buttonsLO );
    
    
    mainLayout->addWidget(typeGB);
    mainLayout->addWidget(nameGB);
    mainLayout->addWidget(buttonsWG);

    setLayout(mainLayout);

    changeLabel( cb->currentText() );

    setWindowTitle(tr("Add New Processor"));
}

void APDialog::changeLabel(const QString& text){

    //set the label with the processor description
    if(procTypes.find(text.toStdString()) != procTypes.end()){
	procLabel->setText(procTypes[text.toStdString()].c_str());
	QString name="My";
	name+=text;
	le->setText(name);
    }
}

void APDialog::addProcessor(){

    if( le->displayText().isEmpty() ){
	QMessageBox::warning(this, tr("Add New Processor"),
		tr("You cannot add a processor without name!!\nType a name in the appropriate edit box...") );
	return;
    }

    int existsProc = _msc->existsProcessor(cb->currentText().toStdString(), le->displayText().toStdString() );

    if( !existsProc ){
	//add new processor
	_msc->addProcessor( ACTIVE, le->displayText().toStdString(), cb->currentText().toStdString() );
	
	//edit processor
	emit( editProcessor( (int)_msc->getAProcs().size()-1 ));

	//aply changes
	emit( apply() );
    }
    else{
	QString error;
	if( existsProc == 1 ){
	    error+="An active";
	}
	else{
	    error+="An inactive";
	}
	error+=" processor with the same name & type already exists...\nPlease choose another name";
	
	QMessageBox::warning(this, tr("Add New Processor"), error );
    }
}

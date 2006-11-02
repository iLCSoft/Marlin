#include <QtGui>

#include "addprocdialog.h"

#include "marlin/MarlinSteerCheck.h"

using namespace std;

APDialog::APDialog( MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QDialog(parent,f), _msc(msc)
{
    _parent = qobject_cast<QMainWindow *>(parent);

    connect(this, SIGNAL(editProcessor(int)), _parent, SLOT(editAProcessor(int)));
    
    mainLayout = new QVBoxLayout;
    
    //Processor Type
    cb = new QComboBox;

    sSet procTypes=msc->getAvailableProcTypes();
    
    for( sSet::const_iterator p=procTypes.begin(); p != procTypes.end(); p++ ){
        cb->addItem((*p).c_str());
    }
    
    QVBoxLayout *typeLO = new QVBoxLayout;
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
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));

    connect(okButton, SIGNAL(clicked()), this, SLOT(addProcessor()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
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

    setWindowTitle(tr("Add New Processor"));
}

void APDialog::addProcessor(){

    //add new processor
    _msc->addProcessor( ACTIVE, le->displayText().toStdString(), cb->currentText().toStdString() );
    
    //edit processor
    emit( editProcessor( (int)_msc->getAProcs().size()-1 ));
}

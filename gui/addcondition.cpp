#include <QtGui>

#include "addcondition.h"

#include "marlin/MarlinSteerCheck.h"

using namespace std;

ACDialog::ACDialog( MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QDialog(parent,f), _msc(msc)
{
    _parent = qobject_cast<QMainWindow *>(parent);

    connect(this, SIGNAL(apply()), this, SLOT(accept()));

    mainLayout = new QVBoxLayout;
  
    le = new QLineEdit;
    
    QVBoxLayout *condLO = new QVBoxLayout;
    condLO->addWidget( le );
    
    QGroupBox *condGB = new QGroupBox(tr("Condition"), this);
    condGB->setLayout( condLO );
    
    //Buttons
    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));

    okButton->setAutoDefault(true);

    connect(okButton, SIGNAL(clicked()), this, SLOT(addCondition()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    
    QHBoxLayout *buttonsLO = new QHBoxLayout;
    buttonsLO->addWidget( okButton );
    buttonsLO->addWidget( cancelButton );
    
    QWidget *buttonsWG = new QWidget(this);
    buttonsWG->setLayout( buttonsLO );
    
    mainLayout->addWidget(condGB);
    mainLayout->addWidget(buttonsWG);

    setLayout(mainLayout);

    setWindowTitle(tr("Add New Condition"));
}

void ACDialog::addCondition(){
    _msc->addCondition( le->text().toStdString() );
    
    emit(apply());
}

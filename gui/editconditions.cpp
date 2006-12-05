#include <QtGui>

#include "editconditions.h"
#include "addcondition.h"

#include "marlin/MarlinSteerCheck.h"

using namespace std;

ECWidget::ECWidget( MarlinSteerCheck* msc, QWidget *parent, Qt::WFlags f) : QWidget(0,f), _msc(msc)
{
    _parent = qobject_cast<QMainWindow *>(parent);

    connect(this, SIGNAL(condChanged()), _parent, SLOT(updateAProcessors()));
    connect(this, SIGNAL(condChanged()), _parent, SIGNAL(modifiedContent()));
    
    condTable = new QTableWidget;
    
    QStringList labels;
    labels << tr("Condition");
    condTable->setColumnCount(1);
    condTable->setHorizontalHeaderLabels(labels);
    condTable->horizontalHeader()->resizeSection(0, 500);
    condTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    condTable->setSelectionMode(QAbstractItemView::SingleSelection);
    condTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    //Buttons
    QPushButton *addCond = new QPushButton(tr("&Add"));
    QPushButton *remCond = new QPushButton(tr("&Remove"));
                                                                                                                                                             
    addCond->setToolTip(tr("Add New Condition"));
    remCond->setToolTip(tr("Remove Selected Condition"));
                                                                                                                                                             
    connect(addCond, SIGNAL(clicked()), this, SLOT(addCondition()));
    connect(remCond, SIGNAL(clicked()), this, SLOT(remCondition()));
                                                                                                                                                             
    //Buttons Layout
    QVBoxLayout *condButtonsLayout = new QVBoxLayout;
    condButtonsLayout->addWidget(addCond);
    condButtonsLayout->addWidget(remCond);
                                                                                                                                                             
    QWidget *condButtons = new QWidget;
    condButtons->setLayout( condButtonsLayout );
    
    //Layout
    QGridLayout *condLayout = new QGridLayout;
    condLayout->addWidget(condTable,0,0);
    condLayout->addWidget(condButtons,0,1,Qt::AlignTop);
 
    //GroupBox
    QGroupBox *condGBox = new QGroupBox(tr("Conditions List"));
    condGBox->setLayout(condLayout);

    //Layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(condGBox);
 
    setLayout(mainLayout);

    setWindowTitle(tr("Edit Conditions"));

    updateConds();
}

void ECWidget::updateConds( int pos ){
    condTable->setRowCount(0);
    for( sSet::const_iterator p=_msc->getPConditions().begin(); p!=_msc->getPConditions().end(); p++ ){
	int row = condTable->rowCount();
	condTable->setRowCount(row + 1);

	QTableWidgetItem *item = new QTableWidgetItem((*p).c_str());
	condTable->setItem(row, 0, item);
    }
    if( pos == -1 ){
        selectRow(condTable->rowCount());
    }
    else{
        selectRow(pos);
    }
}

void ECWidget::addCondition(){
    ACDialog dg( _msc, this, Qt::Dialog | Qt::WindowStaysOnTopHint );
    dg.resize(400,150);
    if( dg.exec() ){
	updateConds();
	emit(condChanged());
    }
}

void ECWidget::remCondition(){
    if( _msc->getPConditions().size() > 0 ){
	int ret = QMessageBox::warning(this, tr("Delete Condition"),
                tr( "Delete selected condition?\nThe condition will also be removed from every processor afected by it!!\n"),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No | QMessageBox::Escape);
                                                                                                                                                             
        if( ret == QMessageBox::Yes ){
	    _msc->remCondition( condTable->currentItem()->text().toStdString() );
	    updateConds( condTable->currentRow() );
	    emit(condChanged());
	}
    }
}

void ECWidget::selectRow( int row ){
    if( row>=0 && row<=condTable->rowCount() ){
        if( row==condTable->rowCount() ){ row--; }
        condTable->setCurrentCell( row, 0 );
        condTable->setItemSelected( condTable->currentItem(), true );
    }
}

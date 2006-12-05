#ifndef EDITCONDITIONS_H
#define EDITCONDITIONS_H

#include <QWidget>

#include "marlin/MarlinSteerCheck.h"

class QVBoxLayout;
class QTableWidget;
class QMainWindow;

using namespace marlin;

class ECWidget : public QWidget
{
    Q_OBJECT

public:
    ECWidget(MarlinSteerCheck* msc, QWidget *parent = 0, Qt::WFlags f = 0);

signals:
    void condChanged();
    
private slots:
    void selectRow( int row );
    void updateConds( int pos=-1 );
    void addCondition();
    void remCondition();

private:
    //Variables
    QMainWindow* _parent;
    QVBoxLayout* mainLayout;

    QTableWidget* condTable;
    
    MarlinSteerCheck* _msc;
};

#endif

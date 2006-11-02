#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "marlin/MarlinSteerCheck.h"

class QListWidget;
class QTableWidget;
class QTableWidgetItem;
class QGroupBox;
class QMenu;
class QSplitter;
class QPushButton;
class QByteArray;
class QTextEdit;

using namespace marlin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void setMarlinSteerCheck( const char* filename=NULL );

signals:
    void modifiedContent();

public slots:
    void aProcNameChanged();
    void iProcNameChanged();

    void editAProcessor(int row=-1);

protected slots:
    virtual void closeEvent(QCloseEvent *e);
	
private slots:
    void madeChanges();
    void selectionChanged(int row);
    void hideIProcessors(bool checked);
    void hideAProcErrors(bool checked);
    
    void addAProcessor();
    void remAProcessor();
    void remIProcessor();
    void editIProcessor();
    void activateProcessor();
    void deactivateProcessor();
    void moveProcessorUp();
    void moveProcessorDown();
    void changeGearFile();
    void addLCIOFile();
    void remLCIOFile();
    void newXMLFile();
    void openXMLFile();
    void saveXMLFile();
    void saveAsXMLFile();

private:
    void setupViews();
    void updateProcessors();
    void updateAProcessors( int row=-1 );
    void updateIProcessors( int row=-1 );
    void updateFiles();
    void updateGlobalSection();
    void selectRow( QTableWidget* t, int row, bool colors=false );
    QMenu* createMenu();

    //variables
    bool _modified;
    std::string _file;
    MarlinSteerCheck* msc;
    MarlinSteerCheck* mx;
    ProcVec _aProc;
    ProcVec _iProc;
    ColVec _lcioCols;
    StringVec _lcioFiles;

    QGroupBox *aProcErrorsGBox;
    QGroupBox *viewButtonsGBox;
    QGroupBox *aProcButtonsGBox;
    QGroupBox *iProcButtonsGBox;
    QGroupBox *aProcGBox;
    QGroupBox *iProcGBox;
    QGroupBox *lcioFilesGBox;
    QGroupBox *lcioColsGBox;
    QGroupBox *globalSectionGBox;
    
    QTableWidget *aProcTable;
    QTableWidget *iProcTable;
    QTableWidget *globalSectionTable;
    QTableWidget *lcioColsTable;
    QListWidget *lcioFilesList;

    QTextEdit *aProcErrors;
    
    QPushButton *hideProcs;
    QPushButton *hideErrors;
    QList<int> vSizes;
    int vSplitterESize;
    int vSplitterISize;
    QSplitter *vSplitter;
    QSplitter *hSplitter;
    
    QWidget *centralWidget;
};

#endif

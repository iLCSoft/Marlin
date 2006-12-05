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
class QLabel;
class QString;
class QActionGroup;
class QTextBrowser;

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
    void changeStyle(bool checked);
    void aboutGUI();
    void help();
    void madeChanges();
    void selectionChanged(int row);
    void conditionChanged(int row, int col);
    void hideIProcessors(bool checked);
    void hideAProcErrors(bool checked);
    void showConditions(bool checked);
    void updateAProcessors( int pos=-1 );
    
    void editConditions();
    void addAProcessor();
    void remAProcessor();
    void remIProcessor();
    void editIProcessor();
    void activateProcessor();
    void deactivateProcessor();
    void moveLCIOFileUp();
    void moveLCIOFileDown();
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
    void updateIProcessors( int pos=-1 );
    void updateFiles( int pos=-1 );
    void updateGlobalSection();
    void selectRow( QTableWidget* t, int row, bool colors=false );
    void selectLCIORow( QListWidget* t, int row );
    void createMenus();
    void checkCurrentStyle();

    //variables
    bool _modified;
    bool _saved;
    std::string _file;
    MarlinSteerCheck* msc;

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
    
    QString saveChangesMsg;
    QString aboutGUIMsg;
    
    QPushButton *hideProcs;
    QPushButton *hideErrors;
    QPushButton *showCond;

    QList<int> vSizes;
    QList<int> hSizes;
    int vSplitterESize;
    int vSplitterISize;
    int hSplitterSize;
    QSplitter *vSplitter;
    QSplitter *hSplitter;
    
    QWidget *centralWidget;
    QWidget *ec;

    QActionGroup *styleActionGroup;

    QTextBrowser *browser;
};

#endif

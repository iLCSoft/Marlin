#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**@class
 * This class is the main window of Marlin GUI and therefore the most important class.
 * 
 * Marlin GUI is just a graphical representation for a model that is set by the function:
 * void setMarlinSteerCheck( const char* filename=NULL );
 * 
 * The model parses the given xml file and creates the needed objects for performing a 
 * full consistency check of the steering file.
 *
 * When the user makes changes in the GUI, the model gets automatically updated and
 * consistency checks are made if needed.
 *
 * @author Jan Engels, DESY
 * @version $Id: mainwindow.h,v 1.15 2006-12-11 10:59:28 engels Exp $
 */

#include <QMainWindow>

#include "marlin/MarlinSteerCheck.h"

class QListWidget;
class QTableWidget;
class QGroupBox;
class QSplitter;
class QPushButton;
class QTextEdit;
class QString;
class QActionGroup;

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

private slots:
    virtual void closeEvent(QCloseEvent *e);
    void changeStyle(bool checked);
    void aboutGUI();
    void help();
    void madeChanges();
    void selectionChanged(int row);
    void conditionChanged(int row, int col);
    void hideIProcessors(bool checked);
    void hideAProcErrors(bool checked);
    void showConditions(bool checked);
    void selectColumn(int col);
    void procTypeDC(int row, int col);
   
    //operations
    void addLCIOFile();
    void remLCIOFile();
    void addCondition();
    void editCondition();
    void remCondition();
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

    //file managing
    void changeGearFile();
    void newXMLFile();
    void openXMLFile();
    void saveXMLFile();
    void saveAsXMLFile();

private:
    
    //init functions
    void setupViews();
    void createMenus();
    
    //update functions
    void updateProcessors();
    void updateIProcessors( int pos=-1 );
    void updateAProcessors( int pos=-1 );
    void updateConds( int pos=-1 );
    void updateFiles( int pos=-1 );
    void updateGlobalSection();
    
    //selection functions
    void selectRow( QTableWidget* t, int row, bool colors=false );
    void selectLCIORow( QListWidget* t, int row );
    void checkCurrentStyle();

    //variables
    bool _modified;
    bool _saved;
    std::string _file;
    MarlinSteerCheck* msc;

    //group boxes
    QGroupBox *aProcErrorsGBox;
    QGroupBox *viewButtonsGBox;
    QGroupBox *aProcButtonsGBox;
    QGroupBox *iProcButtonsGBox;
    QGroupBox *aProcGBox;
    QGroupBox *iProcGBox;
    QGroupBox *condGBox;
    QGroupBox *lcioFilesGBox;
    QGroupBox *lcioColsGBox;
    QGroupBox *globalSectionGBox;
    
    //tables
    QTableWidget *aProcTable;
    QTableWidget *iProcTable;
    QTableWidget *globalSectionTable;
    QTableWidget *lcioColsTable;
    QTableWidget *condTable;
    QListWidget *lcioFilesList;

    //toggle buttons
    QPushButton *hideProcs;
    QPushButton *hideErrors;
    QPushButton *showCond;
    
    //splitters
    QSplitter *vSplitter;
    QSplitter *hSplitter;
    QList<int> hSizes;
    int hSplitterSize;
    
    //other
    QTextEdit *aProcErrors;
    QString saveChangesMsg;
    QString aboutGUIMsg;
    QActionGroup *styleActionGroup;
};

#endif

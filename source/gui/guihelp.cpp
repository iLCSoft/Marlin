#include <QtGui>

#include "guihelp.h"

GUIHelp::GUIHelp( QString path, QWidget* parent, Qt::WFlags f ) : QWidget( parent, f ){
    
    browser = new QTextBrowser;
    
    QString mPath=getenv("MARLIN");
    QString sPath=mPath;
    sPath+="/gui/help/index.html";

    QString lPath=mPath;
    lPath+=path;
    
    //set the browser home
    browser->setSource(QUrl(sPath));

    //goto path given in constructor
    browser->setSource(QUrl(lPath));
    
    //Buttons
    QPushButton *forwButton = new QPushButton(tr("->"));
    QPushButton *backButton = new QPushButton(tr("<-"));
    QPushButton *homeButton = new QPushButton(tr("TOC"));
    QPushButton *closeButton = new QPushButton(tr("Close"));
                                                                                                                                                             
    forwButton->setToolTip(tr("Forward"));
    backButton->setToolTip(tr("Backward"));
    homeButton->setToolTip(tr("Table of Contents"));
    closeButton->setToolTip(tr("Close Help"));
                                                                                                                                                             
    forwButton->setMaximumWidth(50);
    backButton->setMaximumWidth(50);
    homeButton->setMaximumWidth(90);
    homeButton->setMaximumWidth(70);
                                                                                                                                                             
    connect(forwButton, SIGNAL(clicked()), browser, SLOT(forward()));
    connect(backButton, SIGNAL(clicked()), browser, SLOT(backward()));
    connect(homeButton, SIGNAL(clicked()), browser, SLOT(home()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
                                                                                                                                                             
    //Layout
    QHBoxLayout *helpBLayout = new QHBoxLayout;
    helpBLayout->addWidget(backButton);
    helpBLayout->addWidget(forwButton);
    helpBLayout->addWidget(homeButton);
    helpBLayout->addWidget(closeButton);
                                                                                                                                                             
    //GroupBox
    QGroupBox *browserButtons = new QGroupBox(tr("Help Browser"));
    browserButtons->setLayout(helpBLayout);
    browserButtons->setMaximumWidth(250);
    browserButtons->setMaximumHeight(70);
                                                                                                                                                             
    QVBoxLayout *helpLayout = new QVBoxLayout;
    helpLayout->addWidget(browserButtons);
    helpLayout->addWidget(browser);
    
    setLayout(helpLayout);
    resize(1000,800);
    setWindowTitle(tr("Marlin GUI Help"));
}


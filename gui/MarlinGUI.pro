######################################################################
# QMAKE File for MarlinGUI
######################################################################

TEMPLATE = app
CONFIG -= moc
CONFIG += create_prl link_prl
INCLUDEPATH += $(MARLIN)/include $(LCIO)/src/cpp/include
DEFINES += MARLIN_QT
unix:LIBS += `. $(MARLIN)/bin/marlin_libs.sh` -lMarlin -L$(MARLIN)/lib
DESTDIR = ../bin/
                                                                                                                                                             
# Input
HEADERS	=   mainwindow.h \
	    aprocdelegate.h \
	    iprocdelegate.h \
	    paramdelegate.h \
	    gparamdelegate.h \
	    icoldelegate.h \
	    icoltdelegate.h \
	    ocoldelegate.h \
	    flowlayout.h \
	    addprocdialog.h \
	    addcondition.h \
	    editcondition.h \
	    guihelp.h \
	    dialog.h
	      
SOURCES =   main.cpp \
	    mainwindow.cpp \
	    aprocdelegate.cpp \
	    iprocdelegate.cpp \
	    paramdelegate.cpp \
	    gparamdelegate.cpp \
	    icoldelegate.cpp \
	    icoltdelegate.cpp \
	    ocoldelegate.cpp \
	    flowlayout.cpp \
	    addprocdialog.cpp \
	    addcondition.cpp \
	    editcondition.cpp \
	    guihelp.cpp \
	    dialog.cpp
#unix:!mac:LIBS+= -lm


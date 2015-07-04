TEMPLATE = app
TARGET = kontrollerlab

QT += core gui xml

HEADERS += src/klasminstruction.h \
           src/klavrcpu.h \
           src/klavrflash.h \
           src/klavrgcccompiler.h \
           src/klavrram.h \
           src/klcharvector.h \
           src/klcompilerinterface.h \
           src/klcpufeatures.h \
           src/klcpufuses.h \
           src/klcpuregisterdescription.h \
           src/kldatadisplay.h \
           src/kldebugger.h \
           src/kldebuggerbreakpoint.h \
           src/kldebuggerconfigwidget.h \
           src/kldebuggermemorymapping.h \
           src/kldocument.h \
           src/kldocumentview.h \
           #src/kldotmatrixcharacterwizard.h \
           #src/kldotmatrixwidget.h \
           #src/kleditdotswidget.h \
           src/klfilenewdialog.h \
           src/klhistogram.h \
           src/klmemorycelllistboxitem.h \
           src/klmemoryviewwidget.h \
           src/klprocess.h \
           src/klprocesshandler.h \
           src/klprocessmanager.h \
           src/klprogramfuseswidget.h \
           src/klprogrammeravrdude.h \
           src/klprogrammerconfigwidget.h \
           src/klprogrammerinterface.h \
           src/klprogrammeruisp.h \
           src/klproject.h \
           src/klprojectconfigwidget.h \
           src/klprojectmanagerwidget.h \
           src/klscope.h \
           src/klserialport.h \
           src/klserialportlistener.h \
           src/klserialterminalwidget.h \
           src/klsevensegmentwidget.h \
           src/klsourcecodetoasmrelation.h \
           src/klwizardheadersetting.h \
           src/klwizardheadersettings.h \
           src/kontrollerlab.h \
           src/klspinboxwidget.h \
    src/kldebuggermemmappingtreewidgetitem.h

FORMS +=   src/kldebuggerconfigwidgetbase.ui \
           #src/kldotmatrixcharacterwizardbase.ui \
           #src/kldotmatrixdisplaywizardbase.ui \
           src/klfilenewdialogbase.ui \
           src/klmemoryview.ui \
           src/klprogramfuseswidgetbase.ui \
           src/klprogrammerconfigwidgetbase.ui \
           src/klprojectconfigwidgetbase.ui \
           src/klprojectmanagerwidgetbase.ui \
           src/klserialterminalwidgetbase.ui \
           src/klsevensegmentwidgetbase.ui \
           src/kleditorwidgetbase.ui

SOURCES += src/klasminstruction.cpp \
           src/klavrcpu.cpp \
           src/klavrflash.cpp \
           src/klavrgcccompiler.cpp \
           src/klavrram.cpp \
           src/klcharvector.cpp \
           src/klcompilerinterface.cpp \
           src/klcpufeatures.cpp \
           src/klcpufuses.cpp \
           src/klcpuregisterdescription.cpp \
           src/kldatadisplay.cpp \
           src/kldebugger.cpp \
           src/kldebuggerbreakpoint.cpp \
           src/kldebuggerconfigwidget.cpp \
           src/kldebuggermemorymapping.cpp \
           src/kldocument.cpp \
           src/kldocumentview.cpp \
           #src/kldotmatrixcharacterwizard.cpp \
           #src/kldotmatrixwidget.cpp \
           #src/kleditdotswidget.cpp \
           src/klfilenewdialog.cpp \
           src/klhistogram.cpp \
           src/klmemorycelllistboxitem.cpp \
           src/klmemoryviewwidget.cpp \
           src/klprocess.cpp \
           src/klprocesshandler.cpp \
           src/klprocessmanager.cpp \
           src/klprogramfuseswidget.cpp \
           src/klprogrammeravrdude.cpp \
           src/klprogrammerconfigwidget.cpp \
           src/klprogrammerinterface.cpp \
           src/klprogrammeruisp.cpp \
           src/klproject.cpp \
           src/klprojectconfigwidget.cpp \
           src/klprojectmanagerwidget.cpp \
           src/klscope.cpp \
           src/klserialport.cpp \
           src/klserialportlistener.cpp \
           src/klserialterminalwidget.cpp \
           src/klsevensegmentwidget.cpp \
           src/klsourcecodetoasmrelation.cpp \
           src/klwizardheadersetting.cpp \
           src/klwizardheadersettings.cpp \
           src/kontrollerlab.cpp \
           src/main.cpp \
           src/klspinboxwidget.cpp \
    src/kldebuggermemmappingtreewidgetitem.cpp

OTHER_FILES += src/CMakeLists.txt \
               src/kontrollerlabui.rc \
               src/cpu_config.xml \
               src/registers.xml

DESTDIR = /home/andreas/0_OBJ/
OBJECTS_DIR = /home/andreas/0_OBJ/deb
MOC_DIR = /home/andreas/0_OBJ/moc
UI_DIR  = /home/andreas/0_OBJ/ui

#For KLSpinBoxWidget
INCLUDEPATH += src

#KDE
LIBS += -lkdeui -lkdecore -lktexteditor -lkparts -lkio

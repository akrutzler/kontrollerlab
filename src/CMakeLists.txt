project(kontrollerlab)
 
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES})

#For qt3support libs -> maybe there is another solution??
find_package( Qt4 REQUIRED )
set( QT_USE_QT3SUPPORT TRUE )
include( ${QT_USE_FILE} )
 
set(kontrollerlab_SRCS 
	   klasminstruction.cpp 
           klavrcpu.cpp 
           klavrflash.cpp 
           klavrgcccompiler.cpp 
           klavrram.cpp 
           klcharvector.cpp 
           klcompilerinterface.cpp 
           klcpufeatures.cpp 
           klcpufuses.cpp 
           klcpuregisterdescription.cpp 
           kldatadisplay.cpp 
           kldebugger.cpp 
           kldebuggerbreakpoint.cpp 
           kldebuggerconfigwidget.cpp 
           kldebuggermemmappinglistviewitem.cpp 
           kldebuggermemorymapping.cpp 
           kldocument.cpp 
           kldocumentview.cpp 
           kldotmatrixcharacterwizard.cpp 
           kldotmatrixwidget.cpp 
           kleditdotswidget.cpp 
           klfilenewdialog.cpp 
           klhistogram.cpp 
           klmemorycelllistboxitem.cpp 
           klmemoryviewwidget.cpp 
           klprocess.cpp 
           klprocesshandler.cpp 
           klprocessmanager.cpp 
           klprogramfuseswidget.cpp 
           klprogrammeravrdude.cpp 
           klprogrammerconfigwidget.cpp 
           klprogrammerinterface.cpp 
           klprogrammeruisp.cpp 
           klproject.cpp 
           klprojectconfigwidget.cpp 
           klprojectmanagerwidget.cpp 
           klscope.cpp 
           klserialport.cpp 
           klserialportlistener.cpp 
           klserialterminalwidget.cpp 
           klsevensegmentwidget.cpp 
           klsourcecodetoasmrelation.cpp 
           klwizardheadersetting.cpp 
           klwizardheadersettings.cpp 
           kontrollerlab.cpp 
           main.cpp 
	   klspinboxwidget.cpp
)
kde4_add_ui_files(kontrollerlab_SRCS
	 kldebuggerconfigwidgetbase.ui 
         kldotmatrixcharacterwizardbase.ui 
         kldotmatrixdisplaywizardbase.ui 
         klfilenewdialogbase.ui 
         klmemoryview.ui 
         klprogramfuseswidgetbase.ui 
         klprogrammerconfigwidgetbase.ui 
         klprojectconfigwidgetbase.ui 
         klprojectmanagerwidgetbase.ui 
         klserialterminalwidgetbase.ui 
         klsevensegmentwidgetbase.ui 
         kleditorwidgetbase.ui
)
#for klspinboxwidget
include_directories("${PROJECT_SOURCE_DIR}")
 
kde4_add_executable(kontrollerlab ${kontrollerlab_SRCS})
target_link_libraries(kontrollerlab ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KPART_LIBS} ${QT_LIBRARIES})

install(TARGETS kontrollerlab DESTINATION ${BIN_INSTALL_DIR})
install(FILES kontrollerlabui.rc 
        DESTINATION  ${DATA_INSTALL_DIR}/kontrollerlab)

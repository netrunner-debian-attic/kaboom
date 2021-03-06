######################################################################
# Automatically generated by qmake (2.01a) Tue Feb 10 00:29:55 2009
######################################################################

TEMPLATE = app
TARGET = kaboom
CONFIG += release
DEPENDPATH += .
INCLUDEPATH += .

TRANSLATIONS += \
      kaboom_ca.ts \
      kaboom_da.ts \
      kaboom_de.ts \
      kaboom_es.ts \
      kaboom_it.ts \
      kaboom_el.ts \
      kaboom_fr.ts

# Input
HEADERS += \
      kaboomsettings.h \
      choicepage.h \
      intropage.h \
      migrationpage.h \
      migrationpage_p.h  \
      migrationtool.h \
      migrationtool_p.h \
      warningpage.h \
      diroperations/diroperations.h \
      diroperations/recursivedirjob.h \
      diroperations/recursivedirjob_p.h \
      diroperations/progresswidget.h \
      richradiobutton.h


SOURCES += \
      kaboomsettings.cpp \
      choicepage.cpp \
      intropage.cpp \
      main.cpp \
      migrationpage.cpp \
      migrationtool.cpp \
      warningpage.cpp \
      diroperations/diroperations.cpp \
      diroperations/recursivedirjob.cpp \
      diroperations/progresswidget.cpp \
      richradiobutton.cpp \
      kaboomlog.cpp

RESOURCES += resources.qrc

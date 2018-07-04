QT += core gui widgets

TARGET = example
TEMPLATE = app

INCLUDEPATH +=  ../external/

DESTDIR = $${PWD}

#DEFINES += LFGUI_SEPARATE_COLOR_CHANNELS

SOURCES += main.cpp\
        ../lfgui/lfgui.cpp \
        ../lfgui/image.cpp \
        ../lfgui/font.cpp \
        ../lfgui/window.cpp \
        ../lfgui/lineedit.cpp \
        ../lfgui/slider.cpp \
        ../common_sample_code.cpp \

HEADERS  += \
        ../lfgui/font.h \
        ../lfgui/key.h \
        ../lfgui/signal.h \
        ../lfgui/lfgui.h \
        ../lfgui/image.h \
        ../lfgui/slider.h \
        ../lfgui/button.h \
        ../lfgui/checkbox.h \
        ../lfgui/radio.h \
        ../lfgui/lfgui_wrapper_qt.h \
        ../lfgui/general.h \
        ../lfgui/label.h \
        ../lfgui/lineedit.h \
        ../lfgui/window.h \
        ../common_sample_code.h \
        example.h \
        ../stb_truetype.h \
        ../stk_debugging.h \
        ../stk_misc.h \
        ../stk_timer.h

RESOURCES += lfgui_data.qrc

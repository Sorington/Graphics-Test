TEMPLATE = app
CONFIG -= qt

LIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL -lGLU -lGLEW

SOURCES += main.cpp \
    tools.cpp

OTHER_FILES += \
    shaders/vert.glsl \
    shaders/frag.glsl

HEADERS += \
    tools.h


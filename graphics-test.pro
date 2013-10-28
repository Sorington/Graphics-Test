TEMPLATE = app
CONFIG -= qt

LIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL -lGLU -lGLEW

SOURCES += main.cpp \
    tools.cpp

OTHER_FILES += \
    shaders/vert.glsl \
    shaders/frag.glsl \
    shaders/fragNormal.glsl \
    shaders/vertNormal.glsl \
    shaders/fragFBO.glsl \
    shaders/vertFBO.glsl

HEADERS += \
    tools.h


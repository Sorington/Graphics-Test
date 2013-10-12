TEMPLATE = app
CONFIG -= qt

LIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL -lGLU -lBox2D -lGLEW

SOURCES += main.cpp

OTHER_FILES += \
    shaders/vert.glsl \
    shaders/frag.glsl


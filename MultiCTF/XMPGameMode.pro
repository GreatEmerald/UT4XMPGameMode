TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += Source/Classes/XMPGameMode.cpp \
    Source/Classes/XMPArtifactNode.cpp \
    Source/Classes/XMPArtifact.cpp \
    Source/Classes/XMPArtifactRed.cpp
SOURCES += Source/Classes/XMPGameModePlugin.cpp

HEADERS += \
    Source/Classes/XMPGameMode.h \
    Source/Classes/XMPArtifactNode.h \
    Source/Classes/XMPArtifact.h \
    Source/Classes/XMPArtifactRed.h

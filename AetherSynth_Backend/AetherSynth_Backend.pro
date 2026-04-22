QT += core gui serialport network widgets

# Se usi Qt 5, decommenta la riga sotto (togli il #)
# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# L'output dell'applicazione nel log
CONFIG += console
CONFIG -= app_bundle

# Specifica i file del progetto
SOURCES += main.cpp
HEADERS +=

# Periferiche di default (di solito gestite da Qt Creator)
TARGET = AetherSynth_Backend
TEMPLATE = app

# Rimuovi file generati automaticamente in compilazioni precedenti
QMAKE_DEL_FILE += main.moc
#!/bin/bash
# Script to preview QML files with proper Qt5 environment

# Set Qt5 environment variables
export QTDIR=/usr/lib/x86_64-linux-gnu/qt5
export QT_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt5/plugins
export QML2_IMPORT_PATH=/usr/lib/x86_64-linux-gnu/qt5/qml
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
export PATH=/usr/lib/x86_64-linux-gnu/qt5/bin:/usr/bin:$PATH

# Find qmlscene
QMLSCENE=$(which qmlscene 2>/dev/null || echo "/usr/bin/qmlscene")

if [ ! -f "$QMLSCENE" ]; then
    echo "Error: qmlscene not found"
    echo "Please install: sudo apt-get install qtdeclarative5-dev"
    exit 1
fi

# Check if file exists
if [ ! -f "$1" ]; then
    echo "Error: QML file not found: $1"
    exit 1
fi

# Run qmlscene with the QML file
echo "Previewing: $1"
echo "Using qmlscene: $QMLSCENE"
echo "Qt5 Path: $QTDIR"
echo ""

$QMLSCENE "$@"


DEFINES += QPM_INIT\\(E\\)=\"E.addImportPath(QStringLiteral(\\\"qrc:/\\\"));\"
DEFINES += QPM_USE_NS
INCLUDEPATH += $$PWD
QML_IMPORT_PATH += $$PWD


include($$PWD/fr/grecko/sortfilterproxymodel/SortFilterProxyModel.pri))

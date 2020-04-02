TEMPLATE = app

QT += webenginewidgets webchannel

HEADERS += \
    highlight/myhighlighter.h \
    mainwindow.h \
    previewpage.h \
    document.h

SOURCES = \
    highlight/myhighlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    document.cpp

RESOURCES = \
    resources/markdowneditor.qrc

# Disable Qt Quick compiler because the example doesn't use QML, but more importantly so that
# the source code of the .js files is not removed from the embedded qrc file.
CONFIG -= qtquickcompiler

FORMS += \
    mainwindow.ui

DISTFILES += \
    resources/3rdparty/MARKDOWN-LICENSE.txt \
    resources/3rdparty/MARKED-LICENSE.txt

QMAKE_POST_LINK =

TRANSLATIONS = app.zh_CN.ts
TRANSLATIONS += app.en_US.ts


DESTDIR     = deployment

RC_ICONS = logo.ico


# copies the given files to the destination directory
defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

#copy translations
copyToDestDir($$PWD/translations, translations)

#include "mainwindow.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QCommandLineParser>
#include <QApplication>
#include <QScroller>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    w.setWindowTitle("Ortodox");

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Dir View Example");

    // Default built in options
    parser.addHelpOption();
    parser.addVersionOption();

    // Create an option that will ignore custom
    // folder or file icons and display them alike
    QCommandLineOption dontUseCustomDirectoryIconsOption("c", "Set QFileSystemModel::DontUseCustomDirectoryIcons");
    parser.addOption(dontUseCustomDirectoryIconsOption);

    parser.addPositionalArgument("directory", "The directory to start in.");
    parser.process(app);

    // Process root dir, if it was specified
    QString rootPath = ".";
    QStringList positionalArgs = parser.positionalArguments();
    if (!positionalArgs.isEmpty()) {
        rootPath = positionalArgs.first();
    }

    // Create SystemModel
    QFileSystemModel model;
    model.setRootPath(rootPath);

    if (parser.isSet(dontUseCustomDirectoryIconsOption))
        model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);

    w.setCurrentPath(rootPath);
    // tree.show();
    w.show();

    return app.exec();
}

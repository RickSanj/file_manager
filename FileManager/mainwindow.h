#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QInputDialog>
#include <QKeyEvent>
#include <QApplication>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include <QTranslator>
#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <quazip.h>
#include <quazipfile.h>
#include "iostream"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QLabel *label;

private slots:
    void setupTreeViews();
    void setupModels();
    void setupLayouts();
    void setupConnections();

    void handleTreeViewDoubleClicked(const QModelIndex &index);
    void handleCustomContextMenuRequested(const QPoint &pos);
    QStringList getSelectedFilePaths();

    void handleDeleteTriggered();
    void handleCutTriggered();
    void handleRenameTriggered();
    void handleOpenActionTriggered();
    void handleCopyTriggered();
    void handlePasteTriggered();

    bool copyDirectory(QDir sourceDir, QDir targetDir);
    bool eventFilter(QObject *object, QEvent *event);

    void onEnterPressed();
    void onEscPressed();
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void pasteFile(QString sourcePath, QString newFilePath);
    void pasteDir(QString sourcePath, QString destinationPath);
    void checkExistance(QString path);
    void on_langButton_clicked();
    void showProperties();
    void createNew();
    void createNewFile();
    void createNewFolder();
    void handleCompression();
    void handleExtraction();
    bool addFileToZip(const QString &filePath, QuaZipFile &outFile, const QString &rootPath);
    bool addDirectoryToZip(const QString &dirPath, QuaZipFile *outFile, const QString &rootPath);

    void processCommandLine(const QString &input);
    void handleCopyOperation(const QString &arguments);
    void navigateToHome();
    void changeDirectory(const QString &path);
    void createDirectory(const QString &dirName);
    void handleMoveOperation(QString arguments);
    bool moveDirectory(const QString &source, const QString &destination);
    void removeFileOrDirectory(const QString &target);
    void handleFileManagerAction(QTreeView *activeTreeView, QFileSystemModel *activeModel);
    void on_helpButton_clicked();

private:
    Ui::MainWindow *ui;
    QTreeView *treeViewLeft;
    QTreeView *treeViewRight;
    QFileSystemModel *modelLeft;
    QFileSystemModel *modelRight;
    QModelIndex currentIndex;
    QString copyPath;
    QString cutPath;
    QList<QString> selectedRowsBuffer;
    bool isCutOperation = false;
    QTranslator translator;

};


#endif // MAINWINDOW_H

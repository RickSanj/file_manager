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
//
//#include <quazip/quazip.h>
//#include <quazip/quazipfile.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // void handleSelectPathButtonClicked();
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
//    bool archive(const QString &zipFilePath, const QDir &dir);
//    bool addFileToZip(const QString &filePath, QuaZip &zip);

    void processCommandLine(const QString &input);
    void navigateToHome();
    void changeDirectory(const QString &path);
    void createDirectory(const QString &dirName);
    void removeFileOrDirectory(const QString &target);
    void handleFileManagerAction(QTreeView *activeTreeView, QFileSystemModel *activeModel);


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

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit CustomFileSystemModel(QObject *parent = nullptr) : QFileSystemModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        int count = QFileSystemModel::rowCount(parent);
        if (!parent.isValid() || !fileInfo(parent).isDir()) {
            return count;
        }
        return count + 1;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (index.row() == 0 && index.parent() == QModelIndex() && role == Qt::DisplayRole) {
            return QStringLiteral("../");
        }
        return QFileSystemModel::data(this->index(index.row()-1, index.column(), index.parent()), role);
    }

    // Override index to handle "../" row click
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (row == 0 && parent == QModelIndex()) {
            return createIndex(row, column);
        }
        return QFileSystemModel::index(row, column, parent);
    }

    bool isUpDirectory(const QModelIndex &index) const {
        return index.row() == 0 && index.parent() == QModelIndex();
    }
};

#endif // MAINWINDOW_H

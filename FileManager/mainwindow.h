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
    void on_pushButton_clicked();
    void handleSelectPathButtonClicked();
    void handleTreeViewDoubleClicked(const QModelIndex &index);
    void on_lineEdit_returnPressed();
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

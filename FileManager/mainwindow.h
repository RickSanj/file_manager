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
    void handleDeleteTriggered();
    void renameItem();
    void handleOpenActionTriggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QModelIndex currentIndex;
};

#endif // MAINWINDOW_H


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QFileSystemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Declare function that displays current path
    void setCurrentPath(const QString &text);

private:
    Ui::MainWindow *ui;

    // Declare label for current path text
    QLabel *directoryLabel;

    QTreeView *tree;
    QVBoxLayout *TreeLayout;
    QFileSystemModel *model;
    QWidget *centralTreeWidget;


};
#endif // MAINWINDOW_H

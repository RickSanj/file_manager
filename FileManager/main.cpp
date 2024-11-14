#include "mainwindow.h"
#include <QApplication>
#include <QApplication>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
int main(int argc, char *argv[])
{
     QApplication a(argc, argv);
     MainWindow w;
     w.show();
     return a.exec();
}

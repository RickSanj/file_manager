#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    ui->label_2->setText("Current directory: ");

    model = new QFileSystemModel(this);
    model->setRootPath(QDir::rootPath());
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(QDir::rootPath()));
    ui->lineEdit->setText(QDir::rootPath());

    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    QString path = ui->lineEdit->text();
    if (path==""){
        ui->treeView->setRootIndex(model->index(QDir::homePath()));
        ui->lineEdit->setText(QDir::homePath());
    } else{
        ui->treeView->setRootIndex(model->index(path));
    }
}


void MainWindow::handleSelectPathButtonClicked(){
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::homePath());
    if (!folderPath.isEmpty()) {
        model->setRootPath(folderPath);
        ui->treeView->setModel(model);
        ui->treeView->setRootIndex(model->index(folderPath));
        ui->lineEdit->setText(QDir::homePath());
    }
}


void MainWindow::on_lineEdit_returnPressed(){
    QString path = ui->lineEdit->text();
    if (path==""){
        ui->treeView->setRootIndex(model->index(QDir::homePath()));
        ui->lineEdit->setText(QDir::homePath());
    } else{
        ui->treeView->setRootIndex(model->index(path));
    }
}


void MainWindow::handleTreeViewDoubleClicked(const QModelIndex &index)
{
    QString path = model->filePath(index);
    QFileInfo fileInfo(path);

    if (fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

}


void MainWindow::handleCustomContextMenuRequested(const QPoint &pos)
{
    currentIndex = ui->treeView->indexAt(pos);

    if (!currentIndex.isValid()) {
        return;
    }

    QMenu contextMenu(this);

    QFileInfo fileInfo = model->fileInfo(currentIndex);

    if (fileInfo.isFile()) {
        QAction *openAction = contextMenu.addAction("Open");
        connect(openAction, &QAction::triggered, this, &MainWindow::handleOpenActionTriggered);
    }


    QAction *deleteAction = contextMenu.addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);


    QAction *renameAction = contextMenu.addAction("Rename");
    connect(renameAction, &QAction::triggered, this, &MainWindow::renameItem);


    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}


void MainWindow::renameItem()
{
    QModelIndex index = ui->treeView->currentIndex();

    if (!index.isValid()) {
        return;
    }

    QString oldName = model->fileName(index);
    QString oldFilePath = model->filePath(index);
    QFileInfo fileInfo(oldFilePath);
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "Enter new name:", QLineEdit::Normal, oldName, &ok);



    if (!ok || newName.isEmpty()) {
        QMessageBox::warning(this, "Rename Error", "Failed to rename file or folder.");
    }
    else {
        QString newFilePath = fileInfo.absolutePath() + "/" + newName;
        QFile::rename(oldFilePath, newFilePath);
    }



}


void MainWindow::handleDeleteTriggered(){
    QString path = model->filePath(currentIndex);
    QFileInfo fileInfo(path);

    if (path.isEmpty()) {
        QMessageBox::warning(this, "Delete", "No file or directory selected.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (fileInfo.isFile()) {
            if (QFile::remove(path)) {
                QMessageBox::information(this, "Delete", "File deleted successfully.");
            } else {
                QMessageBox::warning(this, "Delete", "Failed to delete file.");
            }
        } else if (fileInfo.isDir()) {
            QDir dir(path);
            if (dir.removeRecursively()) {
                QMessageBox::information(this, "Delete", "Directory deleted successfully.");
            } else {
                QMessageBox::warning(this, "Delete", "Failed to delete directory.");
            }
        }
    }
}


void MainWindow::handleOpenActionTriggered()
{
    QModelIndex index = currentIndex;
    if (!index.isValid()) {
        QMessageBox::warning(this, "Open", "No file or directory selected.");
        return;
    }
    handleTreeViewDoubleClicked(index);
}

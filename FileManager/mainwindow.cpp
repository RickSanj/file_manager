#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    ui->label_2->setText("Current directory: ");
    ui->treeView->installEventFilter(this);

    model = new QFileSystemModel(this);
    model->setRootPath(QDir::rootPath());
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(QDir::rootPath()));
    ui->treeView->setItemsExpandable(false);
    ui->treeView->setRootIsDecorated(false);
    ui->lineEdit->setText(QDir::rootPath());

    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
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


void MainWindow::handleTreeViewDoubleClicked(const QModelIndex &index){
        QString path = model->filePath(index);

        QFileInfo fileInfo(path);

        if (fileInfo.isDir()) {
            ui->treeView->setRootIndex(model->index(path));
            ui->lineEdit->setText(path);
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
}


void MainWindow::handleCustomContextMenuRequested(const QPoint &pos){
    currentIndex = ui->treeView->indexAt(pos);


    QMenu contextMenu(this);

    QFileInfo fileInfo = model->fileInfo(currentIndex);

    if (fileInfo.isFile()) {
        QAction *openAction = contextMenu.addAction("Open");
        connect(openAction, &QAction::triggered, this, &MainWindow::handleOpenActionTriggered);
    }


    QAction *copyAction = contextMenu.addAction("Copy");
    QAction *pasteAction = contextMenu.addAction("Paste");
    QAction *deleteAction = contextMenu.addAction("Delete");
    QAction *renameAction = contextMenu.addAction("Rename");

    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);
    connect(renameAction, &QAction::triggered, this, &MainWindow::renameItem);    
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopyTriggered);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPasteTriggered);

    if (copyPath.isEmpty()){
        pasteAction->setDisabled(true);
    }

    if (!currentIndex.isValid()) {
        copyAction->setDisabled(true);
        deleteAction->setDisabled(true);
        renameAction->setDisabled(true);
    }

    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}


void MainWindow::renameItem(){
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


void MainWindow::handleOpenActionTriggered(){
    QModelIndex index = currentIndex;
    if (!index.isValid()) {
        QMessageBox::warning(this, "Open", "No file or directory selected.");
        return;
    }
    handleTreeViewDoubleClicked(index);
}


void MainWindow::onCopyTriggered(){
    QString sourcePath = model->filePath(currentIndex);
    QFileInfo fileInfo(sourcePath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "Copy", "No file or directory selected.");
        return;
    }

    copyPath = sourcePath;
}


void MainWindow::onPasteTriggered() {
    QString destinationPath = model->filePath(currentIndex);
    QFileInfo destInfo(destinationPath);

    if (!destInfo.isDir()) {
        destinationPath = model->filePath(ui->treeView->rootIndex());
    }

    QFileInfo sourceInfo(copyPath);
    QString newFilePath = destinationPath + "/" + sourceInfo.fileName();

    if (QFile::exists(newFilePath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Overwrite File", "The file already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    if (sourceInfo.isFile()) {
        if (!QFile::copy(copyPath, newFilePath)) {
            QMessageBox::warning(this, "Paste", "Failed to paste the file.");
        }
    } else if (sourceInfo.isDir()) {
        QDir sourceDir(copyPath);
        QDir targetDir(destinationPath);

        QString targetPathWithSourceDir = targetDir.filePath(sourceDir.dirName());
        QDir newTargetDir(targetPathWithSourceDir);

        if (!copyDirectory(sourceDir, newTargetDir)) {
            QMessageBox::warning(this, "Paste", "Failed to paste the directory.");
        }
    }
    copyPath.clear();
}


bool MainWindow::copyDirectory(QDir sourceDir, QDir targetDir){
    if (!targetDir.exists()) {
        targetDir.mkdir(targetDir.path());
    }

    foreach (QString fileName, sourceDir.entryList(QDir::Files)) {
        QString sourceFilePath = sourceDir.filePath(fileName);
        QString targetFilePath = targetDir.filePath(fileName);
        if (!QFile::copy(sourceFilePath, targetFilePath)) {
            return false;
        }
    }

    foreach (QString subDirName, sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString sourceSubDirPath = sourceDir.filePath(subDirName);
        QString targetSubDirPath = targetDir.filePath(subDirName);
        QDir sourceSubDir(sourceSubDirPath);
        QDir targetSubDir(targetSubDirPath);
        if (!copyDirectory(sourceSubDir, targetSubDir)) {
            return false;
        }
    }

    return true;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if (object == ui->treeView && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);


        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            onEnterPressed();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Escape) {
            onEscPressed(); // Call function to go to parent directory
            return true; // Indicate that the event was handled
        }

    }
    // Standard event processing
    return QMainWindow::eventFilter(object, event);
}


void MainWindow::onEnterPressed() {
    if (ui->treeView->hasFocus()) {
        QModelIndex selectedIndex = ui->treeView->currentIndex();
        if (selectedIndex.isValid()) {
            QFileInfo fileInfo = model->fileInfo(selectedIndex);

            if (fileInfo.isDir()) {
                ui->treeView->setRootIndex(model->index(fileInfo.absoluteFilePath()));
                ui->lineEdit->setText(fileInfo.absoluteFilePath());
            } else if (fileInfo.isFile()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            }
        }
    }
}

void MainWindow::onEscPressed() {
    QModelIndex currentRootIndex = ui->treeView->rootIndex();

    QModelIndex parentIndex = currentRootIndex.parent();
    if (parentIndex.isValid()) {

        QFileInfo fileInfo = model->fileInfo(parentIndex);

        if (fileInfo.isDir()) {
            ui->treeView->setRootIndex(model->index(fileInfo.absoluteFilePath()));
            ui->lineEdit->setText(fileInfo.absoluteFilePath());
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }
}


MainWindow::~MainWindow(){
    delete ui;
}

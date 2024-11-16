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

    QAction *cutAction = contextMenu.addAction("Cut");
    QAction *copyAction = contextMenu.addAction("Copy");
    QAction *pasteAction = contextMenu.addAction("Paste");
    QAction *deleteAction = contextMenu.addAction("Delete");
    QAction *renameAction = contextMenu.addAction("Rename");

    connect(cutAction, &QAction::triggered, this, &MainWindow::handleCutTriggered);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);
    connect(renameAction, &QAction::triggered, this, &MainWindow::handleRenameTriggered);
    connect(copyAction, &QAction::triggered, this, &MainWindow::handleCopyTriggered);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::handlePasteTriggered);

    if (copyPath.isEmpty() && !cutAction){
        pasteAction->setDisabled(true);
    }

    if (!currentIndex.isValid()) {
        copyAction->setDisabled(true);
        deleteAction->setDisabled(true);
        renameAction->setDisabled(true);
    }

    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::handleOpenActionTriggered(){
    QModelIndex index = currentIndex;
    if (!index.isValid()) {
        QMessageBox::warning(this, "Open", "No file or directory selected.");
        return;
    }
    handleTreeViewDoubleClicked(index);
}

void MainWindow::handleCopyTriggered(){
    QString sourcePath = model->filePath(currentIndex);
    QFileInfo fileInfo(sourcePath);
    QModelIndex selectedIndex = ui->treeView->currentIndex();

    if (!fileInfo.exists()) {
        if(!selectedIndex.isValid()){
            QMessageBox::warning(this, "Copy", "No file or directory selected.");
            return;
        } else {
            sourcePath = model->filePath(selectedIndex);
        }
    }

    copyPath = sourcePath;
}

void MainWindow::handleCutTriggered() {
    QString sourcePath = model->filePath(currentIndex);
    QFileInfo fileInfo(sourcePath);
    QModelIndex selectedIndex = ui->treeView->currentIndex();

    if (!fileInfo.exists()) {
        if(!selectedIndex.isValid()){
            QMessageBox::warning(this, "Cut", "No file or directory selected.");
            return;
        } else {
            cutPath = model->filePath(selectedIndex);
            isCutOperation = true;
            copyPath.clear();
        }
    }
}

void MainWindow::handlePasteTriggered() {
    QString destinationPath = model->filePath(currentIndex);
    QFileInfo destInfo(destinationPath);

    if (!destInfo.isDir()) {
        destinationPath = model->filePath(ui->treeView->rootIndex());
    }

    QString sourcePath = isCutOperation ? cutPath : copyPath;
    if (sourcePath.isEmpty()) {
        QMessageBox::warning(this, "Paste", "No source file or directory to paste.");
        return;
    }

    QFileInfo sourceInfo(sourcePath);
    QString newFilePath = destinationPath + "/" + sourceInfo.fileName();

    if (QFile::exists(newFilePath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Overwrite File", "The file or directory already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }

        if (sourceInfo.isFile()) {
            QFile::remove(newFilePath);
        } else if (sourceInfo.isDir()) {
            QDir(newFilePath).removeRecursively();
        }
    }

    if (sourceInfo.isFile()) {
        if (isCutOperation) {
            if (!QFile::rename(sourcePath, newFilePath)) {
                QMessageBox::warning(this, "Paste", "Failed to move the file.");
                return;
            }
        } else {
            if (!QFile::copy(sourcePath, newFilePath)) {
                QMessageBox::warning(this, "Paste", "Failed to copy the file.");
                return;
            }
        }
    } else if (sourceInfo.isDir()) {
        QDir sourceDir(sourcePath);
        QDir targetDir(destinationPath);

        QString targetPathWithSourceDir = targetDir.filePath(sourceDir.dirName());
        QDir newTargetDir(targetPathWithSourceDir);

        if (isCutOperation) {
            if (!sourceDir.rename(sourceDir.path(), targetPathWithSourceDir)) {
                QMessageBox::warning(this, "Paste", "Failed to move the directory.");
                return;
            }
        } else {
            if (!copyDirectory(sourceDir, newTargetDir)) {
                QMessageBox::warning(this, "Paste", "Failed to copy the directory.");
                return;
            }
        }
    }

    if (isCutOperation) {
        cutPath.clear();
        isCutOperation = false;
    }
}

void MainWindow::handleDeleteTriggered(){
    QString path = model->filePath(currentIndex);
    QModelIndex selectedIndex = ui->treeView->currentIndex();
    if (path.isEmpty()) {
        if(!selectedIndex.isValid()){
            QMessageBox::warning(this, "Delete", "No file or directory selected.");
            return;
        } else {
            path = model->filePath(selectedIndex);
        }
    }
    QFileInfo fileInfo(path);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (fileInfo.isFile()) {
            if (!QFile::remove(path)) {
                QMessageBox::warning(this, "Delete", "Failed to delete file.");
            }
        } else if (fileInfo.isDir()) {
            QDir dir(path);
            if (!dir.removeRecursively()) {
                QMessageBox::warning(this, "Delete", "Failed to delete directory.");
            }
        }
    }
}

void MainWindow::handleRenameTriggered(){
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
            onEscPressed();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Delete) {
            handleDeleteTriggered();
            return true;
        }
        if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ControlModifier) {
            handleCopyTriggered();
            return true;
        }
        if (keyEvent->key() == Qt::Key_V && keyEvent->modifiers() == Qt::ControlModifier) {
            handlePasteTriggered();
            return true;
        }

        if (keyEvent->key() == Qt::Key_X && keyEvent->modifiers() == Qt::ControlModifier) {
            handleCutTriggered();
            return true;
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

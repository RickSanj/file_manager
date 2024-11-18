#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::createNewFile() {
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;


    if (ui->treeViewLeft->hasFocus()) {
        activeTreeView = ui->treeViewLeft;
        activeModel = modelLeft;
    } else if (ui->treeViewRight->hasFocus()) {
        activeTreeView = ui->treeViewRight;
        activeModel = modelRight;
    }

    if (!activeTreeView || !activeModel) {
        QMessageBox::warning(this, tr("New file"), tr("No panel is active."));
        return;
    }
    QModelIndex currentIndex = activeTreeView->currentIndex();
    QString currentPath = activeModel->filePath(currentIndex);

    if (currentPath.isEmpty()) {
        currentPath = activeModel->rootPath();
    }

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Create New File"), tr("Enter file name:"), QLineEdit::Normal, tr("newfile.txt"), &ok);

    if (ok && !fileName.isEmpty()) {
        QFile file(currentPath + "/" + fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            QMessageBox::information(this, tr("Success"), tr("File created successfully."));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to create file."));
        }
    }
}

void MainWindow::pasteFile(QString sourcePath, QString destinationPath){
    QFileInfo sourceInfo(sourcePath);
    QString newFilePath = destinationPath + "/" + sourceInfo.fileName();

    QFileInfo destInfo(newFilePath);

    if (isCutOperation) {
        if (!QFile::rename(sourcePath, newFilePath)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to move the file."));
            return;
        }
    } else {
        if (!QFile::copy(sourcePath, newFilePath)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to copy the file."));
            return;
        }
    }
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::handleCustomContextMenuRequested(const QPoint &pos) {
    QTreeView *treeView = qobject_cast<QTreeView *>(sender());
    if (!treeView) return;

    QFileSystemModel *model = nullptr;
    if (treeView == ui->treeViewLeft) {
        model = modelLeft;
    } else if (treeView == ui->treeViewRight) {
        model = modelRight;
    }
    if (!model) return;

    QModelIndex index = treeView->indexAt(pos);
    QFileInfo fileInfo = model->fileInfo(index);

    QMenu contextMenu(this);

    if (fileInfo.isFile()) {
        QString filePath = fileInfo.filePath();
        QStringList archiveExtensions = {".zip", ".rar", ".7z", ".tar", ".gz"};
        bool isArchive = false;

        for (const QString &ext : archiveExtensions) {
            if (filePath.endsWith(ext, Qt::CaseInsensitive)) {
                isArchive = true;
                break;
            }
        }

        if (isArchive) {
            QAction *extractAction = contextMenu.addAction(tr("Extract"));
            connect(extractAction, &QAction::triggered, this, &MainWindow::handleExtraction);
        } else {
            QAction *openAction = contextMenu.addAction(tr("Open"));
            connect(openAction, &QAction::triggered, this, &MainWindow::handleOpenActionTriggered);
        }
    }

    QAction *newAction = contextMenu.addAction(tr("New"));
    QAction *cutAction = contextMenu.addAction(tr("Cut"));
    QAction *copyAction = contextMenu.addAction(tr("Copy"));
    QAction *pasteAction = contextMenu.addAction(tr("Paste"));
    QAction *deleteAction = contextMenu.addAction(tr("Delete"));
    QAction *renameAction = contextMenu.addAction(tr("Rename"));
    QAction *zipAction = contextMenu.addAction(tr("Compress"));
    QAction *propertiesAction = contextMenu.addAction(tr("Properties"));


    connect(cutAction, &QAction::triggered, this, &MainWindow::handleCutTriggered);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);
    connect(renameAction, &QAction::triggered, this, &MainWindow::handleRenameTriggered);
    connect(copyAction, &QAction::triggered, this, &MainWindow::handleCopyTriggered);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::handlePasteTriggered);
    connect(propertiesAction, &QAction::triggered, this, &MainWindow::showProperties);
    connect(newAction, &QAction::triggered, this, &MainWindow::createNew);
    connect(zipAction, &QAction::triggered, this, &MainWindow::handleCompression);

    newAction->setDisabled(true);
    if (copyPath.isEmpty() && !cutAction){
        pasteAction->setDisabled(true);
    }

    if (!index.isValid()) {
        copyAction->setDisabled(true);
        deleteAction->setDisabled(true);
        renameAction->setDisabled(true);
        newAction->setEnabled(true);
    }
    contextMenu.exec(treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::handleCopyTriggered() {
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
        QMessageBox::warning(this, tr("Copy"), tr("No panel is active."));
        return;
    }


    selectedRowsBuffer = getSelectedFilePaths();

}

void MainWindow::handleCutTriggered() {
    selectedRowsBuffer = getSelectedFilePaths();

    isCutOperation = true;

}

void MainWindow::handlePasteTriggered() {
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
        QMessageBox::warning(this, tr("Paste"), tr("No panel is active."));
        return;
    }
    QModelIndex currentIndex = activeTreeView->currentIndex();
    QString destinationPath = activeModel->filePath(currentIndex);
    QFileInfo destInfo(destinationPath);


    if (!destInfo.isDir()) {
        destinationPath = activeModel->filePath(activeTreeView->rootIndex());
    }
    if (selectedRowsBuffer.isEmpty()) {
        QMessageBox::warning(this, tr("Paste"), tr("No files or directories to paste."));
        return;
    }

    for(const QString &path : selectedRowsBuffer){
        QFileInfo sourceInfo(path);

        if (!QFile::exists(path)) {
            QMessageBox::warning(this, tr("Error"), tr("Source file does not exist."));
            return;
        }
        if (path.isEmpty()) {
            QMessageBox::warning(this, tr("Paste"), tr("No source file or directory to paste."));
            return;
        }

        checkExistance(destinationPath + "/" + sourceInfo.fileName());
        if (sourceInfo.isFile()) {
            pasteFile(path, destinationPath);

        } else if (sourceInfo.isDir()) {
            pasteDir(path, destinationPath);
        }
    }

    if (isCutOperation) {
        cutPath.clear();
        copyPath.clear();
        isCutOperation = false;
        selectedRowsBuffer.clear();
    }
}

void MainWindow::handleRenameTriggered(){
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
        QMessageBox::warning(this, tr("Rename Error"), tr("No panel is active."));
        return;
    }

    QModelIndex index = activeTreeView->currentIndex();

    if (!index.isValid()) {
        return;
    }

    QString oldName = activeModel->fileName(index);
    QString oldFilePath = activeModel->filePath(index);
    QFileInfo fileInfo(oldFilePath);
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Rename"), tr("Enter new name:"), QLineEdit::Normal, oldName, &ok);

    if (!ok || newName.isEmpty()) {
        QMessageBox::warning(this, tr("Rename Error"), tr("Failed to rename file or folder."));
    }
    else {
        QString newFilePath = fileInfo.absolutePath() + "/" + newName;
        QFile::rename(oldFilePath, newFilePath);
    }
}


void MainWindow::showProperties() {
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
        QMessageBox::warning(this, tr("Properties"), tr("No panel is active."));
        return;
    }

    QModelIndex index = activeTreeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Properties"), tr("No file or directory selected."));
        return;
    }

    QString filePath = activeModel->filePath(index);
    QFileInfo fileInfo(filePath);

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Properties"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    layout->addWidget(new QLabel(tr("Name: ") + fileInfo.fileName()));
    layout->addWidget(new QLabel(tr("Path: ") + fileInfo.absoluteFilePath()));
    layout->addWidget(new QLabel(tr("Size: ") + QString::number(fileInfo.size()) + tr(" bytes")));
    QString type;
    if (fileInfo.isDir()) {
        type = tr("Folder");
    } else if (fileInfo.suffix() == "zip" || fileInfo.suffix() == "tar" || fileInfo.suffix() == "rar") {
        type = tr("Archive");
    } else {
        type = tr("File");
    }

    layout->addWidget(new QLabel(QString(tr("Type: ")) + type));
    layout->addWidget(new QLabel(tr("Last Modified: ") + fileInfo.lastModified().toString()));

    dialog->setLayout(layout);
    dialog->exec();
}

void MainWindow::handleOpenActionTriggered() {

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
        QMessageBox::warning(this, tr("Open"), tr("No panel is active."));
        return;
    }
    QModelIndex index = activeTreeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Open"), tr("No file or directory selected."));
        return;
    }
   // handleTreeViewDoubleClicked(index);
    QString path = activeModel->filePath(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::handleDeleteTriggered(){
    selectedRowsBuffer = getSelectedFilePaths();
    if (selectedRowsBuffer.isEmpty()) {
        QMessageBox::information(this, tr("Delete"), tr("No files or directories selected."));
        return;
    }

    QString itemsList = selectedRowsBuffer.join("\n");
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete"),
                                  tr("Are you sure you want to delete this?\n%1").arg(itemsList),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    for (const QString &path : selectedRowsBuffer) {
        QFileInfo fileInfo(path);
        if (fileInfo.isFile()) {
            if (!QFile::remove(path)) {
                QMessageBox::warning(this, tr("Delete"), tr("Failed to delete file:\n%1").arg(path));
            }
        } else if (fileInfo.isDir()) {
            QDir dir(path);
            if (!dir.removeRecursively()) {
                QMessageBox::warning(this, tr("Delete"), tr("Failed to delete directory:\n%1").arg(path));
            }
        }
    }

}

void MainWindow::createNew() {
    QMenu newMenu;
    QAction *newFileAction = newMenu.addAction(tr("File"));
    QAction *newFolderAction = newMenu.addAction(tr("Folder"));

    connect(newFileAction, &QAction::triggered, this, &MainWindow::createNewFile);
    connect(newFolderAction, &QAction::triggered, this, &MainWindow::createNewFolder);

    newMenu.exec(QCursor::pos());
}

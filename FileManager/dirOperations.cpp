#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::removeFileOrDirectory(const QString &target) {
    QModelIndex rootIndex = ui->treeViewLeft->rootIndex();
    QString rootPath = modelLeft->filePath(rootIndex);
    QString fullPath = QDir::cleanPath(rootPath + QDir::separator() + target);

    QFileInfo targetInfo(fullPath);

    if (targetInfo.exists()) {
        if (targetInfo.isFile()) {
            QFile file(fullPath);
            if (file.remove()) {
                statusBar()->showMessage(tr("File removed successfully."));
            } else {
                statusBar()->showMessage(tr("Error: Unable to remove file."));
            }
        } else if (targetInfo.isDir()) {
            QDir dir(fullPath);
            if (dir.removeRecursively()) {
                statusBar()->showMessage(tr("Directory removed successfully."));
            } else {
                statusBar()->showMessage(tr("Error: Unable to remove directory."));
            }
        }
    } else {
        statusBar()->showMessage(tr("Error: Target does not exist."));
    }
}

void MainWindow::createDirectory(const QString &dirName) {
    QModelIndex rootIndex = ui->treeViewLeft->rootIndex();
    QString rootPath = modelLeft->filePath(rootIndex);
    QDir dir(rootPath);
    if (dir.mkdir(dirName)) {
        statusBar()->showMessage(tr("Directory created successfully."));
    } else {
        statusBar()->showMessage(tr("Error: Unable to create directory."));
    }
}

void MainWindow::changeDirectory(const QString &path) {
    QString currentPath = ui->label->text();
    QDir dir(currentPath);

    QString fullPath = QDir::isAbsolutePath(path) ? QDir::cleanPath(path)
                                                  : QDir::cleanPath(currentPath + QDir::separator() + path);
    QFileInfo targetInfo(fullPath);

    if (path == "..") {
        if (dir.cdUp()) {
            QString parentPath = dir.absolutePath();
            ui->treeViewLeft->setRootIndex(modelLeft->index(parentPath));
            ui->label->setText(parentPath);
            statusBar()->showMessage(tr("Moved to parent directory."));
        } else {
            statusBar()->showMessage(tr("Error: Unable to move to parent directory."));
        }
    } else if (targetInfo.isDir() && targetInfo.exists()) {
        ui->treeViewLeft->setRootIndex(modelLeft->index(fullPath));
        ui->label->setText(fullPath);
        statusBar()->showMessage(tr("Moved to directory."));
    } else {
        statusBar()->showMessage(tr("Error: Directory does not exist."));
    }
}

void MainWindow::pasteDir(QString sourcePath, QString destinationPath){
    QDir sourceDir(sourcePath);
    QDir targetDir(destinationPath);

    QString targetPathWithSourceDir = targetDir.filePath(sourceDir.dirName());
    QDir newTargetDir(targetPathWithSourceDir);

    if (isCutOperation) {
        if (!sourceDir.rename(sourceDir.path(), targetPathWithSourceDir)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to move the directory."));
            return;
        }
    } else {
        if (!copyDirectory(sourceDir, newTargetDir)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to copy the directory."));
            return;
        }
    }
};

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

void MainWindow::createNewFolder() {
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
        QMessageBox::warning(this, tr("New folder"), tr("No panel is active."));
        return;
    }

    QModelIndex currentIndex = activeTreeView->currentIndex();
    QString currentPath = activeModel->filePath(currentIndex);

    if (currentPath.isEmpty()) {
        currentPath = activeModel->rootPath();
    }

    bool ok;
    QString folderName = QInputDialog::getText(this, tr("Create New Folder"), tr("Enter folder name:"), QLineEdit::Normal, tr("NewFolder"), &ok);

    if (ok && !folderName.isEmpty()) {
        QDir dir(currentPath);
        if (dir.mkdir(folderName)) {
            QMessageBox::information(this, tr("Success"), tr("Folder created successfully."));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to create folder."));
        }
    }
}

bool MainWindow::moveDirectory(const QString &source, const QString &destination) {
    QDir sourceDir(source);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir destinationDir(destination);
    if (!destinationDir.exists()) {
        if (!destinationDir.mkpath(destination)) {
            return false;
        }
    }

            foreach (QString fileName, sourceDir.entryList(QDir::Files)) {
            QString srcFilePath = sourceDir.filePath(fileName);
            QString destFilePath = destinationDir.filePath(fileName);
            if (!QFile::rename(srcFilePath, destFilePath)) {
                return false;
            }
        }

            foreach (QString subDirName, sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString srcSubDirPath = sourceDir.filePath(subDirName);
            QString destSubDirPath = destinationDir.filePath(subDirName);
            if (!moveDirectory(srcSubDirPath, destSubDirPath)) {
                return false;
            }
        }

    return sourceDir.removeRecursively();
}

void MainWindow::handleMoveOperation(QString arguments){
    QStringList paths = arguments.split(" ");
    if (paths.size() != 2) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid number of arguments for mv."));
        return;
    }

    QString sourcePath = paths[0];
    QString destinationPath = paths[1];

    QString currentPath = ui->label->text();
    QDir currentDir(currentPath);

    QString fullSourcePath = QDir::isAbsolutePath(sourcePath)
                             ? QDir::cleanPath(sourcePath)
                             : QDir::cleanPath(currentPath + QDir::separator() + sourcePath);
    QString fullDestinationPath = QDir::isAbsolutePath(destinationPath)
                                  ? QDir::cleanPath(destinationPath)
                                  : QDir::cleanPath(currentPath + QDir::separator() + destinationPath);

    QFileInfo sourceInfo(fullSourcePath);
    QFileInfo destInfo(fullDestinationPath);

    if (!sourceInfo.exists()) {
        QMessageBox::warning(this, tr("Error"), tr("Source file or directory does not exist."));
        return;
    }

    if (!destInfo.exists()) {
        QMessageBox::warning(this, tr("Error"), tr("Destination directory does not exist."));
        return;
    }

    // If destination is a directory, append the source file name to it
    if (destInfo.isDir()) {
        fullDestinationPath = QDir(fullDestinationPath).filePath(sourceInfo.fileName());
    }

    if (sourceInfo.isDir()) {
        if (!moveDirectory(fullSourcePath, fullDestinationPath)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to move the directory."));
            return;
        }
    } else {
        if (!QFile::rename(fullSourcePath, fullDestinationPath)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to move the file."));
            return;
        }
    }
    statusBar()->showMessage(tr("Moved '%1' to '%2'").arg(fullSourcePath, fullDestinationPath));
}


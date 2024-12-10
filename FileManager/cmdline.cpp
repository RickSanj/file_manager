#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::handleCopyOperation(const QString &arguments) {
    QStringList paths = arguments.split(" ");
    if (paths.size() != 2) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid number of arguments for cp."));
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
    if (!sourceInfo.exists()) {
        QMessageBox::warning(this, tr("Error"), tr("Source file does not exist."));
        return;
    }

    if (!QFile::copy(fullSourcePath, fullDestinationPath)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to copy the file."));
    } else {
        statusBar()->showMessage(tr("Copied '%1' to '%2'").arg(fullSourcePath, fullDestinationPath));
    }
}


void MainWindow::processCommandLine(const QString &input) {
    if (input.isEmpty()) {
        navigateToHome();
        return;
    }

    QStringList tokens = input.split(" ", Qt::SkipEmptyParts);
    if (tokens.isEmpty()) {
        navigateToHome();
        return;
    }

    QString command = tokens[0];
    QString arguments = input.mid(command.length()).trimmed();

    if (command == "cd") {
        changeDirectory(arguments);
    } else if (command == "mkdir") {
        createDirectory(arguments);
    } else if (command == "rm") {
        removeFileOrDirectory(arguments);
    } else if (command == "cp") {
        handleCopyOperation(arguments);
    } else if (command == "mv") {
        handleMoveOperation(arguments);
    } else {
        changeDirectory(input);
    }

    ui->lineEdit->clear();
}


void MainWindow::navigateToHome() {
    QString homePath = QDir::homePath();
    ui->treeViewLeft->setRootIndex(modelLeft->index(homePath));
    ui->label->setText(homePath);
}

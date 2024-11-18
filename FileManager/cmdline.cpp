#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::processCommandLine(const QString &input) {
    if (input.isEmpty()) {
        navigateToHome();
    } else if (input.startsWith("cd ")) {
        changeDirectory(input.mid(3).trimmed());
    } else if (input.startsWith("mkdir ")) {
        createDirectory(input.mid(6).trimmed());
    } else if (input.startsWith("rm ")) {
        removeFileOrDirectory(input.mid(3).trimmed());
    }
    else {
        QString command = input.split(" ")[0];
        QString arguments = input.mid(command.length()).trimmed();

        if (command == "cp") {
            QStringList paths = arguments.split(" ");
            if (paths.size() != 2) {
                QMessageBox::warning(this, tr("Error"), tr("Invalid number of arguments for cp."));
                return;
            }

            QString sourcePath = paths[0];
            QString destinationPath = paths[1];

            if (!QFile::copy(sourcePath, destinationPath)) {
                QMessageBox::warning(this, tr("Paste"), tr("Failed to copy the file."));
                return;
            }
        }
        else if (command == "mv") {
            QStringList paths = arguments.split(" ");
            if (paths.size() != 2) {
                QMessageBox::warning(this, tr("Error"), tr("Invalid number of arguments for mv."));
                return;
            }

            QString sourcePath = paths[0];
            QString destinationPath = paths[1];
            QFileInfo sourceInfo(sourcePath);
            if (!QFile::exists(sourcePath)) {
                QMessageBox::warning(this, tr("Error"), tr("Source file or directory does not exist."));
                return;
            }
            if (sourceInfo.isDir() || sourceInfo.isFile()) {
                if (!QFile::rename(sourcePath, destinationPath)) {
                    QMessageBox::warning(this, tr("Error"), tr("Failed to move the file or directory."));
                    return;
                }
            }
        }
        else{
            changeDirectory(input);
        }
    }
    ui->lineEdit->clear();
}

void MainWindow::navigateToHome() {
    QString homePath = QDir::homePath();
    ui->treeViewLeft->setRootIndex(modelLeft->index(homePath));
    ui->label->setText(homePath);
}

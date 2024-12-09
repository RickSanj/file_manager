#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::handleExtraction() {
    selectedRowsBuffer = getSelectedFilePaths();
    if (selectedRowsBuffer.isEmpty()) {
        QMessageBox::information(this, tr("Extract"), tr("No files selected."));
        return;
    }

    if (selectedRowsBuffer.size() > 1) {
        QMessageBox::warning(this, tr("Extract"), tr("Please select only one file at a time."));
        return;
    }

    QString zipFilePath = selectedRowsBuffer.first();
    if (!zipFilePath.endsWith(".zip", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, tr("Extract"), tr("Selected file is not a ZIP archive:\n%1").arg(zipFilePath));
        return;
    }

    // Extract directory: create a folder with the same name as the ZIP file
    QFileInfo zipFileInfo(zipFilePath);
    QString baseDir = zipFileInfo.absolutePath();
    QString extractDir = QDir(baseDir).filePath(zipFileInfo.baseName());

    if (!QDir().exists(extractDir)) {
        if (!QDir().mkpath(extractDir)) {
            QMessageBox::warning(this, tr("Extract"), tr("Failed to create extraction directory:\n%1").arg(extractDir));
            return;
        }
    }

    QuaZip zip(zipFilePath);
    if (!zip.open(QuaZip::mdUnzip)) {
        QMessageBox::warning(this, tr("Extract"), tr("Failed to open archive:\n%1").arg(zipFilePath));
        return;
    }

    QuaZipFile zipFile(&zip);
    for (bool moreFiles = zip.goToFirstFile(); moreFiles; moreFiles = zip.goToNextFile()) {
        QuaZipFileInfo fileInfo;
        if (!zip.getCurrentFileInfo(&fileInfo)) {
            QMessageBox::warning(this, tr("Extract"), tr("Failed to read file info from archive."));
            continue;
        }

        QString outputPath = QDir(extractDir).filePath(fileInfo.name);

        if (fileInfo.name.endsWith('/')) {
            if (!QDir().mkpath(outputPath)) {
                QMessageBox::warning(this, tr("Extract"), tr("Failed to create directory:\n%1").arg(outputPath));
            }
        } else {
            QFileInfo outputFileInfo(outputPath);
            if (!outputFileInfo.dir().exists()) {
                if (!QDir().mkpath(outputFileInfo.dir().path())) {
                    QMessageBox::warning(this, tr("Extract"), tr("Failed to create parent directory:\n%1").arg(outputFileInfo.dir().path()));
                    continue;
                }
            }

            if (!zipFile.open(QIODevice::ReadOnly)) {
                QMessageBox::warning(this, tr("Extract"), tr("Failed to open file in archive:\n%1").arg(fileInfo.name));
                continue;
            }

            QFile outputFile(outputPath);
            if (!outputFile.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this, tr("Extract"), tr("Failed to create output file:\n%1").arg(outputPath));
                zipFile.close();
                continue;
            }

            outputFile.write(zipFile.readAll());
            zipFile.close();
            outputFile.close();
        }
    }
    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        QMessageBox::warning(this, tr("Extract"), tr("An error occurred while extracting:\n%1").arg(zipFilePath));
    }
}


void MainWindow::handleCompression() {
    selectedRowsBuffer = getSelectedFilePaths();
    if (selectedRowsBuffer.isEmpty()) {
        QMessageBox::information(this, tr("Compress"), tr("No files or directories selected."));
        return;
    }

    QString zipFilePath = QFileDialog::getSaveFileName(this, tr("Save Archive As"), QString(), tr("ZIP Archive (*.zip)"));
    if (zipFilePath.isEmpty()) {
        return;
    }

    if (!zipFilePath.endsWith(".zip", Qt::CaseInsensitive)) {
        zipFilePath += ".zip";
    }

    QuaZip zip(zipFilePath);
    qDebug() << "zipFilePath: " + zipFilePath;
    if (!zip.open(QuaZip::mdCreate)) {
        QMessageBox::warning(this, tr("Compress"), tr("Failed to create archive:\n%1").arg(zipFilePath));
        return;
    }

    QuaZipFile outFile(&zip);
    for (const QString &path : selectedRowsBuffer) {
        QFileInfo fileInfo(path);
        if (fileInfo.isDir()) {
            QString rootPath = fileInfo.absoluteFilePath();
            if (!addDirectoryToZip(path, &outFile, rootPath)) {
                QMessageBox::warning(this, tr("Compress"), tr("Failed to add directory to archive:\n%1").arg(path));
            }
        } else if (fileInfo.isFile()) {
            QString rootPath = QFileInfo(path).absolutePath();
            QString relativePath = QDir(rootPath).relativeFilePath(path);

            if (!addFileToZip(path, outFile, relativePath)) {
                QMessageBox::warning(this, tr("Compress"), tr("Failed to add file to archive:\n%1").arg(path));
            }
        }
    }

    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        QMessageBox::warning(this, tr("Compress"), tr("An error occurred during compression:\n%1").arg(zipFilePath));
    }
}


QString clearParentDirectories(const QString &inputPath) {
    QStringList pathComponents = inputPath.split('/', SkipEmptyParts);
    QStringList cleanPath;

    for (const QString &component : pathComponents) {
        if (component == "..") {
            if (!cleanPath.isEmpty()) {
                cleanPath.removeLast();
            }
        } else {
            cleanPath.append(component);
        }
    }
    return cleanPath.join('/');
}


bool MainWindow::addFileToZip(const QString &filePath, QuaZipFile &outFile, const QString &relativePath) {
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QString archivePath = clearParentDirectories(relativePath);

    QuaZipNewInfo newInfo(archivePath, filePath);
    if (!outFile.open(QIODevice::WriteOnly, newInfo)) {
        inputFile.close();
        return false;
    }

    outFile.write(inputFile.readAll());
    outFile.close();
    inputFile.close();
    return outFile.getZipError() == UNZ_OK;
}


bool MainWindow::addDirectoryToZip(const QString &dirPath, QuaZipFile *outFile, const QString &rootPath) {
    QDir dir(dirPath);
    QStringList entryList = dir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDir::Name);

    for (const QString &entry : entryList) {
        QString fullPath = dir.filePath(entry);
        QFileInfo entryInfo(fullPath);

        if (entryInfo.isDir()) {
            if (!this->addDirectoryToZip(fullPath, outFile, rootPath)) {
                return false;
            }
        } else if (entryInfo.isFile()) {
            QString relativePath = QDir(rootPath).relativeFilePath(fullPath);
            if (!this->addFileToZip(fullPath, *outFile, relativePath)) {
                return false;
            }
        }
    }

    return true;
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::handleExtraction() {
    selectedRowsBuffer = getSelectedFilePaths();
    if (selectedRowsBuffer.isEmpty()) {
        QMessageBox::information(this, tr("Extract"), tr("No files selected."));
        return;
    }

    if (selectedRowsBuffer.size() > 1) {
        QMessageBox::information(this, tr("Extract"), tr("Please select only one file at a time."));
        return;
    }

    QString zipFilePath = selectedRowsBuffer.first();
    if (!zipFilePath.endsWith(".zip", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, tr("Extract"), tr("Selected file is not a ZIP archive:\n%1").arg(zipFilePath));
        return;
    }

    QString extractDir = QFileDialog::getExistingDirectory(this, tr("Select Extraction Directory"));
    if (extractDir.isEmpty()) {
        return;
    }

    QuaZip zip(zipFilePath);
    if (!zip.open(QuaZip::mdUnzip)) {
        QMessageBox::warning(this, tr("Extract"), tr("Failed to open archive:\n%1").arg(zipFilePath));
        return;
    }

    QuaZipFile zipFile(&zip);
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        QuaZipFileInfo fileInfo;
        if (!zip.getCurrentFileInfo(&fileInfo)) {
            QMessageBox::warning(this, tr("Extract"), tr("Failed to read file info from archive."));
            continue;
        }

        QString outputPath = QDir(extractDir).filePath(fileInfo.name);

        if (fileInfo.name.endsWith('/')) {
            QDir().mkpath(outputPath);
        } else {
            if (!zipFile.open(QIODevice::ReadOnly)) {
                QMessageBox::warning(this, tr("Extract"), tr("Failed to extract file:\n%1").arg(fileInfo.name));
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
    if (!zip.open(QuaZip::mdCreate)) {
        QMessageBox::warning(this, tr("Compress"), tr("Failed to create archive:\n%1").arg(zipFilePath));
        return;
    }

    QuaZipFile outFile(&zip);
    for (const QString &path : selectedRowsBuffer) {
        QFileInfo fileInfo(path);
        if (fileInfo.isDir()) {
            QDir dir(path);
            QStringList fileList = dir.entryList(QDir::Files | QDir::NoSymLinks, QDir::Name);
            for (const QString &file : fileList) {
                QString fullPath = dir.filePath(file);
                if (!addFileToZip(fullPath, zip, outFile, path)) {
                    QMessageBox::warning(this, tr("Compress"), tr("Failed to add file to archive:\n%1").arg(fullPath));
                }
            }
        } else if (fileInfo.isFile()) {
            if (!addFileToZip(path, zip, outFile)) {
                QMessageBox::warning(this, tr("Compress"), tr("Failed to add file to archive:\n%1").arg(path));
            }
        }
    }

    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        QMessageBox::warning(this, tr("Compress"), tr("An error occurred during compression:\n%1").arg(zipFilePath));
    } else {
        QMessageBox::information(this, tr("Compress"), tr("Files successfully compressed into:\n%1").arg(zipFilePath));
    }
}


bool MainWindow::addFileToZip(const QString &filePath, QuaZip &zip, QuaZipFile &outFile, const QString &basePath) {
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QString archivePath = basePath.isEmpty() ? QFileInfo(filePath).fileName()
                                             : QDir(basePath).relativeFilePath(filePath);

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

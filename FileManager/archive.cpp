#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <quazip.h>
#include <quazipfile.h>

void testQuaZip() {
    QuaZip zip("test.zip");  // Open an existing zip file
    if (!zip.open(QuaZip::mdUnzip)) {
        qDebug() << "Error opening zip file!";
        return;
    }

    QuaZipFile file(&zip);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file in zip!";
        return;
    }

    qDebug() << "Successfully opened zip file!";
}

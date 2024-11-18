#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::handleTreeViewDoubleClicked(const QModelIndex &index) {
    QTreeView *senderTreeView = qobject_cast<QTreeView *>(sender());
    QFileSystemModel *model = nullptr;

    if (senderTreeView == ui->treeViewLeft) {
        model = modelLeft;
    } else if (senderTreeView == ui->treeViewRight) {
        model = modelRight;
    }

    if (model) {
        QString path = model->filePath(index);
        QFileInfo fileInfo(path);

        if (fileInfo.isDir()) {
            senderTreeView->setRootIndex(model->index(path));
            ui->label->setText(path);
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }

    selectedRowsBuffer.clear();
}
void MainWindow::onEnterPressed() {
    if (ui->lineEdit->hasFocus()) {
        processCommandLine(ui->lineEdit->text().trimmed());
        return;
    }
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;

    if (ui->treeViewLeft->hasFocus()) {
        activeTreeView = ui->treeViewLeft;
        activeModel = modelLeft;
    } else if (ui->treeViewRight->hasFocus()) {
        activeTreeView = ui->treeViewRight;
        activeModel = modelRight;
    }

    if (activeTreeView && activeModel) {
        handleFileManagerAction(activeTreeView, activeModel);
    }

}


void MainWindow::onEscPressed() {
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
        QMessageBox::warning(this, tr("Esc Key Pressed"), tr("No panel is active."));
        return;
    }

    QModelIndex currentRootIndex = activeTreeView->rootIndex();
    QModelIndex parentIndex = currentRootIndex.parent();

    if (parentIndex.isValid()) {
        QFileInfo fileInfo = activeModel->fileInfo(parentIndex);

        if (fileInfo.isDir()) {
            activeTreeView->setRootIndex(activeModel->index(fileInfo.absoluteFilePath()));
            ui->lineEdit->setText(fileInfo.absoluteFilePath());
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;

    if (ui->treeViewLeft->hasFocus()) {
        activeTreeView = ui->treeViewLeft;
        activeModel = modelLeft;
    } else if (ui->treeViewRight->hasFocus()) {
        activeTreeView = ui->treeViewRight;
        activeModel = modelRight;
    }

    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QItemSelectionModel *selectionModel = activeTreeView->selectionModel();
    if (selectionModel->selectedIndexes().isEmpty()) {
        selectedRowsBuffer.clear();
    }
}


QStringList MainWindow::getSelectedFilePaths() {
    QStringList filePaths;
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
        QItemSelectionModel *selectionModel = activeTreeView->selectionModel();
        QModelIndexList selectedIndexes = selectionModel->selectedRows();

        for (const QModelIndex &index : selectedIndexes) {

            QString filePath = activeModel->filePath(index);
            filePaths.append(filePath);
        }
    }
    return filePaths;
}

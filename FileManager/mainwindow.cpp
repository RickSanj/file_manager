#include "mainwindow.h"
#include "./ui_mainwindow.h"

QTranslator translator;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeViewLeft->installEventFilter(this);
    ui->treeViewLeft->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->treeViewRight->installEventFilter(this);
    ui->treeViewRight->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->langButton->setText("ENG");
    ui->treeViewLeft->installEventFilter(this);
    treeViewLeft = ui->treeViewLeft;
    treeViewRight = ui->treeViewRight;


    modelLeft = new QFileSystemModel(this);
    modelLeft->setRootPath(QDir::rootPath());
    treeViewLeft->setModel(modelLeft);
    treeViewLeft->setRootIndex(modelLeft->index(QDir::homePath()));

    modelRight = new QFileSystemModel(this);
    modelRight->setRootPath(QDir::rootPath());
    treeViewRight->setModel(modelRight);
    treeViewRight->setRootIndex(modelRight->index(QDir::homePath()));


    connect(treeViewLeft, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(treeViewRight, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);

    ui->treeViewLeft->setItemsExpandable(false);
    ui->treeViewRight->setItemsExpandable(false);
    ui->treeViewLeft->setRootIsDecorated(false);
    ui->treeViewRight->setRootIsDecorated(false);
    ui->lineEdit->setText(QDir::rootPath());

    connect(ui->treeViewLeft, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(ui->treeViewLeft->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);


    connect(ui->treeViewRight, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(ui->treeViewRight->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
    // next maybe remove later
    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
    treeViewLeft->setSortingEnabled(true);
    treeViewRight->setSortingEnabled(true);

    treeViewLeft->sortByColumn(0, Qt::AscendingOrder);
    treeViewRight->sortByColumn(0, Qt::AscendingOrder);
    treeViewLeft->header()->setSectionsClickable(true);
    treeViewRight->header()->setSectionsClickable(true);
    treeViewLeft->header()->setSortIndicatorShown(true);
    treeViewRight->header()->setSortIndicatorShown(true);

}


void MainWindow::on_pushButton_clicked(){
    QString path = ui->lineEdit->text();
    if (path==""){
        ui->treeViewLeft->setRootIndex(modelLeft->index(QDir::homePath()));
        ui->lineEdit->setText(QDir::homePath());
    } else{
        ui->treeViewLeft->setRootIndex(modelLeft->index(path));
    }
}

void MainWindow::handleSelectPathButtonClicked(){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::homePath());
    if (!folderPath.isEmpty()) {
        modelLeft->setRootPath(folderPath);
        ui->treeViewLeft->setModel(modelLeft);
        ui->treeViewLeft->setRootIndex(modelLeft->index(folderPath));
        ui->lineEdit->setText(QDir::homePath());
    }
}

void MainWindow::on_lineEdit_returnPressed(){
    QString path = ui->lineEdit->text();
    if (path==""){
        ui->treeViewLeft->setRootIndex(modelLeft->index(QDir::homePath()));
        ui->lineEdit->setText(QDir::homePath());
    } else{
        ui->treeViewLeft->setRootIndex(modelLeft->index(path));
    }
}


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
            ui->lineEdit->setText(path);
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }

    selectedRowsBuffer.clear();
}


void MainWindow::showProperties() {
    // Determine which tree view is active
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;

    // Check which tree view has focus
    if (ui->treeViewLeft->hasFocus()) {
        activeTreeView = ui->treeViewLeft;
        activeModel = modelLeft;
    } else if (ui->treeViewRight->hasFocus()) {
        activeTreeView = ui->treeViewRight;
        activeModel = modelRight;
    }

    // If no tree view is active, return
    if (!activeTreeView || !activeModel) {
        QMessageBox::warning(this, tr("Properties"), tr("No panel is active."));
        return;
    }

    // Get the selected index in the active tree view
    QModelIndex index = activeTreeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Properties"), tr("No file or directory selected."));
        return;
    }

    // Get file information for the selected item
    QString filePath = activeModel->filePath(index);
    QFileInfo fileInfo(filePath);

    // Create the dialog to display properties
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Properties"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // Add property details to the layout
    layout->addWidget(new QLabel(tr("Name: ") + fileInfo.fileName()));
    layout->addWidget(new QLabel(tr("Path: ") + fileInfo.absoluteFilePath()));
    layout->addWidget(new QLabel(tr("Size: ") + QString::number(fileInfo.size()) + tr(" bytes")));
    layout->addWidget(new QLabel(QString(tr("Type: ")) + (fileInfo.isDir() ? tr("Folder") : tr("File"))));
    layout->addWidget(new QLabel(tr("Last Modified: ") + fileInfo.lastModified().toString()));

    dialog->setLayout(layout);
    dialog->exec();
}


void MainWindow::handleCustomContextMenuRequested(const QPoint &pos) {
    // Determine which tree view triggered the context menu
    QTreeView *treeView = qobject_cast<QTreeView *>(sender());
    if (!treeView) return;

    // Determine the model associated with the tree view
    QFileSystemModel *model = nullptr;
    if (treeView == ui->treeViewLeft) {
        model = modelLeft;
    } else if (treeView == ui->treeViewRight) {
        model = modelRight;
    }
    if (!model) return;

    // Get the file or directory at the context menu position
    QModelIndex index = treeView->indexAt(pos);
    QFileInfo fileInfo = model->fileInfo(index);

    // Create the context menu
    QMenu contextMenu(this);

    // Add "Open" action for files
    if (fileInfo.isFile()) {
        QAction *openAction = contextMenu.addAction(tr("Open"));
        connect(openAction, &QAction::triggered, this, &MainWindow::handleOpenActionTriggered);
    }

    QAction *cutAction = contextMenu.addAction(tr("Cut"));
    QAction *copyAction = contextMenu.addAction(tr("Copy"));
    QAction *pasteAction = contextMenu.addAction(tr("Paste"));
    QAction *deleteAction = contextMenu.addAction(tr("Delete"));
    QAction *renameAction = contextMenu.addAction(tr("Rename"));
    QAction *propertiesAction = contextMenu.addAction(tr("Properties"));


    connect(cutAction, &QAction::triggered, this, &MainWindow::handleCutTriggered);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);
    connect(renameAction, &QAction::triggered, this, &MainWindow::handleRenameTriggered);
    connect(copyAction, &QAction::triggered, this, &MainWindow::handleCopyTriggered);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::handlePasteTriggered);
    connect(propertiesAction, &QAction::triggered, this, &MainWindow::showProperties);

    if (copyPath.isEmpty() && !cutAction){
        pasteAction->setDisabled(true);
    }

    // Disable invalid actions if no valid index is selected
    if (!index.isValid()) {
        copyAction->setDisabled(true);
        deleteAction->setDisabled(true);
        renameAction->setDisabled(true);
    }

    // Show the context menu
    contextMenu.exec(treeView->viewport()->mapToGlobal(pos));
}



void MainWindow::handleCopyTriggered() {
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;

    // Determine the active tree view and model
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

    QModelIndex selectedIndex = activeTreeView->currentIndex();

    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, tr("Copy"), tr("No file or directory selected."));
        return;
    }

    QString sourcePath = activeModel->filePath(selectedIndex);
    QFileInfo fileInfo(sourcePath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(this, tr("Copy"), tr("The selected file or directory does not exist."));
        return;
    }

    // Update buffer with the selected file path
    selectedRowsBuffer = QStringList{sourcePath};
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
    QString destinationPath = activeModel->filePath(currentIndex);
    QFileInfo destInfo(destinationPath);

    if (!destInfo.isDir()) {
        destinationPath = activeModel->filePath(activeTreeView->rootIndex());
    }
    for(const QString &path : selectedRowsBuffer){
        QFileInfo sourceInfo(path);

        if (!QFile::exists(path)) {
            QMessageBox::warning(this, "Error", "Source file does not exist.");
            return;
        }
        if (path.isEmpty()) {
            QMessageBox::warning(this, "Paste", "No source file or directory to paste.");
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

void MainWindow::handleDeleteTriggered(){

    for(auto path : selectedRowsBuffer){
        QFileInfo fileInfo(path);
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (fileInfo.isFile()) {
                if (!QFile::remove(path)) {
                    QMessageBox::warning(this, "Delete", "Failed to delete file.");
                }
            } else if (fileInfo.isDir()) {
                QDir dir(path);
                if (!dir.removeRecursively()) {
                    QMessageBox::warning(this, "Delete", "Failed to delete directory.");
                }
            }
        }
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


void MainWindow::handleOpenActionTriggered() {
    QModelIndex index = currentIndex;
    if (!index.isValid()) {
        QMessageBox::warning(this, "Open", "No file or directory selected.");
        return;
    }
    handleTreeViewDoubleClicked(index);
}
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

void MainWindow::pasteFile(QString sourcePath, QString destinationPath){
    QFileInfo sourceInfo(sourcePath);
    QString newFilePath = destinationPath + "/" + sourceInfo.fileName();

    QFileInfo destInfo(newFilePath);

    if (isCutOperation) {
        if (!QFile::rename(sourcePath, newFilePath)) {
            QMessageBox::warning(this, "Paste", "Failed to move the file.");
            return;
        }
    } else {
        if (!QFile::copy(sourcePath, newFilePath)) {
            QMessageBox::warning(this, "Paste", "Failed to copy the file.");
            return;
        }
    }
};

void MainWindow::pasteDir(QString sourcePath, QString destinationPath){
    QDir sourceDir(sourcePath);
    QDir targetDir(destinationPath);

    QString targetPathWithSourceDir = targetDir.filePath(sourceDir.dirName());
    QDir newTargetDir(targetPathWithSourceDir);

    if (isCutOperation) {
        if (!sourceDir.rename(sourceDir.path(), targetPathWithSourceDir)) {
            QMessageBox::warning(this, "Paste", "Failed to move the directory.");
            return;
        }
    } else {
        if (!copyDirectory(sourceDir, newTargetDir)) {
            QMessageBox::warning(this, "Paste", "Failed to copy the directory.");
            return;
        }
    }
};

void MainWindow::checkExistance(QString path){
    QFileInfo sourceInfo(path);

    if (QFile::exists(path)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Overwrite File", "The file or directory already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }

        if (sourceInfo.isFile()) {
            QFile::remove(path);
        } else if (sourceInfo.isDir()) {
            QDir(path).removeRecursively();
        }
    }
}


bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    QTreeView *activeTreeView = nullptr;
    QFileSystemModel *activeModel = nullptr;

    if (ui->treeViewLeft->hasFocus()) {
        activeTreeView = ui->treeViewLeft;
        activeModel = modelLeft;
    } else if (ui->treeViewRight->hasFocus()) {
        activeTreeView = ui->treeViewRight;
        activeModel = modelRight;
    }

    if ((object == treeViewLeft || object == treeViewRight) && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);


        if (keyEvent->modifiers() == Qt::ShiftModifier &&
            (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)) {

            QModelIndex currentIndex = activeTreeView->currentIndex();
            QModelIndex nextIndex;

            if (keyEvent->key() == Qt::Key_Up) {
                nextIndex = activeModel->index(currentIndex.row() - 1, currentIndex.column(), currentIndex.parent());
            } else if (keyEvent->key() == Qt::Key_Down) {
                nextIndex = activeModel->index(currentIndex.row() + 1, currentIndex.column(), currentIndex.parent());
            }

            if (nextIndex.isValid()) {
                activeTreeView->selectionModel()->select(
                    QItemSelection(currentIndex, nextIndex),
                    QItemSelectionModel::Select
                    );
                activeTreeView->setCurrentIndex(nextIndex);
            }
            return true;
        }


        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            onEnterPressed();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Escape) {
            onEscPressed();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Delete) {
            handleDeleteTriggered();
            return true;
        }
        if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ControlModifier) {
            handleCopyTriggered();
            return true;
        }
        if (keyEvent->key() == Qt::Key_V && keyEvent->modifiers() == Qt::ControlModifier) {
            handlePasteTriggered();
            return true;
        }

        if (keyEvent->key() == Qt::Key_X && keyEvent->modifiers() == Qt::ControlModifier) {
            handleCutTriggered();
            return true;
        }

    }
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::onEnterPressed() {
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
        QMessageBox::warning(this, tr("Enter Key Pressed"), tr("No panel is active."));
        return;
    }

    QModelIndex selectedIndex = activeTreeView->currentIndex();
    if (selectedIndex.isValid()) {
        QFileInfo fileInfo = activeModel->fileInfo(selectedIndex);

        if (fileInfo.isDir()) {
            activeTreeView->setRootIndex(activeModel->index(fileInfo.absoluteFilePath()));
            ui->lineEdit->setText(fileInfo.absoluteFilePath());
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
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


    QItemSelectionModel *selectionModel = activeTreeView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedRows();

    for (const QModelIndex &index : selectedIndexes) {
        QString filePath = activeModel->filePath(index);
        filePaths.append(filePath);
    }
    return filePaths;
}

void MainWindow::on_langButton_clicked(){

    QString currentText = ui->langButton->text();

    if (currentText == "ENG") {
       // qDebug() << "Current working directory:" << QDir::currentPath();
        if (translator.load("../../translation_ukr.qm")) {
            qApp->installTranslator(&translator);
            ui->retranslateUi(this);
        }
        else{
            qDebug() << "Failed to load translation file.";
        }
        ui->langButton->setText("УКР");

    }
    else {

        qApp->removeTranslator(&translator);
        ui->retranslateUi(this);
        ui->langButton->setText("ENG");

    }

}

MainWindow::~MainWindow(){
    delete ui;
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"

QTranslator translator;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
    // next maybe remove later
    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
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

    // Add other actions
    QAction *copyAction = contextMenu.addAction(tr("Copy"));
    QAction *pasteAction = contextMenu.addAction(tr("Paste"));
    QAction *deleteAction = contextMenu.addAction(tr("Delete"));
    QAction *renameAction = contextMenu.addAction(tr("Rename"));
    QAction *propertiesAction = contextMenu.addAction(tr("Properties"));

    connect(deleteAction, &QAction::triggered, this, &MainWindow::handleDeleteTriggered);
    connect(renameAction, &QAction::triggered, this, &MainWindow::renameItem);
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopyTriggered);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPasteTriggered);
    connect(propertiesAction, &QAction::triggered, this, &MainWindow::showProperties);

    // Disable "Paste" if no path has been copied
    if (copyPath.isEmpty()) {
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


void MainWindow::renameItem(){
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


void MainWindow::handleDeleteTriggered() {
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
        QMessageBox::warning(this, tr("Delete"), tr("No panel is active."));
        return;
    }

    QModelIndex selectedIndex = activeTreeView->currentIndex();
    QString path = activeModel->filePath(selectedIndex);

    if (!selectedIndex.isValid() || path.isEmpty()) {
        QMessageBox::warning(this, tr("Delete"), tr("No file or directory selected."));
        return;
    }

    QFileInfo fileInfo(path);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete"), tr("Are you sure you want to delete this?"),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (fileInfo.isFile()) {
            if (!QFile::remove(path)) {
                QMessageBox::warning(this, tr("Delete"), tr("Failed to delete file."));
            }
        } else if (fileInfo.isDir()) {
            QDir dir(path);
            if (!dir.removeRecursively()) {
                QMessageBox::warning(this, tr("Delete"), tr("Failed to delete directory."));
            }
        }
    }
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

    QString path = activeModel->filePath(index);
    QFileInfo fileInfo(path);

    if (fileInfo.isDir()) {
        activeTreeView->setRootIndex(activeModel->index(path));
        ui->lineEdit->setText(path);
    } else if (fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}


void MainWindow::onCopyTriggered() {
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

    copyPath = sourcePath;
}



void MainWindow::onPasteTriggered() {
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

    if (!QFileInfo(destinationPath).isDir()) {
        destinationPath = activeModel->filePath(activeTreeView->rootIndex());
    }

    QFileInfo sourceInfo(copyPath);
    QString newFilePath = destinationPath + "/" + sourceInfo.fileName();

    if (QFile::exists(newFilePath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Overwrite File"),
            tr("The file already exists. Do you want to overwrite it?"),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    if (sourceInfo.isFile()) {
        if (!QFile::copy(copyPath, newFilePath)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to paste the file."));
        }
    }
    else if (sourceInfo.isDir()) {
        QDir sourceDir(copyPath);
        QDir targetDir(destinationPath);

        QString targetPathWithSourceDir = targetDir.filePath(sourceDir.dirName());
        QDir newTargetDir(targetPathWithSourceDir);

        if (!copyDirectory(sourceDir, newTargetDir)) {
            QMessageBox::warning(this, tr("Paste"), tr("Failed to paste the directory."));
        }
    }
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

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if ((object == treeViewLeft || object == treeViewRight) && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);


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
            onCopyTriggered();
            return true;
        }
        if (keyEvent->key() == Qt::Key_V && keyEvent->modifiers() == Qt::ControlModifier) {
            onPasteTriggered();
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


MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_langButton_clicked()
{

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

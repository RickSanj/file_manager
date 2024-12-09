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

    treeViewLeft = ui->treeViewLeft;
    treeViewRight = ui->treeViewRight;

    modelLeft = new QFileSystemModel(this);
    modelLeft->setFilter(modelLeft->filter() & ~QDir::Filter::NoDotDot);
    modelLeft->setRootPath(QDir::rootPath());
    treeViewLeft->setModel(modelLeft);
    treeViewLeft->setRootIndex(modelLeft->index(QDir::homePath()));

    modelRight = new QFileSystemModel(this);
    modelRight->setFilter(modelRight->filter() & ~QDir::Filter::NoDotDot);
    modelRight->setRootPath(QDir::rootPath());

    treeViewRight->setModel(modelRight);
    treeViewRight->setRootIndex(modelRight->index(QDir::homePath()));


    connect(treeViewLeft, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(treeViewRight, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::onEnterPressed);

    ui->treeViewLeft->setItemsExpandable(false);
    ui->treeViewRight->setItemsExpandable(false);

    ui->treeViewLeft->setRootIsDecorated(false);
    ui->treeViewRight->setRootIsDecorated(false);

    ui->lineEdit->setText(QDir::rootPath());

    ui->label->setText(QDir::rootPath());

    connect(ui->treeViewLeft->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);


    connect(ui->treeViewRight, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(ui->treeViewRight->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);

    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);

    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);

    treeViewLeft->setSortingEnabled(true);
    treeViewRight->setSortingEnabled(true);

    treeViewLeft->setFocusPolicy(Qt::StrongFocus);
    treeViewRight->setFocusPolicy(Qt::StrongFocus);


    treeViewLeft->sortByColumn(0, Qt::AscendingOrder);
    treeViewRight->sortByColumn(0, Qt::AscendingOrder);

    treeViewLeft->header()->setSectionsClickable(true);
    treeViewRight->header()->setSectionsClickable(true);
    treeViewLeft->header()->setSortIndicatorShown(true);
    treeViewRight->header()->setSortIndicatorShown(true);
}


void MainWindow::checkExistance(QString path){
    QFileInfo sourceInfo(path);

    if (QFile::exists(path)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Overwrite File"), tr("The file or directory already exists. Do you want to overwrite it?"),
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

    if ((object == ui->treeViewLeft || object == ui->treeViewRight) && event->type() == QEvent::KeyPress) {
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

#ifdef Q_OS_MAC
        if (keyEvent->key() == Qt::Key_Backspace || keyEvent->key() == Qt::Key_Delete) {
#else
        if (keyEvent->key() == Qt::Key_Delete) {
#endif
            QString currentDirPath = activeModel->filePath(activeTreeView->currentIndex());

            QFileInfo currentDirInfo(currentDirPath);
            QString parentDirPath = currentDirInfo.absolutePath();

            handleDeleteTriggered();

            activeTreeView->setRootIndex(activeModel->index(parentDirPath));
            ui->lineEdit->setText(parentDirPath);

            return true;
        }

        // Handle Copy
#ifdef Q_OS_MAC
        if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::MetaModifier) {
#else
            if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ControlModifier) {
#endif
            handleCopyTriggered();
            return true;
        }

        // Handle Paste
#ifdef Q_OS_MAC
        if (keyEvent->key() == Qt::Key_V && keyEvent->modifiers() == Qt::MetaModifier) {
#else
            if (keyEvent->key() == Qt::Key_V && keyEvent->modifiers() == Qt::ControlModifier) {
#endif
            handlePasteTriggered();
            return true;
        }

        // Handle Cut
#ifdef Q_OS_MAC
        if (keyEvent->key() == Qt::Key_X && keyEvent->modifiers() == Qt::MetaModifier) {
#else
            if (keyEvent->key() == Qt::Key_X && keyEvent->modifiers() == Qt::ControlModifier) {
#endif
            handleCutTriggered();
            return true;
        }
    }
    return QMainWindow::eventFilter(object, event);
}





void MainWindow::handleFileManagerAction(QTreeView *activeTreeView, QFileSystemModel *activeModel) {
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


void MainWindow::on_langButton_clicked(){

    QString currentText = ui->langButton->text();

    if (currentText == "ENG") {
        if (translator.load("../../translation.qm")) {
            qApp->installTranslator(&translator);
            ui->retranslateUi(this);
        }
        else{
            qDebug() << "Failed to load translation file.";
        }
        ui->langButton->setText("УКР");
    } else {
        qApp->removeTranslator(&translator);
        ui->retranslateUi(this);
        ui->langButton->setText("ENG");
    }

}


MainWindow::~MainWindow(){
    delete ui;
}

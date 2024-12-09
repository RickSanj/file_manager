#include "mainwindow.h"
#include "./ui_mainwindow.h"

QTranslator translator;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize UI components
    setupTreeViews();
    setupModels();
    setupLayouts();
    setupConnections();
}

void MainWindow::setupTreeViews()
{
    ui->treeViewLeft->installEventFilter(this);
    ui->treeViewLeft->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewLeft->setItemsExpandable(false);
    ui->treeViewLeft->setRootIsDecorated(false);
    ui->treeViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeViewRight->installEventFilter(this);
    ui->treeViewRight->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewRight->setItemsExpandable(false);
    ui->treeViewRight->setRootIsDecorated(false);
    ui->treeViewRight->setContextMenuPolicy(Qt::CustomContextMenu);

    treeViewLeft = ui->treeViewLeft;
    treeViewRight = ui->treeViewRight;

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

void MainWindow::setupModels()
{
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
}

void MainWindow::setupLayouts()
{
    QPushButton *helpButton = new QPushButton("Help", this);
    QPushButton *langButton = new QPushButton("ENG", this);
    QLineEdit *lineEdit = new QLineEdit(QDir::rootPath(), this);
    QLabel *bottomLabel = new QLabel("Path:", this);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout();
    QHBoxLayout *labelLayout = new QHBoxLayout();
    QHBoxLayout *commandLineLayout = new QHBoxLayout();
    QSplitter *splitter = new QSplitter(this);

    splitter->addWidget(treeViewLeft);
    splitter->addWidget(treeViewRight);
    splitter->setOrientation(Qt::Horizontal);

    topLayout->addWidget(helpButton);
    topLayout->addWidget(langButton);
    labelLayout->addWidget(bottomLabel);
    commandLineLayout->addWidget(lineEdit);

    mainLayout->addLayout(topLayout, 1);          // 10%
    mainLayout->addWidget(splitter, 7);           // 70%
    mainLayout->addLayout(labelLayout, 1);        // 10%
    mainLayout->addLayout(commandLineLayout, 1);  // 10%

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    this->ui->lineEdit = lineEdit;
    this->ui->helpButton = helpButton;
    this->ui->langButton = langButton;
    ui->label = bottomLabel;
}

void MainWindow::setupConnections()
{
    connect(treeViewLeft, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);
    connect(treeViewRight, &QTreeView::doubleClicked, this, &MainWindow::handleTreeViewDoubleClicked);

    // Connect treeView selection changes
    connect(treeViewLeft->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(treeViewRight->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    // Connect treeView context menu events
    connect(treeViewLeft, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);
    connect(treeViewRight, &QTreeView::customContextMenuRequested, this, &MainWindow::handleCustomContextMenuRequested);

    // Connect button events
    connect(ui->helpButton, &QPushButton::clicked, this, &MainWindow::on_helpButton_clicked);
    connect(ui->langButton, &QPushButton::clicked, this, &MainWindow::on_langButton_clicked);

    // Connect lineEdit events
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::onEnterPressed);
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
            ui->label->setText(fileInfo.absoluteFilePath());
        } else if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }
}

void MainWindow::on_helpButton_clicked() {
    QDialog helpDialog(this);
    helpDialog.setWindowTitle(tr("Help"));

    QVBoxLayout *layout = new QVBoxLayout(&helpDialog);
    QLabel *helpLabel = new QLabel(tr("This file manager application allows you to:"
                                      "\n"
                                      "- Navigate directories by double-clicking on folders.\n"
                                      "- Open files with the default application by double-clicking on them.\n"
                                      "- Ctrl-c copy files or directories.\n"
                                      "- Ctrl-x cut files or directories.\n"
                                      "- Ctrl-v paste files or directories.\n"
                                      "- Extract ZIP archives by double-clicking on them.\n"
                                      "- Esc return to parent directory.\n"
                                      "- Del delete files or directories.\n"
                                      "- Extract ZIP archives by double-clicking on them.\n"
                                      "\n"
                                      "Command line:\n"
                                      "  cd <dir-path> Set current directory\n"
                                      "  cp <file_1> <file_2> Copy from file with <file_1> to <file_2>\n"
                                      "  mv <file_1> <file_2> Move file from <file_1> to <file_2>\n"
                                      "  rm <path> Remove file or directory\n"
                                      ), &helpDialog);
    layout->addWidget(helpLabel);

    QPushButton *okButton = new QPushButton(tr("OK"), &helpDialog);
    connect(okButton, &QPushButton::clicked, &helpDialog, &QDialog::accept);
    layout->addWidget(okButton);

    helpDialog.exec();
}

void MainWindow::on_langButton_clicked(){
    QString currentText = ui->langButton->text();

    if (currentText == "ENG") {
        ui->label->setText("Path:");
        if (translator.load("../../../ukrt.qm")) {
            qApp->installTranslator(&translator);
            ui->retranslateUi(this);
        }
        else{
            qDebug() << "Failed to load translation file.";
        }
        ui->langButton->setText("УКР");
    } else {
        ui->label->setText("Path:");
        qApp->removeTranslator(&translator);
        ui->retranslateUi(this);
        ui->langButton->setText("ENG");
    }
}


MainWindow::~MainWindow(){
    delete ui;
}

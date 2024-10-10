#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QTreeView>
#include <QScreen>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // Setup dirname
    directoryLabel = new QLabel(this);
    directoryLabel->setGeometry(30, 30, 300, 30);  // x, y, width, height
    directoryLabel->setText("Current Directory:");

    // Setup view tree
    this->TreeLayout = new QVBoxLayout;
    this->model = new QFileSystemModel(this);

    // Setup root path
    this->model->setRootPath(QDir::rootPath());

    // Create viewer of our system model
    // model - source of data, tree - viewer
    this -> tree = new QTreeView(this);
    this -> tree->setModel(model);

    // Setting up a root path
    this->tree->setRootIndex(model->index(QDir::rootPath()));

    // Animation of closing/opening of branches
    this -> tree->setAnimated(false);
    // Space from the left for next dir
    this -> tree->setIndentation(20);
    // Sort the tree
    this -> tree->setSortingEnabled(true);

    // Add the tree view to the layout
    this->TreeLayout->addWidget(directoryLabel);
    this->TreeLayout->addWidget(tree);

    // Set a central widget and layout
    this->centralTreeWidget = new QWidget(this);
    this->centralTreeWidget->setLayout(TreeLayout);
    this->setCentralWidget(centralTreeWidget);
}


void MainWindow::setCurrentPath(const QString &text){
    // Show name of current directory
    directoryLabel->setText("Current directory is: " + text);

    // Set path for model and tree
    model->setRootPath(text);
    tree->setRootIndex(model->index(text));
}


MainWindow::~MainWindow(){
    delete ui;
}

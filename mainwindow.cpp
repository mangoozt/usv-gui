#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QFileDialog"
#include <iostream>
#include "./usvdata/InputUtils.h"
#include "usvdata/CaseData.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open json file"), "", tr("Json Files (*.json)"));
    std::cout<<fileName.toStdString();
    auto dir_path = std::filesystem::path(fileName.toStdString()).parent_path();
    auto input_data = USV::InputUtils::loadInputData(dir_path);
    USV::CaseData caseData = USV::CaseData(input_data);
    OGLWidget *ogl_widget = findChild<OGLWidget*>("openGLWidget");
    ogl_widget->loadData(caseData);
}

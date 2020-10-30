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

void MainWindow::on_timeSlider_valueChanged(int value)
{
    OGLWidget *ogl_widget = findChild<OGLWidget*>("openGLWidget");
    QSlider *time_slider = findChild<QSlider*>("timeSlider");
    auto& case_data=ogl_widget->case_data;
    auto starttime=case_data.route.getStartTime();
    auto endtime=case_data.route.endTime();
    auto time = starttime+(endtime-starttime)*value/(time_slider->maximum()+1);
    std::vector<USV::Vessel> vessels;

    auto position = case_data.route.position(time);
    vessels.push_back({position.point,position.course.radians(),{0,1,0},case_data.vessels[0].radius});
    for(size_t i=0; i<case_data.targets_maneuvers.size();++i){
        auto position = case_data.targets_maneuvers[i].position(time);
        vessels.push_back({position.point,position.course.radians(),{0,0,1},case_data.vessels[i+1].radius});
    }
    ogl_widget->updatePositions(vessels);
    ogl_widget->update();
}

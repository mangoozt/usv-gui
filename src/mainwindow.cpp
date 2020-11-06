#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QFileDialog"
#include "usvdata/InputUtils.h"
#include "usvdata/CaseData.h"
#include <iostream>

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
    auto filePath = QFileDialog::getExistingDirectory(this, "Select Folder");
    auto input_data = USV::InputUtils::loadInputData(filePath.toStdString());
    USV::CaseData case_data = USV::CaseData(input_data);
    OGLWidget *ogl_widget = findChild<OGLWidget*>("openGLWidget");
    ogl_widget->loadData(case_data);
    QSlider *time_slider = findChild<QSlider*>("timeSlider");
    time_slider->setValue(time_slider->minimum());
    update_time(case_data.route.getStartTime(),ogl_widget);
}

namespace{
void push_position(double time, const USV::Path &path, std::vector<USV::Vessel> &vessels, USV::Color& color, double radius){
    try{
        auto position = path.position(time);
        vessels.push_back({position.point, position.course.radians(), radius, color});
    }catch(std::out_of_range){

    }
}
}
void MainWindow::update_time(double time, OGLWidget* ogl_widget){
    std::vector<USV::Vessel> vessels;
    auto& case_data=ogl_widget->case_data;
    USV::Color color{0,1,0};
    push_position(time, case_data.route, vessels, color, case_data.radius);

    color={0,0,1};
    for(size_t i=0; i<case_data.targets_maneuvers.size();++i)
        push_position(time, case_data.targets_maneuvers[i], vessels, color, case_data.radius);

    color={0.8,0.8,0.8};
    for(const auto &maneuver: case_data.maneuvers)
        push_position(time, maneuver, vessels, color, case_data.radius);

    ogl_widget->updatePositions(vessels);
    ogl_widget->update();
}

void MainWindow::on_timeSlider_valueChanged(int value)
{
    OGLWidget *ogl_widget = findChild<OGLWidget*>("openGLWidget");
    QSlider *time_slider = findChild<QSlider*>("timeSlider");
    auto& case_data=ogl_widget->case_data;
    auto starttime=case_data.route.getStartTime();
    auto endtime=case_data.route.endTime();
    auto time = starttime+(endtime-starttime)*value/(time_slider->maximum()+1);
    update_time(time, ogl_widget);
}

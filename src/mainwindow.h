#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_openButton_clicked();

    void on_timeSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    static void update_time(double time, OGLWidget* ogl_widget);
};
#endif // MAINWINDOW_H

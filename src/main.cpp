#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
//    QSurfaceFormat fmt;
//    fmt.setDepthBufferSize(24);
//    // Request OpenGL 3.3 core or OpenGL ES 3.0.
//    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
//        qDebug("Requesting 3.3 core context");
//        fmt.setVersion(3, 3);
//        fmt.setProfile(QSurfaceFormat::CoreProfile);
//    } else {
//        qDebug("Requesting 3.0 context");
//        fmt.setVersion(3, 0);
//    }

//    QSurfaceFormat::setDefaultFormat(fmt);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

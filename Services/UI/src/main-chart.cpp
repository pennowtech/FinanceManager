#include "cancom.hpp"
#include "chart.hpp"
#include <QtCore/QDebug>
#include <QtCore/QVersionNumber>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <iostream>

QT_USE_NAMESPACE

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qDebug() << "Qt version:" << qVersion();

    auto chartView = chart();

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();
    return app.exec();
}

/*----------------------------------------------------------------------
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ----------------------------------------------------------------------*/
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

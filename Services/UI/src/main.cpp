#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuickControls2/QQuickStyle>
#include <iostream>
// #include "DatabaseManager.h"
// #include "FinanceManager.h"
#include <Authentication.h>

int main(int argc, char* argv[])
{
    std::cout << "UI Service App\n";

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
    QQuickStyle::setFallbackStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "Light");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "#6200EE"); // Setting
    const auto applicationDirPath = QCoreApplication::applicationDirPath();
    QQmlApplicationEngine engine;
    engine.addImportPath(QString("%1/qml/").arg(applicationDirPath));
    engine.addImportPath("qrc:/");

    // DatabaseManager dbManager;
    // FinanceManager financeManager;
    // financeManager.setDatabaseManager(&dbManager);

    // engine.rootContext()->setContextProperty("databaseManager", &dbManager);
    // engine.rootContext()->setContextProperty("financeManager", &financeManager);
    Authentication auth;

    engine.rootContext()->setContextProperty("auth", &auth);

    engine.load(QUrl::fromLocalFile(QString("%1/qml/main.qml").arg(applicationDirPath)));
    if (engine.rootObjects().isEmpty())
    {
        // LOG_ERROR(appLogger, "Failed to load QML root objects");
        return -1;
    }

    return app.exec();
}

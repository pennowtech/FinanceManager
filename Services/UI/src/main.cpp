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

// #include "DatabaseManager.h"
// #include "FinanceManager.h"

#include <Utils/EnvReader.h>
#include <Utils/Logger.h>
#include <Utils/LoggerQuill.h>

#include <FirebaseAuthQt.h>
#include <RedisHandler.h>

#include <cxxopts.hpp>

#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuickControls2/QQuickStyle>
#include <iostream>

namespace
{
    struct CmdOptions
    {
        std::string envFileName{};
        int port{};
    };

    CmdOptions parseCommandLineOptions(std::shared_ptr<DCS::Utils::Logger>& logger, int argc, char* argv[])
    {
        CmdOptions cmdOptions;

        try
        {
            cxxopts::Options options(argv[0], " - API Gateway Command line options");
            options.add_options()("e,env", "Environment file name (probably .env in your root)", cxxopts::value<std::string>())(
                "p,port", "Port number", cxxopts::value<int>())("h,help", "Print usage");

            auto result = options.parse(argc, argv);

            if (result.count("help"))
            {
                DCS_LOG_ERROR(logger, options.help());
                exit(1);
            }

            cmdOptions.envFileName = result["env"].as<std::string>();
            DCS_LOG_INFO(logger, "Using env file: " << cmdOptions.envFileName);
        }
        catch (const cxxopts::exceptions::exception& e)
        {
            DCS_LOG_ERROR(logger, "Error parsing options: " << e.what());
            exit(1);
        }
        return cmdOptions;
    }
} // namespace

#include <drogon/drogon.h>
#include <iostream>
#include <thread>
void startDrogon()
{
    try
    {
        // Your Drogon initialization code here
        std::cout << "Starting Drogon..." << std::endl;
        drogon::app().run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error starting Drogon: " << ex.what() << std::endl;
    }
}

void stopDrogon()
{
    try
    {
        std::cout << "Stopping Drogon..." << std::endl;
        drogon::app().quit();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error stopping Drogon: " << ex.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "Starting UI Service App\n";

    auto processName = std::filesystem::path(argv[0]).filename().string();
    std::shared_ptr<DCS::Utils::Logger> logger = std::make_shared<DCS::Utils::LoggerQuill>(processName, LogLevel::Info, true);

    CmdOptions cmdOptions = parseCommandLineOptions(logger, argc, argv);

    auto config = EnvReader(cmdOptions.envFileName);
    std::unordered_map<std::string, std::string> publicKeyMap;
    std::string firebaseApiKey{config.getValueOrDefault<std::string>(processName, "DCS_FIREBASE_API_KEY", "")};

    std::string redisHost{config.getValueOrDefault<std::string>("redis", "host", "localhost")};
    int redisPort{config.getValueOrDefault<int>("redis", "port", 6379)};

    if (firebaseApiKey.empty())
    {
        DCS_LOG_ERROR(logger, "No Project ID specified. Please specify DCS_FIREBASE_PROJECT_ID in your .env file.");
        return 1;
    }
    std::shared_ptr<DCS::UI::RedisHandler> redisHandler = std::make_shared<DCS::UI::RedisHandler>(redisHost, redisPort);

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
    QQuickStyle::setFallbackStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "Light");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "#6200EE"); // Setting
    const auto applicationDirPath = QCoreApplication::applicationDirPath();
    QQmlApplicationEngine engine;
    engine.addImportPath(QString("%1/qml/").arg(applicationDirPath));
    engine.addImportPath("qrc:/");

    qmlRegisterSingletonType(
        QUrl::fromLocalFile(QString("%1/qml/DCSStyles.qml").arg(applicationDirPath)), "dcs.financemanager.style", 1, 0, "DCSStyles");

    // DatabaseManager dbManager;
    // FinanceManager financeManager;
    // financeManager.setDatabaseManager(&dbManager);

    // engine.rootContext()->setContextProperty("databaseManager", &dbManager);
    // engine.rootContext()->setContextProperty("financeManager", &financeManager);
    DCS::UI::FirebaseAuthQt auth(firebaseApiKey, logger, redisHandler);

    engine.rootContext()->setContextProperty("auth", &auth);

    QObject::connect(&auth, &DCS::UI::FirebaseAuth::tokenRenewalRequested, [&engine]() {
        QMetaObject::invokeMethod(engine.rootObjects().first(), "showTokenRenewalNotification");
    });
    engine.load(QUrl::fromLocalFile(QString("%1/qml/main.qml").arg(applicationDirPath)));
    if (engine.rootObjects().isEmpty())
    {
        // LOG_ERROR(appLogger, "Failed to load QML root objects");
        return -1;
    }

    // std::jthread drogonThread(startDrogon);
    int result = app.exec();
    // stopDrogon();
    return result;
}

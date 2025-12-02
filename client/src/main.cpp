#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "BackEnd.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // Register BackEnd as QML type
    qmlRegisterType<BackEnd>("AiLaTrieuPhu.BackEnd", 1, 0, "BackEnd");
    
    QQmlApplicationEngine engine;
    
    // Set application directory path for QML
    QString appDirPath = QCoreApplication::applicationDirPath();
    engine.rootContext()->setContextProperty("applicationDirPath", appDirPath);
    
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}



#include "MainWindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Nexus");
    QCoreApplication::setApplicationName("Nexus Music Player");
    QCoreApplication::setApplicationVersion("1.0.0");
    QApplication::setStyle("Fusion");
    MainWindow window;
    window.show();
    return app.exec();
}

#include "LoginForm.h"
#include "dbhelper.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBHelper::getInstance()->connect();
    MainWindow w;
    w.show();
    return a.exec();
}

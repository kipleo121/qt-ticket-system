#ifndef DBHELPER_H
#define DBHELPER_H

/*请在Air.pro 中加入下面这行代码：
    QT += sql
*/

#include <QtGlobal>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDateTime>

class DBHelper {
public:
    // 退票（取消订单）
    bool cancelOrder(int orderId);

    // 使用该实例来调用DBHelper中的其他函数
    static DBHelper* getInstance() {
        static DBHelper instance;
        return &instance;
    }

    // 连接数据库（启动时调用一次）
    bool connect() {
        if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
            QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        }

        if (db.isOpen()) return true;

        db = QSqlDatabase::addDatabase("QODBC");
        db.setHostName(qEnvironmentVariable("AIR_DB_HOST", "localhost"));
        db.setPort(3306);
        db.setDatabaseName(qEnvironmentVariable("AIR_DB_CONNECTION"));

        if (db.databaseName().isEmpty()) {
            QMessageBox::critical(nullptr, "数据库配置缺失", "请设置 AIR_DB_CONNECTION 环境变量（ODBC DSN 或连接字符串）。");
            return false;
        }
        const QString dbUser = qEnvironmentVariable("AIR_DB_USER");
        const QString dbPassword = qEnvironmentVariable("AIR_DB_PASSWORD");
        if (!dbUser.isEmpty()) db.setUserName(dbUser);
        if (!dbPassword.isEmpty()) db.setPassword(dbPassword);

        if (!db.open()) {
            QMessageBox::critical(nullptr, "连接失败", db.lastError().text());
            return false;
        }
        return true;
    }

    // 获取数据库连接
    QSqlDatabase getDb() {
        return db;
    }

    // 关闭数据库连接（程序退出时调用）
    void close() {
        if (db.isOpen()) {
            db.close();
        }
    }

    //=====登录与注册部分=====
    //登录函数(传入UserID和Password,返回True/False
    bool login_sql(const QString &username, const QString &password);

    // 带个人信息的注册函数（用户名/密码/真实姓名/身份证号）
    bool register_sql(const QString &username, const QString &password,
                      const QString &realName, const QString &idCard);

    // 简化注册
    bool register_sql(const QString &username, const QString &password);

    int get_userid_sql(const QString &username);

    // 通过用户ID获取个人信息（真实姓名/身份证号）
    QMap<QString, QVariant> getUserInfoById(int userId);

    // 修改用户个人信息（真实姓名/身份证号）
    bool updateUserInfo(int userId, const QString &newUserName, const QString &newRealName, const QString &newIdCard);

    //=====航班查询与机票购买部分=====
    //查询所有的航班信息
    QList<QMap<QString, QVariant>> queryFlights_sql();

    //根据不同信息筛选查询航班信息
    QList<QMap<QString, QVariant>> queryFlights_sql(const QString &departCity,
                                                    const QString &arriveCity,
                                                    const QDateTime &departTime);
    // 通过航班号查询航班ID
    int getFlightIdByFlightNo(const QString& flightNo);

    //插入订单信息
    bool insertOrder(int userId, int flightId, const QString& seatNum);

    bool rescheduleOrder(int oldOrderId, int newFlightId, int userId, const QString& seatNum);

    //=====订单查询部分=====
    QList<QMap<QString, QVariant>> getOrdersByUserId(int userId);

    //=====管理员部分=====
    // 新增航班
    bool addFlight(const QString& flightNo, const QString& departCity, const QString& arriveCity,
                   const QDateTime& departTime, const QDateTime& arriveTime,
                   int totalSeats, int remainSeats, int price);

    // 修改航班信息（通过航班号）
    bool updateFlight(const QString& flightNo, const QString& departCity, const QString& arriveCity,
                      const QDateTime& departTime, const QDateTime& arriveTime,
                      int totalSeats, int remainSeats, int price);

    // 删除航班（通过航班号）
    bool deleteFlightByNo(const QString& flightNo);

private:
    DBHelper() {}
    DBHelper(const DBHelper&) = delete;
    DBHelper& operator=(const DBHelper&) = delete;

    QSqlDatabase db ; // 数据库连接对象
};

#endif // DBHELPER_H

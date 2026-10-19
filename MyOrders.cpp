#include "MyOrders.h"
#include <QMap>
#include <QVariant>
#include <QList>
#include "ui_MyOrders.h"
#include "dbhelper.h"
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QHeaderView>

extern int g_loginUserId;

MyOrders::MyOrders(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::MyOrders)
{
    ui->setupUi(this);

    this->resize(1000, 600);

    initOrderTable();
    loadOrdersFromDB();
    setWindowTitle("我的订单");
}

MyOrders::~MyOrders()
{
    delete ui;
}

void MyOrders::initOrderTable()
{
    QStringList headers;
    headers << "订单号" << "航班号" << "出发地" << "目的地"
            << "出发时间" << "座位号" << "票价" << "状态";

    ui->tableWidget_orders->setColumnCount(headers.size());
    ui->tableWidget_orders->setHorizontalHeaderLabels(headers);

    ui->tableWidget_orders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_orders->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget_orders->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    ui->tableWidget_orders->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->tableWidget_orders->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget_orders->setColumnWidth(4, 180);
}

void MyOrders::loadOrdersFromDB()
{
    ui->tableWidget_orders->setRowCount(0);

    DBHelper *db = DBHelper::getInstance();
    if (!db->connect()) {
        QMessageBox::critical(this, "错误", "数据库连接失败");
        return;
    }

    QList<QMap<QString, QVariant>> orders = db->getOrdersByUserId(g_loginUserId);

    if (orders.isEmpty()) {
        QMessageBox::information(this, "提示", "暂无订单记录");
        return;
    }

    for (const auto &order : orders) {
        addOrderRow(order);
    }
    ui->tableWidget_orders->resizeColumnsToContents();
    ui->tableWidget_orders->setColumnWidth(4, 180);
}

void MyOrders::addOrderRow(const QMap<QString, QVariant> &order)
{
    int row = ui->tableWidget_orders->rowCount();
    ui->tableWidget_orders->insertRow(row);

    QDateTime depTime = order["depart_time"].toDateTime();
    QString timeStr = depTime.isValid() ? depTime.toString("yyyy-MM-dd HH:mm") : "未获取到时间";

    QList<QVariant> cols = {
        order["order_id"],
        order["flight_no"],
        order["depart_city"],
        order["arrive_city"],
        timeStr,
        order["seat_num"],
        order["price"],
        order["status"].toInt() == 1 ? "有效" : "已取消"
    };

    for (int c = 0; c < cols.size(); ++c) {
        QTableWidgetItem *item = new QTableWidgetItem(cols[c].toString());
        item->setTextAlignment(Qt::AlignCenter);

        if (c == 7) {
            item->setForeground(order["status"].toInt() == 1 ? Qt::darkGreen : Qt::red);
        }
        ui->tableWidget_orders->setItem(row, c, item);
    }
}

void MyOrders::on_pushButton_clicked()
{
    QModelIndexList selectedRows = ui->tableWidget_orders->selectionModel()->selectedRows();

    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要退票的订单！");
        return;
    }

    int row = selectedRows.first().row();
    QString statusText = ui->tableWidget_orders->item(row, 7)->text();

    if (statusText == "已取消") {
        QMessageBox::information(this, "提示", "该订单已退票！");
        return;
    }

    int orderId = ui->tableWidget_orders->item(row, 0)->text().toInt();

    if (QMessageBox::question(this, "确认退票", "确定要退掉该订单吗？") != QMessageBox::Yes) {
        return;
    }

    DBHelper *db = DBHelper::getInstance();
    if (db->cancelOrder(orderId)) {
        loadOrdersFromDB();
        QMessageBox::information(this, "退票成功", "订单已成功退票！");
    } else {
        QMessageBox::critical(this, "失败", "退票失败，请重试！");
    }
}


void MyOrders::on_pushButton_2_clicked()
{
    int row = ui->tableWidget_orders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要改签的订单！");
        return;
    }

    QString status = ui->tableWidget_orders->item(row, 7)->text();
    if (status == "已取消") {
        QMessageBox::warning(this, "提示", "已取消的订单无法进行改签！");
        return;
    }

    int orderId = ui->tableWidget_orders->item(row, 0)->text().toInt();
    QString depart = ui->tableWidget_orders->item(row, 2)->text();
    QString arrive = ui->tableWidget_orders->item(row, 3)->text();

    int ret = QMessageBox::question(this, "确认改签",
                                    QString("改签将先退掉当前订单（订单ID：%1），再为您重新查询从 %2 到 %3 的航班，是否继续？")
                                        .arg(orderId).arg(depart, arrive));
    if (ret != QMessageBox::Yes) {
        return;
    }

    DBHelper *db = DBHelper::getInstance();
    if (!db->cancelOrder(orderId)) {
        QMessageBox::critical(this, "失败", "原订单退票失败，无法完成改签！");
        return;
    }

    loadOrdersFromDB();
    QMessageBox::information(this, "提示", "原订单已退票，即将为您查询新航班！");

    emit rescheduleRequested(depart, arrive, orderId);
    this->hide();
}

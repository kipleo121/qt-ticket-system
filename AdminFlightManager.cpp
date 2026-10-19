#include "AdminFlightManager.h"
#include "ui_AdminFlightManager.h"
#include "dbhelper.h"
#include "FilterDialog.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <Qt>

AdminFlightManager::AdminFlightManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminFlightManager)
{
    ui->setupUi(this);
    this->setWindowTitle("管理员航班订单管理");
    this->setFixedSize(1200, 800);

    initFlightTable();
    initOrderTable();

    loadAllFlights();
    loadAllOrders();
}


AdminFlightManager::~AdminFlightManager()
{
    delete ui;
}

// 初始化航班表格
void AdminFlightManager::initFlightTable()
{
    QStringList headers = {"航班号", "出发地", "目的地", "出发时间", "到达时间", "总座位", "剩余座位", "票价（元）"};
    ui->flightTable->setColumnCount(headers.size());
    ui->flightTable->setHorizontalHeaderLabels(headers);
    ui->flightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->horizontalHeader()->setStretchLastSection(true);
    ui->flightTable->setColumnWidth(3, 160);
    ui->flightTable->setColumnWidth(4, 160);
}

// 初始化订单表格
void AdminFlightManager::initOrderTable()
{
    QStringList headers = {"订单号", "用户名", "航班号", "出发地", "目的地", "下单时间", "座位号", "票价", "状态"};
    ui->orderTable->setColumnCount(headers.size());
    ui->orderTable->setHorizontalHeaderLabels(headers);
    ui->orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orderTable->horizontalHeader()->setStretchLastSection(true);
    ui->orderTable->setColumnWidth(5, 160);
}

// 加载所有航班
void AdminFlightManager::loadAllFlights()
{
    ui->flightTable->setRowCount(0);
    DBHelper* db = DBHelper::getInstance();
    if (!db->connect()) return;

    QList<QMap<QString, QVariant>> flights = db->queryFlights_sql();
    for (const auto& flight : flights) {
        int row = ui->flightTable->rowCount();
        ui->flightTable->insertRow(row);

        QList<QVariant> data = {
            flight["flight_id"],
            flight["depart_city"],
            flight["arrive_city"],
            flight["depart_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flight["arrive_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flight["total_seats"],
            flight["remaining_seats"],
            flight["price"]
        };

        for (int col = 0; col < data.size(); col++) {
            QTableWidgetItem* item = new QTableWidgetItem(data[col].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->flightTable->setItem(row, col, item);
        }
    }
}

// 加载所有订单
void AdminFlightManager::loadAllOrders()
{
    ui->orderTable->setRowCount(0);
    DBHelper* db = DBHelper::getInstance();
    if (!db->connect()) return;

    QSqlQuery query(db->getDb());
    query.prepare(R"(
        SELECT o.id, u.user_name, f.flight_id, f.depart_city, f.arrive_city,
               o.order_time, o.seat_num, f.price, o.status
        FROM Orders o
        LEFT JOIN Users u ON o.user_id = u.id
        LEFT JOIN Flights f ON o.flight_id = f.id
        ORDER BY o.order_time DESC
    )");

    if (query.exec()) {
        while (query.next()) {
            int row = ui->orderTable->rowCount();
            ui->orderTable->insertRow(row);

            QList<QVariant> data = {
                query.value("id").toInt(),
                query.value("user_name").toString(),
                query.value("flight_id").toString(),
                query.value("depart_city").toString(),
                query.value("arrive_city").toString(),
                query.value("order_time").toDateTime().toString("yyyy-MM-dd HH:mm"),
                query.value("seat_num").toString(),
                query.value("price").toInt(),
                query.value("status").toInt() == 1 ? "有效" : "已取消"
            };

            for (int col = 0; col < data.size(); col++) {
                QTableWidgetItem* item = new QTableWidgetItem(data[col].toString());
                item->setTextAlignment(Qt::AlignCenter);
                if (col == 8) {
                    item->setForeground(query.value("status").toInt() == 1 ? Qt::darkGreen : Qt::red);
                }
                ui->orderTable->setItem(row, col, item);
            }
        }
    }
}

// 获取选中的航班号
QString AdminFlightManager::getSelectedFlightNo()
{
    QModelIndexList selected = ui->flightTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return "";
    return ui->flightTable->item(selected.first().row(), 0)->text();
}

// 获取选中的订单ID
int AdminFlightManager::getSelectedOrderId()
{
    QModelIndexList selected = ui->orderTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return -1;
    return ui->orderTable->item(selected.first().row(), 0)->text().toInt();
}

// 新增航班
void AdminFlightManager::on_btnAddFlight_clicked()
{
    // 获取表单数据
    QString flightNo = ui->leFlightNo->text().trimmed();
    QString departCity = ui->leDepartCity->text().trimmed();
    QString arriveCity = ui->leArriveCity->text().trimmed();
    QDateTime departTime = ui->dtDepart->dateTime();
    QDateTime arriveTime = ui->dtArrive->dateTime();
    int totalSeats = ui->leTotalSeats->text().toInt();
    int remainSeats = ui->leRemainSeats->text().toInt();
    int price = ui->lePrice->text().toInt();

    // 基础校验
    if (flightNo.isEmpty() || departCity.isEmpty() || arriveCity.isEmpty() || totalSeats <= 0 || price <= 0) {
        QMessageBox::warning(this, "提示", "请填写完整且合法的航班信息！");
        return;
    }
    if (arriveTime <= departTime) {
        QMessageBox::warning(this, "提示", "到达时间不能早于出发时间！");
        return;
    }

    // 数据库操作
    DBHelper* db = DBHelper::getInstance();
    if (db->addFlight(flightNo, departCity, arriveCity, departTime, arriveTime, totalSeats, remainSeats, price)) {
        QMessageBox::information(this, "成功", "航班新增成功！");
        loadAllFlights();
        // 清空表单
        ui->leFlightNo->clear();
        ui->leDepartCity->clear();
        ui->leArriveCity->clear();
        ui->leTotalSeats->clear();
        ui->leRemainSeats->clear();
        ui->lePrice->clear();
    } else {
        QMessageBox::critical(this, "失败", "航班新增失败！");
    }
}

// 修改航班
void AdminFlightManager::on_btnModifyFlight_clicked()
{
    QString flightNo = getSelectedFlightNo();
    if (flightNo.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选中要修改的航班！");
        return;
    }

    QString departCity = ui->leDepartCity->text().trimmed();
    QString arriveCity = ui->leArriveCity->text().trimmed();
    QDateTime departTime = ui->dtDepart->dateTime();
    QDateTime arriveTime = ui->dtArrive->dateTime();
    int totalSeats = ui->leTotalSeats->text().toInt();
    int remainSeats = ui->leRemainSeats->text().toInt();
    int price = ui->lePrice->text().toInt();

    DBHelper* db = DBHelper::getInstance();
    if (db->updateFlight(flightNo, departCity, arriveCity, departTime, arriveTime, totalSeats, remainSeats, price)) {
        QMessageBox::information(this, "成功", "航班修改成功！");
        loadAllFlights();
    } else {
        QMessageBox::critical(this, "失败", "航班修改失败！");
    }
}

// 删除航班
void AdminFlightManager::on_btnDeleteFlight_clicked()
{
    QString flightNo = getSelectedFlightNo();
    if (flightNo.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选中要删除的航班！");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定要删除该航班吗？", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    DBHelper* db = DBHelper::getInstance();
    if (db->deleteFlightByNo(flightNo)) {
        QMessageBox::information(this, "成功", "航班删除成功！");
        loadAllFlights();
    } else {
        QMessageBox::critical(this, "失败", "航班删除失败（可能有关联订单）！");
    }
}

// 刷新航班列表
void AdminFlightManager::on_btnRefreshFlight_clicked()
{
    loadAllFlights();
}

// 选中航班自动填充表单
void AdminFlightManager::on_flightTable_cellClicked(int row, int column)
{
    ui->leFlightNo->setText(ui->flightTable->item(row, 0)->text());
    ui->leDepartCity->setText(ui->flightTable->item(row, 1)->text());
    ui->leArriveCity->setText(ui->flightTable->item(row, 2)->text());
    ui->dtDepart->setDateTime(QDateTime::fromString(ui->flightTable->item(row, 3)->text(), "yyyy-MM-dd HH:mm"));
    ui->dtArrive->setDateTime(QDateTime::fromString(ui->flightTable->item(row, 4)->text(), "yyyy-MM-dd HH:mm"));
    ui->leTotalSeats->setText(ui->flightTable->item(row, 5)->text());
    ui->leRemainSeats->setText(ui->flightTable->item(row, 6)->text());
    ui->lePrice->setText(ui->flightTable->item(row, 7)->text());
}

// 刷新订单列表
void AdminFlightManager::on_btnRefreshOrder_clicked()
{
    loadAllOrders();
}

// 取消订单（退票）
void AdminFlightManager::on_btnCancelOrder_clicked()
{
    int orderId = getSelectedOrderId();
    if (orderId == -1) {
        QMessageBox::warning(this, "提示", "请先选中要取消的订单！");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定要取消该订单吗？", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    DBHelper* db = DBHelper::getInstance();
    if (db->cancelOrder(orderId)) {
        QMessageBox::information(this, "成功", "订单取消成功！");
        loadAllOrders();
        loadAllFlights();
    } else {
        QMessageBox::critical(this, "失败", "订单取消失败！");
    }
}

void AdminFlightManager::on_btnFilter_clicked()
{
    FilterDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString departCity = dialog.getDepartCity();
    QString arriveCity = dialog.getArriveCity();
    QDateTime departTime = dialog.getDepartTime();

    loadFilteredFlights(departCity, arriveCity, departTime);

}

// 加载筛选后的航班
void AdminFlightManager::loadFilteredFlights(const QString &departCity,
                                             const QString &arriveCity,
                                             const QDateTime &departTime)
{
    ui->flightTable->setRowCount(0);

    DBHelper* db = DBHelper::getInstance();
    if (!db->connect()) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QList<QMap<QString, QVariant>> flights = db->queryFlights_sql(departCity, arriveCity, departTime);
    if (flights.isEmpty()) {
        QMessageBox::information(this, "提示", "未查询到符合条件的航班！");
        return;
    }

    for (const auto& flight : flights) {
        int row = ui->flightTable->rowCount();
        ui->flightTable->insertRow(row);

        QList<QVariant> data = {
            flight["flight_id"],
            flight["depart_city"],
            flight["arrive_city"],
            flight["depart_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flight["arrive_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flight["total_seats"],
            flight["remaining_seats"],
            flight["price"]
        };

        for (int col = 0; col < data.size(); col++) {
            QTableWidgetItem* item = new QTableWidgetItem(data[col].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->flightTable->setItem(row, col, item);
        }
    }
}

void AdminFlightManager::on_btnCancelChoose_clicked()
{
        ui->leFlightNo->clear();
        ui->leDepartCity->clear();
        ui->leArriveCity->clear();
        ui->leTotalSeats->clear();
        ui->leRemainSeats->clear();
        ui->lePrice->clear();
        ui->dtDepart->setDateTime(QDateTime::currentDateTime());
        ui->dtArrive->setDateTime(QDateTime::currentDateTime());
}


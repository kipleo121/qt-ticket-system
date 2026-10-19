#include "Air_inquiryForm.h"
#include "ui_Air_inquiryForm.h"
#include "dbhelper.h"
#include "filterdialog.h"
#include <QTableWidgetItem>
#include <Qt>
#include <QMessageBox>
#include <QDateTime>
int g_loginUserId=0;
extern int g_loginUserId; // 登录用户ID（登录成功后赋值）

Air_Inquiry::Air_Inquiry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Air_Inquiry)
{
    ui->setupUi(this);
    initFlightTable();
}

void Air_Inquiry::setupRescheduleMode(const QString &depart, const QString &arrive, int oldOrderId)
{
    this->m_isReschedule = true;
    this->m_oldOrderId = oldOrderId;

    refreshFlightTable(depart, arrive, QDateTime());

    QMessageBox::information(this, "改签搜索",
                             QString("已为您找到所有从 %1 到 %2 的可选航班，请选择新时间进行改签。").arg(depart, arrive));
}
Air_Inquiry::~Air_Inquiry()
{
    delete ui;
}

// 初始化航班信息表格
void Air_Inquiry::initFlightTable()
{
    QStringList headers;

    headers << "航班号" << "出发地" << "目的地" << "出发时间" << "到达时间" << "票价（元）" << "剩余座位";
    ui->airtableWidget->setColumnCount(headers.size());
    ui->airtableWidget->setHorizontalHeaderLabels(headers);

    ui->airtableWidget->setShowGrid(true);
    ui->airtableWidget->setGridStyle(Qt::SolidLine);
    ui->airtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->airtableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->airtableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->airtableWidget->setColumnWidth(0, 80);
    ui->airtableWidget->setColumnWidth(3, 160);
    ui->airtableWidget->setColumnWidth(4, 160);
    ui->airtableWidget->setColumnWidth(5, 80);

    DBHelper* dbHelper = DBHelper::getInstance();
    if (!dbHelper->connect()) {
        QMessageBox::critical(this, "数据库错误", "无法连接到数据库！", QMessageBox::Ok);
        return;
    }

    QList<QMap<QString, QVariant>> flightList = dbHelper->queryFlights_sql();
    if (flightList.isEmpty()) {
        QMessageBox::information(this, "提示", "数据库中暂无航班数据！", QMessageBox::Ok);
        return;
    }

    auto addFlightRow = [&](const QMap<QString, QVariant> &flightData) {
        int row = ui->airtableWidget->rowCount();
        ui->airtableWidget->insertRow(row);

        QList<QVariant> columnData = {
            flightData["flight_id"],
            flightData["depart_city"],
            flightData["arrive_city"],
            flightData["depart_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flightData["arrive_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flightData["price"],
            flightData["remaining_seats"]
        };

        for (int col = 0; col < columnData.size(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(columnData[col].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->airtableWidget->setItem(row, col, item);
        }
    };

    for (const auto &flight : flightList) {
        addFlightRow(flight);
    }
}

void Air_Inquiry::on_airtableWidget_cellClicked(int row, int column)
{
    QTableWidgetItem *clickedItem = ui->airtableWidget->item(row, column);
    if (!clickedItem) return;

    ui->txt_flightid->setText(ui->airtableWidget->item(row, 0)->text());
    ui->txt_depart->setText(ui->airtableWidget->item(row, 1)->text());
    ui->txt_arrive->setText(ui->airtableWidget->item(row, 2)->text());
    ui->txt_depart_time->setText(ui->airtableWidget->item(row, 3)->text());
    ui->txt_arrive_time->setText(ui->airtableWidget->item(row, 4)->text());
    ui->txt_price->setText(ui->airtableWidget->item(row, 5)->text());
    ui->txt_remain_seats->setText(ui->airtableWidget->item(row, 6)->text());
}


void Air_Inquiry::on_btn_buy_clicked()
{
    QModelIndexList selectedRows = ui->airtableWidget->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选中要购买的航班！", QMessageBox::Ok);
        return;
    }

    int selectedRow = selectedRows.first().row();
    QString flightNo = ui->txt_flightid->text().trimmed();
    QString realName = ui->txt_realname->text().trimmed();
    QString realId = ui->txt_realid->text().trimmed();
    QString seatNum = ui->lineEdit_10->text().trimmed();
    int remainSeats = ui->txt_remain_seats->text().toInt();


    if (realName.isEmpty() || realId.isEmpty() || seatNum.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写完整的乘客信息（姓名/身份证/座位号）！", QMessageBox::Ok);
        return;
    }

    if (remainSeats <= 0) {
        QMessageBox::warning(this, "提示", "该航班无剩余座位！", QMessageBox::Ok);
        return;
    }

    if (seatNum.length() != 2 || !seatNum.at(0).isDigit() ||
        !(seatNum.at(1).isUpper() && seatNum.at(1).isLetter()) ) {
        QMessageBox::warning(this, "提示", "座位号只能由1位数字和1位大写字母构成！", QMessageBox::Ok);
        return;
    }

    QRegularExpression idRegex("^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$");
    QString newIdCard = realId;
    QRegularExpressionMatch match = idRegex.match(newIdCard);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "提示", "身份证格式不正确！", QMessageBox::Ok);
        return ;
    }

    DBHelper* dbHelper = DBHelper::getInstance();
    if (!dbHelper->connect()) {
        QMessageBox::critical(this, "错误", "数据库连接失败！", QMessageBox::Ok);
        return;
    }
    int flightId = dbHelper->getFlightIdByFlightNo(flightNo);
    if (flightId <= 0) {
        QMessageBox::critical(this, "错误", "未查询到该航班信息！", QMessageBox::Ok);
        return;
    }
    if (dbHelper->insertOrder(g_loginUserId, flightId, seatNum)) {
        // 6. 购买成功：更新UI（新增：余票显示更新+清空输入）
        ui->airtableWidget->item(selectedRow, 6)->setText(QString::number(remainSeats - 1));
        ui->txt_remain_seats->setText(QString::number(remainSeats - 1));
        QMessageBox::information(this, "购买成功",
                                 QString("航班%1机票购买成功！").arg(flightNo), QMessageBox::Ok);
        ui->txt_realname->clear();
        ui->txt_realid->clear();
        ui->lineEdit_10->clear();
        ui->airtableWidget->clearSelection();
    } else {
        QMessageBox::critical(this, "购买失败", "订单写入数据库失败！", QMessageBox::Ok);
    }
}

void Air_Inquiry::on_btn_filter_clicked()
{
    FilterDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    QString departCity = dialog.getDepartCity();
    QString arriveCity = dialog.getArriveCity();
    QDateTime departTime = dialog.getDepartTime();

    refreshFlightTable(departCity, arriveCity, departTime);
}

void Air_Inquiry::refreshFlightTable(const QString &departCity,
                                     const QString &arriveCity,
                                     const QDateTime &departTime)
{
    ui->airtableWidget->setRowCount(0);
    DBHelper* dbHelper = DBHelper::getInstance();
    if (!dbHelper->connect()) {
        return;
    }
    QList<QMap<QString, QVariant>> flightList = dbHelper->queryFlights_sql(departCity, arriveCity, departTime);
    if (flightList.isEmpty()) {
        QMessageBox::information(this, "提示", "未查询到符合条件的航班！", QMessageBox::Ok);
        return;
    }
    auto addFlightRow = [&](const QMap<QString, QVariant> &flightData) {
        int row = ui->airtableWidget->rowCount();
        ui->airtableWidget->insertRow(row);

        QList<QVariant> columnData = {
            flightData["flight_id"],
            flightData["depart_city"],
            flightData["arrive_city"],
            flightData["depart_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flightData["arrive_time"].toDateTime().toString("yyyy-MM-dd HH:mm"),
            flightData["price"],
            flightData["remaining_seats"]
        };

        for (int col = 0; col < columnData.size(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(columnData[col].toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->airtableWidget->setItem(row, col, item);
        }
    };

    for (const auto &flight : flightList) {
        addFlightRow(flight);
    }
}

void Air_Inquiry::on_btn_cancel_clicked()
{
    ui->txt_arrive->clear();
    ui->txt_arrive_time->clear();
    ui->txt_depart_time->clear();
    ui->txt_depart->clear();
    ui->txt_flightid->clear();
    ui->txt_price->clear();
    ui->txt_remain_seats->clear();
    ui->txt_realname->clear();
    ui->txt_realid->clear();
    ui->lineEdit_10->clear();
    ui->airtableWidget->clearSelection();
}

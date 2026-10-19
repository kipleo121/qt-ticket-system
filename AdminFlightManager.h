#ifndef ADMINFLIGHTMANAGER_H
#define ADMINFLIGHTMANAGER_H
#include <QWidget>
#include <QDateTime>

namespace Ui {
class AdminFlightManager;
}

class AdminFlightManager : public QWidget
{
    Q_OBJECT

public:
    explicit AdminFlightManager(QWidget *parent = nullptr);
    ~AdminFlightManager();

private slots:
    // 航班管理槽函数
    void on_btnAddFlight_clicked();
    void on_btnModifyFlight_clicked();
    void on_btnDeleteFlight_clicked();
    void on_btnRefreshFlight_clicked();
    void on_flightTable_cellClicked(int row, int column);

    // 订单管理槽函数
    void on_btnRefreshOrder_clicked();
    void on_btnCancelOrder_clicked();

    void on_btnFilter_clicked();

    void on_btnCancelChoose_clicked();

    void loadFilteredFlights(const QString &departCity,
                             const QString &arriveCity,
                             const QDateTime &departTime);

private:
    Ui::AdminFlightManager *ui;
    void initFlightTable(); // 初始化航班表格
    void initOrderTable();  // 初始化订单表格
    void loadAllFlights();  // 加载所有航班
    void loadAllOrders();   // 加载所有订单
    QString getSelectedFlightNo(); // 获取选中的航班号
    int getSelectedOrderId();      // 获取选中的订单ID
};

#endif // ADMINFLIGHTMANAGER_H

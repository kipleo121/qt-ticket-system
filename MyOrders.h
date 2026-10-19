#ifndef MYORDERS_H
#define MYORDERS_H

#include <QWidget>

namespace Ui {
class MyOrders;
}

class MyOrders : public QWidget
{
    Q_OBJECT

public:
    explicit MyOrders(QWidget *parent = nullptr);
    ~MyOrders();

signals:
    void rescheduleRequested(const QString &depart, const QString &arrive, int oldOrderId);

private slots:

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MyOrders *ui;
    void initOrderTable();
    void loadOrdersFromDB();
    void addOrderRow(const QMap<QString, QVariant> &order);
};

#endif // MYORDERS_H

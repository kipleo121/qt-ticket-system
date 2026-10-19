#ifndef AIR_INQUIRYFORM_H
#define AIR_INQUIRYFORM_H
#include <QDateTime>
#include <QWidget>

namespace Ui {
class Air_Inquiry;
}

class Air_Inquiry : public QWidget
{
    Q_OBJECT

public:
    explicit Air_Inquiry(QWidget *parent = nullptr);
    ~Air_Inquiry();

    void setupRescheduleMode(const QString &depart,const QString &arrive, int oldOrderId);

private slots:
    void on_airtableWidget_cellClicked(int row, int column);

    void on_btn_buy_clicked();

    void on_btn_filter_clicked();

    void refreshFlightTable(const QString &departCity = "",
                            const QString &arriveCity = "",
                            const QDateTime &departTime = QDateTime());

    void on_btn_cancel_clicked();

private:
    Ui::Air_Inquiry *ui;
    bool m_isReschedule = false; // 是否处于改签模式
    int m_oldOrderId = -1;       // 存储待取消的旧订单ID
    void initFlightTable();
};

#endif // AIR_INQUIRYFORM_H

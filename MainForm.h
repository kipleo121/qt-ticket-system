#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QPainter>

#ifndef MAINFORM_H
#endif // MAINFORM_H
class QCloseEvent;

namespace Ui {
class Form1;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void paintEvent(QPaintEvent * event) override;
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnAbout_clicked();

    void on_btnNotice_clicked();

    void on_btnFlightQuery_clicked();

    void on_btnMyOrders_clicked();

    void on_btnPersonalInfo_clicked();

private:
    Ui::Form1 *ui;
};

#endif // MAINFORM_H

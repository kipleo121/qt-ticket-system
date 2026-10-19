#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();


    QString getDepartCity() const;
    QString getArriveCity() const;
    QDateTime getDepartTime() const;

private slots:

    void on_btn_confirm_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H

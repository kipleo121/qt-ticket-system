#include "FilterDialog.h"
#include "ui_filterdialog.h"
#include <QDateTime>

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
    setWindowTitle("航班筛选");
    ui->dateTimeEdit_depart->setDateTime(QDateTime::currentDateTime());
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

QString FilterDialog::getDepartCity() const
{
    return ui->lineEdit_depart->text().trimmed();
}

QString FilterDialog::getArriveCity() const
{
    return ui->lineEdit_arrive->text().trimmed();
}

QDateTime FilterDialog::getDepartTime() const
{
    if (!ui->checkBox_time->isChecked()) {
        return QDateTime();
    }
    return ui->dateTimeEdit_depart->dateTime();
}

void FilterDialog::on_btn_confirm_clicked()
{
    accept();
}

void FilterDialog::on_btn_cancel_clicked()
{
    reject();
}

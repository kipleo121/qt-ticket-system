#include "AdminLoginDialog.h"
#include "ui_AdminLoginDialog.h"
#include <QMessageBox>
#include <QtGlobal>

AdminLoginDialog::AdminLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminLoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("管理员验证");
    this->setFixedSize(300, 180);
    ui->lePassword->setEchoMode(QLineEdit::Password);
}

AdminLoginDialog::~AdminLoginDialog()
{
    delete ui;
}

void AdminLoginDialog::on_btnConfirm_clicked()
{
    QString inputPwd = ui->lePassword->text().trimmed();
    if (inputPwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入管理员密码！");
        return;
    }
    const QString ADMIN_PASSWORD = qEnvironmentVariable("AIR_ADMIN_PASSWORD");
    if (ADMIN_PASSWORD.isEmpty()) {
        QMessageBox::warning(this, "配置缺失", "请设置 AIR_ADMIN_PASSWORD 环境变量后再启动程序。");
        return;
    }
    if (inputPwd == ADMIN_PASSWORD) {
        QMessageBox::information(this, "成功", "管理员验证通过！");
        this->accept(); // 验证成功，返回确认状态
    } else {
        QMessageBox::warning(this, "失败", "密码错误，请重试！");
        ui->lePassword->clear();
    }
}

void AdminLoginDialog::on_btnCancel_clicked()
{
    this->reject();
}

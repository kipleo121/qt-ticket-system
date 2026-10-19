#include "RegisterForm.h"
#include "ui_RegisterForm.h"
#include "dbhelper.h"
#include <QMessageBox>
RegisterForm::RegisterForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterForm)
{
    ui->setupUi(this);
    this->setWindowTitle("用户注册");
}
RegisterForm::~RegisterForm()
{
    delete ui;
}
void RegisterForm::on_btnRegister_clicked()
{
    QString user_name = ui->leUsername->text().trimmed();
    QString user_password = ui->lePassword->text().trimmed();
    QString realName = ui->leRealName->text().trimmed();
    QString idCard = ui->leIdCard->text().trimmed();
    bool success = DBHelper::getInstance()->register_sql(user_name,user_password, realName, idCard);
    if (success) {
        QMessageBox::information(this, "注册成功", "注册成功！请返回登录。");
        on_btnReset_clicked();
        this->accept();
    }
}
void RegisterForm::on_btnReset_clicked()
{
    ui->leUsername->clear();
    ui->lePassword->clear();
    ui->leRealName->clear();
    ui->leIdCard->clear();
    ui->leUsername->setFocus();
}

QString RegisterForm::getUsername() const {
    return ui->leUsername->text().trimmed();
}

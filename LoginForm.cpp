#include "LoginForm.h"
#include "dbhelper.h"
#include "ui_LoginForm.h"
#include <QLineEdit>
#include <QApplication>
#include <QMessageBox>
#include "MainForm.h"
#include "RegisterForm.h"
#include "AdminLoginDialog.h"

#include "AdminFlightManager.h"

MainWindow* g_loginWindow = nullptr;

extern int g_loginUserId;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    g_loginWindow = this;

    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    ui->centralwidget->setStyleSheet(
        "QWidget#centralwidget {"
        "    background-image: url(:/basepic.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "    background-size: cover;"
        "}"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_login_clicked()
{
    QString username = ui->lineEdit_user->text().trimmed();
    QString password = ui->lineEdit_password->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名和密码不能为空！");
        return;
    }

    if (DBHelper::getInstance()->login_sql(username,password)) {
        Form *mainForm = new Form();
        mainForm->setAttribute(Qt::WA_DeleteOnClose);
        mainForm->show();
        this->hide();

        g_loginUserId = DBHelper::getInstance()->get_userid_sql(username);

        connect(mainForm, &Form::destroyed, this, [this]() {
            this->show();
        });
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
        ui->lineEdit_password->clear();
        ui->lineEdit_user->setFocus();
    }
}

void MainWindow::on_btn_cancel_clicked()
{
    this->close();
}

void MainWindow::on_btn_register_clicked()
{
    RegisterForm registerDlg(this);
    int result = registerDlg.exec();
    if (result == QDialog::Accepted) {

        ui->lineEdit_user->setText(registerDlg.getUsername());
    }
}
void MainWindow::on_btnAdminLogin_clicked()
{
    AdminLoginDialog adminDlg(this);
    if (adminDlg.exec() == QDialog::Accepted) {
        AdminFlightManager* adminForm = new AdminFlightManager();
        adminForm->setAttribute(Qt::WA_DeleteOnClose);
        adminForm->show();
        this->hide();

        connect(adminForm, &AdminFlightManager::destroyed, this, [this]() {
            this->show();
        });
    }
}

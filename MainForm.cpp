#include "MainForm.h"
#include "ui_MainForm.h"
#include "LoginForm.h"
#include <QCloseEvent>
#include <QApplication>
#include <QMessagebox>
#include "Air_inquiryForm.h"
#include "MyOrders.h"
#include "MainForm.h"
#include "ui_MainForm.h"
#include "PersonalInfoWidget.h"
#include "dbhelper.h"
extern int g_loginUserId;

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form1)
{
    ui->setupUi(this);
    setWindowTitle("飞机票务系统 - 主界面");

}

Form::~Form()
{
    delete ui;
}

void Form::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void Form::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/basepic.png"));
}

void Form::on_btnAbout_clicked()
{
    QMessageBox::about(this, "关于本系统",
                       "✈️ 飞机票务管理系统震撼版\n"
                       "作者：施子扬\n"
                       "功能：航班查询、订票、退票等。\n"
                       "技术栈：Qt 6 + C++");
}


void Form::on_btnNotice_clicked()
{
    QString notice =
        "📌 使用说明：\n\n"
        "1. 登录账号：admin / 123456\n"
        "2. 查询航班：输入出发地、目的地和日期\n"
        "3. 订票：选中航班后点击【订票】\n"
        "4. 退票：在【我的订单】中操作\n"
        "5. 关于/说明：点击右上角按钮查看";

    QMessageBox::information(this, "使用说明", notice);
}


void Form::on_btnFlightQuery_clicked()
{
    Air_Inquiry *air = new Air_Inquiry();
    air->setAttribute(Qt::WA_DeleteOnClose);
    air->show();
}


void Form::on_btnMyOrders_clicked()
{
    MyOrders *w = new MyOrders();
    w->setAttribute(Qt::WA_DeleteOnClose);

    connect(w, &MyOrders::rescheduleRequested, this, [=](QString depart, QString arrive, int oldOrderId){
        Air_Inquiry *air = new Air_Inquiry();

        air->setAttribute(Qt::WA_DeleteOnClose);

        connect(air, &Air_Inquiry::destroyed, w, [=](){
        });

        air->setupRescheduleMode(depart, arrive, oldOrderId);
        air->show();
    });

    w->show();
}
void Form::on_btnPersonalInfo_clicked()
{
    PersonalInfoWidget *infoWidget = new PersonalInfoWidget();
    infoWidget->setWindowTitle("个人信息");
    infoWidget->setWindowModality(Qt::ApplicationModal);
    infoWidget->setAttribute(Qt::WA_DeleteOnClose);
    infoWidget->setCurrentUserId(g_loginUserId);
    infoWidget->show();
}

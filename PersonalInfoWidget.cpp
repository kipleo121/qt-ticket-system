#include "PersonalInfoWidget.h"
#include "ui_PersonalInfoWidget.h"
#include "dbhelper.h"
#include <QMessageBox>
#include <QMap>
#include <QVariant>

PersonalInfoWidget::PersonalInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PersonalInfoWidget),
    m_currentUserId(-1)
{
    ui->setupUi(this);

    setInputEnabled(false);
}

PersonalInfoWidget::~PersonalInfoWidget()
{
    delete ui;
}

void PersonalInfoWidget::loadUserInfo()
{
    if (m_currentUserId == -1) {
        QMessageBox::warning(this, "错误", "未选择用户");
        return;
    }
    QMap<QString, QVariant> userInfo = DBHelper::getInstance()->getUserInfoById(m_currentUserId);
    if (userInfo.isEmpty()) {
        QMessageBox::information(this, "提示", "未查询到用户信息");
        return;
    }
    m_username = userInfo["user_name"].toString();
    ui->leUsername->setText(m_username);

    ui->leRealName->setText(userInfo["real_name"].toString());
    ui->leIdCard->setText(userInfo["real_id"].toString());
}

void PersonalInfoWidget::setInputEnabled(bool enabled)
{
    ui->leUsername->setEnabled(enabled);
    ui->leRealName->setEnabled(enabled);
    ui->leIdCard->setEnabled(enabled);
}

void PersonalInfoWidget::on_btnModify_clicked()
{
    if (m_currentUserId == -1) {
        QMessageBox::warning(this, "错误", "未选择用户");
        return;
    }
    setInputEnabled(true);
    QMessageBox::information(this, "提示", "已进入修改模式，请填写信息后点击保存");
}

void PersonalInfoWidget::setCurrentUserId(int userId)
{
    this->m_currentUserId = userId;
    loadUserInfo();
}

void PersonalInfoWidget::on_btnSave_clicked()
{
    if (m_currentUserId == -1) {
        QMessageBox::warning(this, "错误", "未选择用户");
        return;
    }

    QString UserName = ui->leUsername->text().trimmed();
    QString realName = ui->leRealName->text().trimmed();
    QString idCard = ui->leIdCard->text().trimmed();

    bool success = DBHelper::getInstance()->updateUserInfo(m_currentUserId, UserName, realName, idCard);
    if (success) {
        QMessageBox::information(this, "成功", "个人信息修改完成");
        setInputEnabled(false);
    } else {
        QMessageBox::critical(this, "失败", "信息修改失败，请重试");
    }
}

void PersonalInfoWidget::on_btnReset_clicked()
{
    loadUserInfo();
    setInputEnabled(false);
}

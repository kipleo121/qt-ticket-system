#ifndef PERSONALINFOWIDGET_H
#define PERSONALINFOWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class PersonalInfoWidget;
}

class PersonalInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInfoWidget(QWidget *parent = nullptr);
    ~PersonalInfoWidget();
    void setCurrentUserId(int userId);

private slots:
    void on_btnModify_clicked();
    void on_btnSave_clicked();
    void on_btnReset_clicked();

private:
    QString m_userId; // 存储当前用户ID
    Ui::PersonalInfoWidget *ui;
    int m_currentUserId;       // 当前登录用户ID
    QString m_username;
    QSqlQuery m_query;         // 绑定数据库连接的查询对象
    void loadUserInfo();       // 加载用户信息
    void setInputEnabled(bool enabled); // 控制输入框启用状态
};

#endif // PERSONALINFOWIDGET_H

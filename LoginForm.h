#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_login_clicked();
    void on_btn_cancel_clicked();
    void on_btnAdminLogin_clicked();
    void on_btn_register_clicked();

private:
    Ui::MainWindow *ui;
};

extern MainWindow* g_loginWindow;

#endif // LOGINFORM_H

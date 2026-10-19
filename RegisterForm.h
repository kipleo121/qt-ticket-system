#ifndef REGISTERFORM_H
#define REGISTERFORM_H
#include <QDialog>
namespace Ui {
class RegisterForm;
}
class RegisterForm : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterForm(QWidget *parent = nullptr);

    ~RegisterForm();

    QString getUsername() const;
private slots:
    void on_btnRegister_clicked();
    void on_btnReset_clicked();
private:
    Ui::RegisterForm *ui;
};
#endif // REGISTERFORM_H

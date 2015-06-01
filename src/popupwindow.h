#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QDialog>

namespace Ui {
class popupWindow;
}

class popupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit popupWindow(QWidget *parent = 0);
    ~popupWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::popupWindow *ui;
};

#endif // POPUPWINDOW_H

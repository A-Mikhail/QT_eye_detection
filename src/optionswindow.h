#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>

namespace Ui {
class optionsWindow;
}

class optionsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit optionsWindow(QWidget *parent = 0);
    ~optionsWindow();

private slots:
    void on_pushButton_clicked();

    void on_toolButton_clicked();

private:
    Ui::optionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

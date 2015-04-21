#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QMainWindow>

namespace Ui {
class aboutwindow;
}

class aboutwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit aboutwindow(QWidget *parent = 0);
    ~aboutwindow();

private:
    Ui::aboutwindow *ui;
};

#endif // ABOUTWINDOW_H

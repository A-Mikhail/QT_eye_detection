#ifndef SELECTWEBCAMWINDOW_H
#define SELECTWEBCAMWINDOW_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class selectWebcamWindow;
}

class selectWebcamWindow : public QWidget
{
    Q_OBJECT

public:
    explicit selectWebcamWindow(QWidget *parent = 0);

    ~selectWebcamWindow();

signals:
    void signalWebcamIndex(QString);

private slots:
    void on_pushButton_clicked();

private:
    Ui::selectWebcamWindow *ui;

    QPushButton *button;
};

#endif // SELECTWEBCAMWINDOW_H

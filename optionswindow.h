#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QWidget>

namespace Ui {
class optionsWindow;
}

class optionsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit optionsWindow(QWidget *parent = 0);
    ~optionsWindow();

private slots:

private:
    Ui::optionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

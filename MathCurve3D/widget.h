#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    double alpha, beta, gamma, inpA, inpB, inpC, inpDelta;
    bool flag, dF;
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private slots:
    void on_pushButton_clicked(bool checked);

    void on_horizontalScrollBar_valueChanged(int value);

    void on_verticalScrollBar_valueChanged(int value);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_valueChanged(double arg1);


    void on_lineEdit_textChanged(const QString &arg1);

    void on_checkBox_toggled(bool checked);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H

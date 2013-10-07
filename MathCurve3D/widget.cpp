#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QChar>
#include "math.h"
#include <QColor>

const double pi = 3.1415926535;

class my_matrix4 {
  private:

    double A[16];
    int dim[2];

  public:

    my_matrix4(double val){
        int i, j;
        dim[0] = 4;
        dim[1] = 4;
        for ( i = 0; i < dim[0]; i++ )
            for ( j = 0; j < dim[1]; j++ ){
                if (i == j)
                    A[i*dim[1] + j] = 1.0;
                else
                    A[i*dim[1] + j] = 0.0;
            }
    }

    ~my_matrix4(){
    }

    double getElem( int i, int j ){

        return A[ i * dim[1] + j];

    }

    void setElem ( int i, int j, double val ){
        A[ i * dim[1] + j] = val;
    }



    };


class my_vector4{

  private:

    double x[4];
    int n;

  public:

    my_vector4 ( double vx, double vy, double vz ){
        n = 4;
        x[0] = vx;
        x[1] = vy;
        x[2] = vz;
        x[3] = 1.0;
    }

    ~my_vector4(){
    }

    double getElem(int i){
        return x[i];
    }

    void setElem(int i, double val){
        x[i] = val;
    }

    void numMul(double num){
        x[0] *= num;
        x[1] *= num;
        x[2] *= num;
    }

};

double maxAbs(double a, double b){
    if (abs(a) > abs(b))
        return abs(a);
    else
        return abs(b);
}



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    alpha(0.0),
    beta(0.0),
    gamma(0.0),
    inpA(0.0),
    inpB(0.0),
    inpC(0.0),
    flag(false),
    inpDelta(0.005),
    dF(false)
{
    ui->setupUi(this);
    inpA = 1.0;
    ui->doubleSpinBox->setValue(inpA);
    inpB = 1.0;
    ui->doubleSpinBox_2->setValue(inpB);
    inpC = 4.0;
    ui->doubleSpinBox_3->setValue(inpC);
    ui->lineEdit->setText("0.005");
    QString s = QChar(948);
    ui->label_7->setText(s+"t");
    ui->checkBox->setText("Derivative-dependent\n"+s+"t");
}



Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked(bool checked)
{
    this->close();
}

void vecMatMul( my_vector4 &x, my_matrix4 A );

void rotateX( my_vector4 &xyz,
                 double angle )
{
    my_matrix4 U(0);

    U.setElem(1, 1, cos(angle));
    U.setElem(1, 2, sin(angle));
    U.setElem(2, 1, -sin(angle));
    U.setElem(2, 2, cos(angle));

    vecMatMul(xyz, U);

}


void rotateY( my_vector4 &xyz,
               double angle )
{
    my_matrix4 U(0);

    U.setElem(0, 0, cos(angle));
    U.setElem(0, 2, sin(angle));
    U.setElem(2, 0, -sin(angle));
    U.setElem(2, 2, cos(angle));

    vecMatMul(xyz, U);
}

/*void rotate_z( double &x, double &y, double &z, double angle )
{
    double xx, yy, zz, xxx, yyy, zzz;
    xx = x;
    yy = y;
    zz = z;

    xxx = xx * cos( angle ) - yy * sin( angle );
    yyy = xx * sin( angle ) + yy * cos( angle );
    zzz = zz;

    x = xxx;
    y = yyy;
    z = zzz;
}*/


void vecMatMul( my_vector4 &x, my_matrix4 A ){
    double xx[4];
    int i, j;
    for (i=0; i<4; i++){
        xx[i] = 0.0;
        for(j=0; j<4; j++)
            xx[i] += x.getElem(j) * A.getElem(j, i);
    }
    x.setElem(0, xx[0]);
    x.setElem(1, xx[1]);
    x.setElem(2, xx[2]);
    x.setElem(3, xx[3]);

}

void vecSum (my_vector4 &x, my_vector4 y){
    double res[4];
    res[0] = x.getElem(0) + y.getElem(0);
    res[1] = x.getElem(1) + y.getElem(1);
    res[2] = x.getElem(2) + y.getElem(2);
    res[3] = 1.0;
    x.setElem(0, res[0]);
    x.setElem(1, res[1]);
    x.setElem(2, res[2]);
}

void getShadowXY(my_vector4 lightPoint, my_vector4 curvePoint, my_vector4 &shadowPoint){
    double x, y, z;

    x = lightPoint.getElem(0) + ( lightPoint.getElem(0) - curvePoint.getElem(0) ) * ( lightPoint.getElem(2) / ( curvePoint.getElem(2) - lightPoint.getElem(2) ));
    y = lightPoint.getElem(1) + ( lightPoint.getElem(1) - curvePoint.getElem(1) ) * ( lightPoint.getElem(2) / ( curvePoint.getElem(2) - lightPoint.getElem(2) ));
    z = 0.0;

    shadowPoint.setElem(0, x);
    shadowPoint.setElem(1, y);
    shadowPoint.setElem(2, z);

}

void Widget::paintEvent(QPaintEvent *)
{
    int i, nSteps, count;
    double t, tt, scale = ( width()/4 + height()/4 )/6, sum, k;
    double x, y, z, delta = 0.005, dxdt = 1, dydt = 1, tempDelta;

    my_vector4 xyz(0.0, 0.0, 0.0), xyz0(width()/2, height()/2, scale*5.0), xxyyzz(0.0, 0.0, 0.0),
            light_point(0.0, 0.0, scale*13.0), xyz_s(0.0,0.0,0.0), xxyyzz_s(0.0, 0.0, 0.0);

    QPainter p(this);

    if (!dF){
        delta = inpDelta;
        nSteps =  (2*pi) / delta  + 1;
    }
    else{
        delta = 0.05;
        count = 0;
        sum = 0;
        t = 0;
        tt = 0;
        k = 1;
        while (sum < 2*pi){
            dxdt = -inpA * sin( t ) + inpB * inpC * sin ( inpC * t );
            dydt = inpA * cos( t ) - inpB * inpC * cos ( inpC * t );
            tempDelta = delta/maxAbs(dxdt, dydt);
            t += tempDelta;
            sum+=tempDelta;
            count++;
        }
        nSteps = count + 1;
    }

    k = 1;
    t = 0;
    tt = 0;
    dxdt = 1;
    dydt = 1;

    for (i = 0; i < nSteps; i++){

        if(dF){
            dxdt = -inpA * sin( t ) + inpB * inpC * sin ( inpC * t );
            dydt = inpA * cos( t ) - inpB * inpC * cos ( inpC * t );
            k = 1/maxAbs(dxdt, dydt);
        }

        tempDelta = delta * k;
        tt = t + tempDelta;

        x = inpA * cos( t ) - inpB * cos ( inpC * t );
        y = inpA * sin( t ) - inpB * sin ( inpC * t );
        z = cos ( sqrt( x * x + y * y ) );
        t = t + tempDelta;

        xyz.setElem(0, x );
        xyz.setElem(1, y );
        xyz.setElem(2, z );

        x = inpA * cos( tt ) - inpB * cos ( inpC * tt );
        y = inpA * sin( tt ) - inpB * sin ( inpC * tt );
        z = cos ( sqrt( x * x + y * y ) );

        xxyyzz.setElem(0, x );
        xxyyzz.setElem(1, y );
        xxyyzz.setElem(2, z );


        if (flag) {
            rotateX ( xyz , beta);
            rotateX ( xxyyzz, beta );

            rotateY ( xyz, alpha );
            rotateY ( xxyyzz, alpha );
        }

        if (!flag){
            rotateY ( xyz , alpha);
            rotateY ( xxyyzz, alpha );

            rotateX ( xyz, beta );
            rotateX ( xxyyzz, beta );
        }

        xyz.numMul(scale);
        xxyyzz.numMul(scale);

        vecSum(xyz, xyz0);
        vecSum(xxyyzz, xyz0);

        getShadowXY(light_point, xyz, xyz_s);
        getShadowXY(light_point, xxyyzz, xxyyzz_s);

        p.setPen(Qt::darkMagenta);
        p.drawLine( xyz.getElem(0), xyz.getElem(1), xxyyzz.getElem(0), xxyyzz.getElem(1) );

        p.setPen(Qt::darkGray);
        p.drawLine( xyz_s.getElem(0), xyz_s.getElem(1), xxyyzz_s.getElem(0), xxyyzz_s.getElem(1) );

    }
}


void Widget::on_horizontalScrollBar_valueChanged(int value)
{
    alpha = value * 0.04;
    flag = true;
    repaint();
}

void Widget::on_verticalScrollBar_valueChanged(int value)
{
    beta = value * 0.04;
    flag = false;
    repaint();
}

void Widget::on_doubleSpinBox_3_valueChanged(double arg1)
{
    inpC = arg1;
    repaint();
}

void Widget::on_doubleSpinBox_2_valueChanged(double arg1)
{
    inpB = arg1;
    repaint();
}

void Widget::on_doubleSpinBox_valueChanged(double arg1)
{
    inpA = arg1;
    repaint();
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
        alpha=e->x()*0.01;
        if (alpha > 2*pi)
            alpha -= 2*pi;
        beta=-e->y()*0.01;
        if (beta > 2*pi)
            beta -= 2*pi;
        repaint();
}

void Widget::on_lineEdit_textChanged(const QString &arg1)
{
    inpDelta = arg1.toDouble();
    if (inpDelta == 0.0)
        inpDelta = 0.005;
    repaint();
}



void Widget::on_checkBox_toggled(bool checked)
{
    if (checked)
        dF = true;
    else
        dF = false;
    repaint();
}

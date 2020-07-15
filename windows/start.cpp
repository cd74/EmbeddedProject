#include "start.h"
#include "ui_start.h"
#include "qt1.h"

start::start(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::start)
{
    ui->setupUi(this);
}

start::~start()
{
    delete ui;
}

void start::on_pushButton_clicked()
{
    //if(uid->text()==  && pass->text()== )   
    Qt1 *q=new Qt1();
    q->show();
    this->hide();
}

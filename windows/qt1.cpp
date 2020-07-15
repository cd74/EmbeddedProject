#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <QPainter>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QLineEdit>
#include <QTimer>
#include <pthread.h>
#include <QProcess>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QString>
#include <fstream>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QTableView>
#include <QDateTime>
#include <QByteArray>
#include <QtNetwork>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QtNetwork/QHostInfo>
#include <QStandardItemModel>
#include <QDebug>
#include <QFile>
//#include <QImage>
#include "qt1.h"
#include "dlinklist.c"		//����dlinklist.h������ʾundefined reference

QT_CHARTS_USE_NAMESPACE

static int i=0;
//DLIST* Qt1:: p=NULL;
DLIST *p;
DLIST *q;
DLIST head;
void insert_dlinklist(DLIST *d,char *s);
//=====just for you to learn how to use comboBox=======
void Qt1::InitBox()
{
    comboBox->clear();
    QStringList strList;
    strList<<"aaa"<<"bbb"<<"ccc";
    comboBox->addItems(strList);
}

void Qt1::fun_refresh_label()
{
	lb_sum->setText(comboBox->currentText());
}
//=====just for you to learn how to use comboBox=======

Qt1::Qt1(QWidget *parent):QDialog(parent)
{
  	setupUi(this);
//  rb_manual->setChecked(true);
        isCapOpen = false;
        isToSave = false;
        m_image = NULL;
	rb_manual->setDisabled(true);
	rb_auto->setDisabled(true);
	pb_prev->setDisabled(true);
	pb_next->setDisabled(true);
    pb_del->setDisabled(true);
    comboBox->setVisible(false);
    filenamecheck->setVisible(false);
    //cam_take->setEnabled(false);
//=====just for you to learn how to use comboBox=======
	InitBox();
    //connect(&t3,SIGNAL(timeout()),this,SLOT(fun_refresh_label()));
	t3.start(100);

	udpsocket=new QUdpSocket();
    udpsocket->bind(QHostAddress("192.168.1.112"),8888,QUdpSocket::ShareAddress);
	connect(udpsocket, SIGNAL(readyRead()), this, SLOT(receive()));
//=====just for you to learn how to use comboBox=======
    connect(rb_auto,SIGNAL(clicked()),this,SLOT(fun_cntl()));
    connect(rb_manual,SIGNAL(clicked()),this,SLOT(fun_cntl()));
	connect(pb_prev,SIGNAL(clicked()),this,SLOT(fun_prev())); 
	connect(pb_next,SIGNAL(clicked()),this,SLOT(fun_pic())); 
	connect(pb_del,SIGNAL(clicked()),this,SLOT(fun_delete())); 	
	connect(pb_open,SIGNAL(clicked()),this,SLOT(fun_open()));
	connect(cam_open,SIGNAL(clicked()),this,SLOT(fun_cap_open()));
	//connect(cam_take,SIGNAL(clicked()),this,SLOT(fun_take_photo())); 
	connect(history,SIGNAL(clicked()),this,SLOT(showhistory())); 
    connect(commandok,SIGNAL(clicked()),this,SLOT(sendcommand()));
	connect(timeok,SIGNAL(clicked()),this,SLOT(settime()));	
  	connect(&t1,SIGNAL(timeout()),this,SLOT(fun_time()));
 	connect(&t2,SIGNAL(timeout()),this,SLOT(fun_pic())); 
    //connect(&t4,SIGNAL(timeout()),this,SLOT(fun_refresh_pic()));
  	t1.start(1000);						//������ʱ�������1s��ÿ��1s�ᷢ��һ���ź�����ʵ�����߱�
	init_dlinklist(&head);
        width = 640;
        height = 480;
       // myCamera = new Camera("/dev/video0", width, height, 0);
        frameBufRGB = new unsigned char[width * height * 3];
        frameBufYUV = new unsigned char[width * height * 2];
}

void Qt1::receive()
{
	while(udpsocket->hasPendingDatagrams())
        {
            QByteArray b;
            QHostAddress addr;
            QString fname;
            b.resize(udpsocket->pendingDatagramSize());//b.resize(sizeof(unsigned short));
            udpsocket->readDatagram(b.data(),b.size(),&addr);
           //if(addr!=QHostAddress::LocalHost)
           // {
            QDataStream in(&b, QIODevice::ReadOnly);
            unsigned short int recv;
            int time;
            in>>recv;
            if(recv==10005)
            {
                if(isCapOpen)
                {
                    cam_open->setText(tr("Start Image"));
                    isCapOpen = false;
                    pb_prev->setDisabled(false);
                    pb_next->setDisabled(false);
                    pb_del->setDisabled(false);
                    pb_open->setDisabled(false);
                }
                else
                {
                    cam_open->setText(tr("Stop Image"));
                    isCapOpen = true;
                    pb_prev->setDisabled(true);
                    pb_next->setDisabled(true);
                    pb_del->setDisabled(true);
                    pb_open->setDisabled(true);
                }
            }
            if(recv==10003)
            {
                in>>time;
                tlabel->setText("SampleTime:"+QString::number(time,10));
            }
            if(recv==10004)
            {
                in>>fname;
                QImage* img=new QImage;
                img->load(fname);
                lb_pic->setPixmap(QPixmap::fromImage(*img));
            }
            if(recv<=10000)
            {
                rlabel->setText("Resistence:"+QString::number(recv,10));
                if(recv<=1000)
                    alert->setText("TooLow");
                if(recv>=9000)
                    alert->setText("TooHigh");
                if(recv>1000&&recv<9000)
                    alert->setText("None");
                in>>fname;
                QImage* img=new QImage;
                img->load(fname);
                lb_pic->setPixmap(QPixmap::fromImage(*img));
//                QFile file("./res.txt");
//                file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text);
//                file.write(current_date_time.toString("yyyyMMddhhmmss").toUtf8());
//                file.write((' '+QString::number(resis,10)+'\n').toUtf8());
//                file.close();
            }
            /*in>>frameBufRGB;
            udpsocket->readDatagram(fb.data(),307200);
            memcpy(frameBufRGB,fb,307100);//frameBufRGB = fb.data();
			if(m_image != NULL)
        		delete m_image;
            m_image = new QImage(frameBufRGB, width, height, QImage::Format_RGB888);//888
			//printf("img got");
    		lb_pic->setPixmap(QPixmap::fromImage(*m_image));
            QDateTime current_date_time = QDateTime::currentDateTime();
            QFile file("./res.txt");
            file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text);
            file.write(current_date_time.toString("yyyyMMddhhmmss").toUtf8());
            file.write((QString::number(resis,10)+'\n').toUtf8());
            file.close();
			QByteArray qByteArray = (current_date_time.toString("yyyyMMddhhmmss")+".jpg").toUtf8();
			char* fstr = qByteArray.data();
			//printf(fstr);
            m_image->save(fstr,"JPG",-1);*/
				//printf("save ok");
			//printf("sssssssssssss");
           // }
		}
}

void Qt1::settime()
{
	QString stime = timeinput->text();
    unsigned short int a = 10003;
    int time = stime.toInt();
	if(time>4000)
	    time = 4000;
	if(time<200)
	    time = 200;
	QByteArray b;
	QDataStream out(&b, QIODevice::WriteOnly);
    out<<a<<time;
    udpsocket->writeDatagram(b,b.length(),QHostAddress("192.168.1.230"),8888);
    tlabel->setText("SampleTime:"+QString::number(time,10));
}

void Qt1::showhistory()
{
    QFile file("./res.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QLineSeries *series1 = new QLineSeries();
    series1->setVisible(true);
    series1->setPointLabelsVisible(true);

    int i=0;
    while(!file.atEnd())
    {
        QByteArray t = file.readLine();
        QString str(t);
        qDebug()<< str;
        printf("%s",str);
        int data = str.split(" ")[1].toInt();
        series1->append(i, data);
        i++;
    }
    file.close();
    series1->append(0, 3219);
    series1->append(1, 3258);
    series1->append(2, 3342);
    series1->append(3, 3401);
    series1->append(4, 3425);
    series1->append(5, 3427);
    series1->append(6, 3426);
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series1);
    series1->setUseOpenGL(true);
    chart->createDefaultAxes();
    chart->setTitle(QStringLiteral("History"));
    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->resize(600,400);
    view->show();
    system("./res.txt");
}

void Qt1::sendcommand()
{
    QByteArray b;
    QDataStream out(&b, QIODevice::WriteOnly);
    QString cmd = commandinput->text();
    unsigned short int a=10005;
    out<<a<<cmd;
    udpsocket->writeDatagram(b,b.length(),QHostAddress::Broadcast,8888);
}

/*
void Qt1::fun_refresh_pic()
{
   if(!myCamera->GetBuffer(frameBufYUV))
   {
       std::cout<< "Get Camera Buf error!\n";
       return;
   }
   //TODO:process_image() function is now empty.
   myCamera->process_image(frameBufYUV, frameBufRGB);
   showCapPhoto();

}*/



void Qt1::fun_cap_open()
{
    //TODO: Use myCamera->OpenDevice() to open camera, and myCamera->CloseDevice() to close it.
    //You should think of it in two cases: camera closed and camera opened.
    //When you open camera, how to refresh image? Tips:use timer to trigge it.
	QByteArray b;
	QDataStream out(&b, QIODevice::WriteOnly);
    unsigned short int ctrl;
	if(isCapOpen)
	{
        ctrl = 10002;
        cam_open->setText(tr("Start Image"));
		isCapOpen = false;
        //pb_prev->setDisabled(false);
        //pb_next->setDisabled(false);
        //pb_del->setDisabled(false);
        pb_open->setDisabled(false);
        rlabel->setText("Resiatence:");
        tlabel->setText("SampleTime:");
        alert->setText("None");
	}
	else
	{
        ctrl = 10001;
		cam_open->setText(tr("Stop Image"));
		isCapOpen = true;
        pb_prev->setDisabled(true);
        pb_next->setDisabled(true);
        pb_del->setDisabled(true);
        pb_open->setDisabled(true);
	}
	out<<ctrl;  
    udpsocket->writeDatagram(b,b.length(),QHostAddress("192.168.1.230"),8888);
	/*if(isCapOpen==true)
	{
	    t4.stop();
		myCamera->CloseDevice();
		isCapOpen = false;
		cam_open->setText(tr("Start Camera"));
		cam_take->setEnabled(false);
		lb_pic->clear();
	}
	else
	{
        myCamera->OpenDevice();
		isCapOpen = true;
		cam_open->setText(tr("Stop Camera"));
		cam_take->setEnabled(true);
		t4.start(200);
	}*/
}
void Qt1::fun_clean_pixmap()
{
    QPixmap p;
    lb_pic->setPixmap(p);
}



//void Qt1::fun_take_photo()
//{
//    //TODO: When this button is clicked, we take a photo and save it.
///*	if(!myCamera->GetBuffer(frameBufYUV))
//    {
//       std::cout<< "Get Camera Buf error!\n";
//       return;
//    }
//   //TODO:process_image() function is now empty.
//    myCamera->process_image(frameBufYUV, frameBufRGB);*/
//	if(m_image != NULL)
//        delete m_image;
//	m_image = NULL;
//    m_image = new QImage(frameBufRGB, width, height, QImage::Format_RGB888);
//	QDateTime current_date_time = QDateTime::currentDateTime();
//	QString filename;
//	if(filenamecheck->isChecked())
//	    filename = "img"+filenameinput->text()+".jpg";
//	else
//        filename = current_date_time.toString("yyyyMMddhhmmss")+".jpg";// .
//	QByteArray qByteArray = filename.toUtf8();
//    char* fstr = qByteArray.data();
//	printf(fstr);
//	if(m_image->save(fstr,"JPG",-1))
//	    printf("save ok");
//	printf("sssssssssssss");
//}
//void Qt1::showCapPhoto()
//{

//    if(m_image != NULL)
//        delete m_image;
//    m_image = NULL;
//	printf("get img");
//    m_image = new QImage(frameBufRGB, width, height, QImage::Format_RGB888);//888
//	printf("img got");
//    lb_pic->setPixmap(QPixmap::fromImage(*m_image));
//}


void Qt1::fun_time()
{
    QDateTime d=QDateTime::currentDateTime();	//��ȡ��ǰʱ��,currentDateTime()��һ����̬����
    lb_time->setText(d.toString("yyyy-MM-dd-ddd hh:mm:ss"));	//����ʱ���ʽ
}

void Qt1::fun_cntl()
{
	if(rb_manual->isChecked ())
	{
		t2.stop();
		pb_prev->setEnabled(true);	
		pb_next->setEnabled(true);	
	}
	else if(rb_auto->isChecked ())
	{
        t2.start(speedinput->text().toInt());
		pb_prev->setEnabled(false);	
		pb_next->setEnabled(false);	
	}
}

void Qt1::fun_pic()		
{
	i++;
	if(i==(len+1))i=1;

	p=p->next;
	display_pic();
}


void Qt1::fun_prev()	//fun_next()��fun_pic()һ��
{
	i--;
	if(i==0)i=len;

	p=p->prev;
	display_pic();
}

void Qt1::display_pic()
{
	QString buf(p->path);
	QPixmap p(buf);
	lb_pic->setPixmap(p);
	lb_pic->setScaledContents(1);
	lb_num->setText(QString::number(i));
	lb_sum->setText(QString::number(len));
}


void Qt1::fun_delete()
{
	int ret;
        ret=QMessageBox::warning(this, tr("m_image browser"),
					   tr("Are you sure to delete it?"),
					   QMessageBox::Yes | QMessageBox::No,
					   QMessageBox::No);
	if(ret==QMessageBox::Yes)
	{
		remove(p->path);
		DLIST *r=(&head)->next;
		int n=1;
		while(r!=p)		//�ҵ���ǰ p��λ��
		{
			n++;
			r=r->next;
		}
		p=p->next;
		del_dlinklist(&head,n);
		if(i==len)i--;
		len=legnth_dlinklist(&head);
		display_pic();
	}
//	dlinkilist_tofile(&head);
}

void Qt1::fun_open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 "/mnt/disk/qrssy/photo2",
                                                 tr("Images (*.png *.xpm *.jpg)"));
	printf("open folder!\n");
	if(fileName!=NULL)
	{
		init_dlinklist(&head);	//Ϊ����һ��Ŀ¼��׼��
		char *str;
		char filename[100]={0};
		DIR *dp;					//ָ��Ŀ¼
		struct dirent *dirp;		//ָ��Ŀ¼��(��������Ŀ¼����ͨ�ļ�)
		
        int m=strlen(fileName.toLatin1().data());
		str=(char*)malloc(m);
        strcpy(str,fileName.toLatin1().data());
		strcpy(filename,str);

        QString buf(fileName.toLatin1().data());
		QPixmap first(buf);
		lb_pic->setPixmap(first);
		lb_pic->setScaledContents(1);
		
		strip(filename);
		dp=opendir(filename);
		while(dirp=readdir(dp))
		{
			if(judge(dirp->d_name)==1)
			{
				strcat(filename,"/");
				strcat(filename,dirp->d_name);	//�ļ����ڵ�Ŀ¼+�ļ���=����·��
				printf("%s\n",filename);
				insert_dlinklist(&head,filename);
				filename[strlen(filename)-strlen(dirp->d_name)-1]='\0';
			}	
		}
		q->next=(&head)->next;
		(&head)->next->prev=q;
		
		len=legnth_dlinklist(&head);
		int pos=1;
		DLIST *r=(&head)->next;
		while(strcmp(r->path,str)!=0)
		{
			r=r->next;
			pos++;
		}
		p=r;
		i=pos;
		lb_num->setText(QString::number(i));
		lb_sum->setText(QString::number(len));
		
		rb_manual->setDisabled(false);
		rb_auto->setDisabled(false);
		pb_prev->setDisabled(false);
		pb_next->setDisabled(false);
		pb_del->setDisabled(false);
		rb_manual->setChecked(true);
//		display_pic();
	}
//	dlinkilist_tofile(&head);
}

void Qt1::strip(char *s)
{
	char *p=s;
	int n=0;
	while(*p)
	{
		if(*p=='/')
		{
			n++;
		}
		p++;
	}
	if(n==1)
	{
		s[1]='\0';
	}
	else
	{
		p--;
		while(*p)
		{
			if(*p=='/')
			{
				*p='\0';
				break;
			}
			p--;
		}
	}
}

int Qt1::judge(char * s)	
{
	int i,len=strlen(s);
	char *p=s;
	if(len<5)
	{
		return 0;
	}
	else 
	{
		while(*p)
		{
			p++;
		}
		for(i=0;i<4;i++)p--;
		if((strncmp(p,".jpg",4)==0)||(strncmp(p,".png",4)==0)||(strncmp(p,".xpm",4)==0))
		{
			return 1;
		}
		else 
			return 0;
	}
}

void insert_dlinklist(DLIST *d,char *s)	//������ĩβ����,֮���԰���������ᵽ�������Ϊ����ʹ��ȫ�ֱ���DLIST *q
{
	DLIST *r=d;
//	DLIST *q;
	q=(DLIST*)malloc(sizeof(DLIST));	//�����q�����Ǿֲ��ģ������޷��γ�˫��ѭ������
	memset(q,0,sizeof(q));
	strcpy(q->path,s);

	if(r->next==NULL)
	{
		r->next=q;
		q->next=NULL;
	}
	else
	{
		while(r->next!=NULL)
		{
			r=r->next;
		}
		r->next=q;
		q->prev=r;
		q->next=NULL;
	}
	printf("insert success\n");
}


Qt1::~Qt1()
{
    delete[] frameBufRGB;
    delete[] frameBufYUV;
    delete m_image;
}

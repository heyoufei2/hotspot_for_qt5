#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <stdio.h>
#include <string.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->open_Button, SIGNAL(clicked()), SLOT(open_hotspot()));
    connect(ui->password_Button, SIGNAL(clicked()), SLOT(set_no_password()));
    connect(ui->update_Button, SIGNAL(clicked()), SLOT(update_client_list()));
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);   //设置选择模式，单行选择
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格不可编辑
    ui->tableWidget->setColumnWidth(0, 400);

    this->hotspot_flag = true;
    this->password_flag = true;
    //ui->ssid_lineEdit->clear();
    ui->tableWidget->clear();
    ui->update_Button->setEnabled(false);
    //ui->password_lineEdit->clear();
    ui->ssid_lineEdit->setText("C600g_AP");
    ui->password_lineEdit->setText("hello123");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_hotspot()
{
    if(hotspot_flag == true)
    {
        ui->open_Button->setText("close hotspot");
        ui->password_Button->setEnabled(false);
        this->hotspot_flag = false;
        ui->ssid_lineEdit->setEnabled(false);
        ui->password_lineEdit->setEnabled(false);

        QString ssid_str = ui->ssid_lineEdit->text();
        QString hotspot_password = ui->password_lineEdit->text();
        qDebug() <<"ssid_str = "<< ssid_str << "hotspot_password = "<< hotspot_password;
        if(ssid_str.isEmpty() == true) //如果ap的名字为空的话
            ssid_str = "C600g_AP";
        modify_hostapd_conf(ssid_str, hotspot_password);


        system("ifconfig wlan0 192.168.0.1");
        system("udhcpd /etc/udhcpd.conf");
        system("hostapd /etc/hostapd.conf &");
        ui->update_Button->setEnabled(true);
    }
    else
    {
        ui->open_Button->setText("open hotspot");
        ui->password_Button->setEnabled(true);
        ui->update_Button->setEnabled(false);
        ui->ssid_lineEdit->setEnabled(true);
        if(this->password_flag == true)
            ui->password_lineEdit->setEnabled(true);
        this->hotspot_flag = true;
       // ui->ssid_lineEdit->clear();
        ui->tableWidget->clear();
        system("killall hostapd");

    }
}

void MainWindow::set_no_password()
{
    if(password_flag == true)
    {
        ui->password_Button->setText("set need password");
        this->password_flag = false;
        //ui->password_lineEdit->clear();
        ui->password_lineEdit->setEnabled(false);
    }
    else
    {
        ui->password_Button->setText("set no password");
        this->password_flag = true;
        ui->password_lineEdit->setEnabled(true);
    }
}

void MainWindow::update_client_list()
{
    ui->tableWidget->clear();
    system("hostapd_cli -i wlan0 all_sta > /etc/hostapd_cli.conf");
    FILE *fp;
    char ch[100];
    int i = 0;
    fp = fopen("/etc/hostapd_cli.conf", "r");
    while(fgets(ch, 100, fp) != NULL)
    {
        printf("strlen(ch) = %d\n", strlen(ch));
        printf("%s", ch);
        if(strlen(ch) == 18 && ch[2] == ch[5] && ch[2] == ':') //找到一个mac地址
        {
            QString str = QString(QLatin1String(ch));
             ui->tableWidget->setRowCount(i+1);
             ui->tableWidget->setItem(i, 0, new QTableWidgetItem(str));
             i++;
        }
    }
    ui->tableWidget->show();
    fclose(fp);
}

void MainWindow::modify_hostapd_conf(QString ssid_str, QString hotspot_password)   //设置热点的名字和密码
{
    FILE *fp;
    const char *text1 = "interface=wlan0\ndriver=rtl871xdrv\nctrl_interface=/var/run/hostapd\ndtim_period=2\nbeacon_int=100\nchannel=6\nhw_mode=g\nmax_num_sta=8\nap_max_inactivity=300\nieee80211n=1\nauth_algs=1\nwpa=2\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP CCMP\nrsn_pairwise=CCMP\n";
    const char *text2 = "interface=wlan0\ndriver=rtl871xdrv\nctrl_interface=/var/run/hostapd\ndtim_period=2\nbeacon_int=100\nchannel=6\nhw_mode=g\nmax_num_sta=8\nap_max_inactivity=300\nieee80211n=1\nauth_algs=1\n";
    const char *ssid_ch="ssid=";
    const char *password = "wpa_passphrase=";
    const char *enter = "\n";
    int i = 0;
    fp = fopen("/etc/hostapd.conf", "w");
    QByteArray temp = ssid_str.toLatin1();
    const char *ssid_temp = temp.data();
    /*while((ch[i] = getc(fp)) != EOF)
    {
        i++;
    }*/
    fwrite(ssid_ch, 1, strlen(ssid_ch), fp);
    //qDebug() << "strlen(ssid_ch) = " << strlen(ssid_ch);
    fwrite(ssid_temp, 1, strlen(ssid_temp), fp);
    //qDebug()<< "strlen(ssid_temp) = " << strlen(ssid_temp);

    fwrite(enter, 1, 1, fp);
    if(this->password_flag == true)   //需要密码
    {
        temp = hotspot_password.toLatin1();
        const char * password_temp = temp.data();
        fwrite(password, 1, strlen(password), fp);
        fwrite(password_temp, 1, strlen(password_temp), fp);

        fwrite(enter, 1, 1, fp);
        fwrite(text1, 1, strlen(text1), fp);
    }
    else  //不需要密码
    {
        fwrite(text2, 1, strlen(text2), fp);
    }

    fclose(fp);
}

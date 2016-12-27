#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool hotspot_flag;
    bool password_flag;
    void modify_hostapd_conf(QString ssid_str, QString hotspot_password);


private:
    Ui::MainWindow *ui;
private slots:
    void open_hotspot();
    void set_no_password();
    void update_client_list();
};

#endif // MAINWINDOW_H

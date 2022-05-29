#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalSocket>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QLocalSocket socket;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connect_button_clicked();
    void on_compute_button_clicked();
    void on_break_button_clicked();
    void set_message(const QString);
    void send_message();
    void read_message();
    void draw_plot(QVector<double> x, QVector<double> y);

private:
    Ui::MainWindow *ui;
    void setup_initial();
};
#endif // MAINWINDOW_H

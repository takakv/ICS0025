#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <variant>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_initial();
    connect(ui->exit_button, &QPushButton::clicked, this, &QApplication::exit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray get_message_bytes(QString s, bool null_terminate = true, std::vector<std::variant<int, double>> data = {})
{
    QByteArray msg;
    QDataStream ds_msg(&msg, QIODevice::WriteOnly);
    ds_msg.setByteOrder(QDataStream::LittleEndian);

    if (null_terminate)
        s.append(QChar(0));

    ds_msg << s;

    for (auto& item : data)
    {
        if (std::holds_alternative<int>(item))
            ds_msg << std::get<int>(item);
        else
            ds_msg << std::get<double>(item);
    }

    // ds_msg << fn_name writes in front of the function name the length
    // of fn_name using 4 bytes. Therefore, msg already has 4 bytes
    // of buffer in front of the actual contents.

    int msg_size = msg.size();
    QByteArray size_bytes(reinterpret_cast<const char*>(&msg_size), sizeof(int));
    msg.replace(0, 4, size_bytes);

    return msg;
}

void MainWindow::setup_initial()
{
    ui->break_button->setEnabled(false);
    ui->compute_button->setEnabled(false);
    ui->plot_graph->xAxis->setLabel("x");
    ui->plot_graph->yAxis->setLabel("y");
}

void MainWindow::on_connect_button_clicked()
{
    set_message("attempting to connect to server");

    socket.connectToServer("\\\\.\\pipe\\ICS0025");
    if (!socket.waitForConnected())
    {
        set_message(socket.errorString());
        return;
    }

    ui->connect_button->setEnabled(false);
    ui->break_button->setEnabled(true);
    ui->compute_button->setEnabled(true);

    set_message("connected to server");

    connect(&socket, &QLocalSocket::readyRead, this, &MainWindow::read_message);
}

void MainWindow::on_compute_button_clicked()
{
    ui->message->clear();
    send_message();
    if (socket.state() == QLocalSocket::ConnectedState || socket.waitForReadyRead())
        set_message("program is operational");
    else
        set_message("no message received");
}

void MainWindow::on_break_button_clicked()
{
    QByteArray stop_message = get_message_bytes("Stop");
    socket.write(stop_message);
    socket.disconnectFromServer();

    if (socket.state() == QLocalSocket::UnconnectedState || socket.waitForDisconnected())
    {
        set_message("disconnected from server");
        ui->connect_button->setEnabled(true);
        ui->break_button->setEnabled(false);
        ui->compute_button->setEnabled(false);

        disconnect(&socket, &QLocalSocket::readyRead, 0, 0);
    }
    else
        set_message(socket.errorString());
}

void MainWindow::set_message(const QString msg)
{
    ui->message->setText(msg);
}

void MainWindow::send_message()
{
    if (socket.state() == QLocalSocket::UnconnectedState)
    {
        set_message("Could not send message: no active server connection");
        return;
    }

    // Avoid a crash on conversions.
    QValidator* i = new QIntValidator();
    QValidator* d = new QDoubleValidator();

    ui->x0_input->setValidator(d);
    ui->xn_input->setValidator(d);
    ui->points_input->setValidator(i);
    ui->order_input->setValidator(i);

    double x0 = ui->x0_input->text().toDouble();
    double xn = ui->xn_input->text().toDouble();
    int points = ui->points_input->text().toInt();
    int order = ui->order_input->text().toInt();

    std::vector<std::variant<int, double>> data{x0, xn, points, order};
    QByteArray message = get_message_bytes(ui->function_input->currentText(), true, data);

    socket.write(message);
}

void MainWindow::read_message()
{
    QByteArray message_bytes = socket.readAll();

    // No need for the size.
    message_bytes.remove(0, 4);

    QDataStream data(&message_bytes, QIODevice::ReadWrite);
    data.setByteOrder(QDataStream::LittleEndian);

    QString label("Curve");
    QByteArray curve((const char*)label.utf16(), (label.size()+1) * 2);

    if (!message_bytes.contains(curve))
    {
        QString error_message = QString::fromWCharArray(reinterpret_cast<wchar_t*>(message_bytes.data()));
        set_message(error_message);
        return;
    }

    QVector<double> x, y;
    data.skipRawData(12);

    while (!data.atEnd())
    {
        double x_tmp, y_tmp;
        data >> x_tmp >> y_tmp;
        x.push_back(x_tmp);
        y.push_back(y_tmp);
    }

    draw_plot(x, y);
}

void MainWindow::draw_plot(QVector<double> x, QVector<double> y)
{
    ui->plot_graph->addGraph();
    ui->plot_graph->graph(0)->setData(x, y);
    ui->plot_graph->rescaleAxes();
    ui->plot_graph->replot();
}

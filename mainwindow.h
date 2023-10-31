#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QVector>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define KOMMEN "Kommen"
#define GEHEN "Gehen"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_released();

private:
    int GetWeekDay();
    void SetUI();
    void SetBtn();
    void SetTables();
    double CalcSum(int day);
    void SetSums();
    void MarkColumn();
    void Save(QString date, float sum);
    void Load();

    Ui::MainWindow *ui;
    QVector<QTableWidget*> m_qtabledays;
    QVector<QLineEdit*> m_timesums;
    std::vector<std::vector<QDateTime>> m_datetimes;
    std::vector<QDateTime> m_datetimes_mo;
    std::vector<QDateTime> m_datetimes_di;
    std::vector<QDateTime> m_datetimes_mi;
    std::vector<QDateTime> m_datetimes_do;
    std::vector<QDateTime> m_datetimes_fr;
    std::vector<QDateTime> m_datetimes_sa;
    std::vector<QDateTime> m_datetimes_so;
};
#endif // MAINWINDOW_H

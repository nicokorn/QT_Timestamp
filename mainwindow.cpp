#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(750,550);

    m_qtabledays.push_back(ui->tableWidget_Mo);
    m_qtabledays.push_back(ui->tableWidget_Di);
    m_qtabledays.push_back(ui->tableWidget_Mi);
    m_qtabledays.push_back(ui->tableWidget_Do);
    m_qtabledays.push_back(ui->tableWidget_Fr);
    m_qtabledays.push_back(ui->tableWidget_Sa);
    m_qtabledays.push_back(ui->tableWidget_So);

    m_timesums.push_back(ui->lineEdit_Mo);
    m_timesums.push_back(ui->lineEdit_Di);
    m_timesums.push_back(ui->lineEdit_Mi);
    m_timesums.push_back(ui->lineEdit_Do);
    m_timesums.push_back(ui->lineEdit_Fr);
    m_timesums.push_back(ui->lineEdit_Sa);
    m_timesums.push_back(ui->lineEdit_So);

    m_datetimes.push_back(m_datetimes_mo);
    m_datetimes.push_back(m_datetimes_di);
    m_datetimes.push_back(m_datetimes_mi);
    m_datetimes.push_back(m_datetimes_do);
    m_datetimes.push_back(m_datetimes_fr);
    m_datetimes.push_back(m_datetimes_sa);
    m_datetimes.push_back(m_datetimes_so);

    for(int day; day<m_qtabledays.size(); day++)
    {
        m_qtabledays.at(day)->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_qtabledays.at(day)->setFocusPolicy(Qt::NoFocus);
        m_qtabledays.at(day)->setSelectionMode(QAbstractItemView::NoSelection);
        m_qtabledays.at(day)->horizontalHeader()->setSectionsClickable(false);
    }

    Load();

    SetUI();

    SetUITimer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::GetWeekDay()
{
    // get current day for the right column
    int day;
    QDateTime now = QDateTime::currentDateTime();
    day = now.date().dayOfWeek();
    day--;
    return day;
}

void MainWindow::MarkColumn()
{
    // get current day for the right column
    int day = GetWeekDay();

    if(day<0||day>m_qtabledays.count())
    {
        return;
    }

    for (int column = 0; column < m_qtabledays.count(); ++column) {
        if(column==day)
        {
            m_qtabledays.at(column)->setStyleSheet("QHeaderView::section { background-color:rgb(153, 204, 255) }");
            m_timesums.at(column)->setEnabled(true);
            m_qtabledays.at(column)->setEnabled(true);
        }
        else
        {
            m_qtabledays.at(column)->setStyleSheet("QHeaderView::section { background-color:white }");
            m_timesums.at(column)->setEnabled(false);
            m_qtabledays.at(column)->setEnabled(false);
        }
    }
}

void MainWindow::SetUI()
{
    // set col marking
    MarkColumn();

    // refresh ui times
    SetTables();

    // refresh sums
    SetSums();

    // set button text
    SetBtn();
}

bool MainWindow::isWorking()
{
    // get current day for the right column
    int day = GetWeekDay();

    // set button to kommen/gehen text
    int row = m_qtabledays.at(day)->rowCount();
    if(row%2)
    {
        return true;
    }
    return false;
}


void MainWindow::SetBtn()
{
    if(isWorking())
    {
        ui->pushButton->setText(GEHEN);
    }
    else
    {
        ui->pushButton->setText(KOMMEN);
    }
}

void MainWindow::SetUITimer()
{
    // connect timer signal slot
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(UpdateUI()));
    m_timer.start(1000);
}

void MainWindow::SetTables()
{
    // set data on top left ui
    QDateTime now = QDateTime::currentDateTime();
    ui->label->setText("Current date: " + now.toString("yyyy-MM-dd"));

    // set time on the table widgets
    for(int day=0; day<m_datetimes.size(); day++)
    {
        // delete rows
        m_qtabledays.at(day)->setRowCount(0);

        for(int timenr=0; timenr<m_datetimes.at(day).size(); timenr++)
        {
            // write into table
            QString entry;
            if(timenr%2)
            {
                entry = "G, ";
            }
            else
            {
                entry = "K, ";
            }

            entry += m_datetimes.at(day).at(timenr).toString("HH:mm:ss");

            // fill row
            m_qtabledays.at(day)->insertRow(m_qtabledays.at(day)->rowCount());
            m_qtabledays.at(day)->setItem( m_qtabledays.at(day)->rowCount()-1, 0, new QTableWidgetItem(entry));
        }
    }
}

void MainWindow::Save(QString date, float sum)
{
    // open the file which stores the times
    QFile file(FILENAME);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString line = "";
        QStringList copy;


        // get last line
        while(!in.atEnd())
        {
            line = in.readLine();
            copy.append(line);
        }
        file.close();

        // if the file has already lines
        if (line.compare(""))
        {
            QStringList fields = line.split(",");

            // if dates on line is identical with the date to be stored
            if(!fields[0].compare(date))
            {
                if(file.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    // overwrite file with the file copy, execpt for the last line
                    for(int linenr=0; linenr<copy.size()-1; linenr++)
                    {
                        file.write(copy[linenr].toUtf8() + "\n");
                    }

                    // last line is mutated / added with new time stamp
                    QString entry = date+","+QString::number(sum, 'f', 2);

                    int day = GetWeekDay();
                    for(int rownr=0; rownr<m_qtabledays.at(day)->rowCount(); rownr++)
                    {
                        QString row = m_qtabledays.at(day)->item(rownr,0)->text();
                        QStringList fields = row.split(", ");
                        entry += ","+fields[1];
                    }

                    file.write(entry.toUtf8() + "\n");
                    file.close();
                }
            }
            // if not append a new line with new date
            else
            {
                if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
                {
                    QTextStream out(&file);
                    QString entry = date+","+QString::number(sum, 'f', 2);
                    QString row = m_qtabledays.at(GetWeekDay())->item(0,0)->text();
                    QStringList fields = row.split(", ");
                    entry += ","+fields[1];
                    file.write(entry.toUtf8() + "\n");
                    file.close();
                }
            }
        }
        else
        {
            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                QString entry = date+","+QString::number(sum, 'f', 2);
                QString row = m_qtabledays.at(GetWeekDay())->item(0,0)->text();
                QStringList fields = row.split(", ");
                entry += ","+fields[1];
                file.write(entry.toUtf8() + "\n");
                file.close();
            }
        }
    }
    else
    {
        qDebug() << "Error";
    }
}

void MainWindow::Load()
{
    // get current day for the right column
    int day;
    QDateTime now = QDateTime::currentDateTime();
    day = now.date().dayOfWeek();
    day--;

    // open the file which stores the times
    QFile file(FILENAME);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream in(&file);
        QStringList copy;

        // get last line
        while(!in.atEnd())
        {
            copy.append(in.readLine());
        }
        file.close();

        for(int entrynr=copy.size()-1; entrynr>=0; entrynr--)
        {
            // get week off the record
            QStringList contentAll = copy[entrynr].split(",");
            QStringList contentDate = contentAll[0].split("-");
            QDate record(contentDate[0].toInt(),contentDate[1].toInt(),contentDate[2].toInt());
            //record.date().setDate();
            int recordeddWeek = record.weekNumber();

            // get current week
            int currentWeek = now.date().weekNumber();

            // if different start new week pake
            if(currentWeek!=recordeddWeek)
            {
                break;
            }

            // day of the record
            volatile int recordDay = record.dayOfWeek();
            recordDay--;

            // create QDateTime objects
            for(int timenr=2; timenr<contentAll.size(); timenr++)
            {
                // create new QDateTime entry
                QDateTime newentry = QDateTime::fromString(contentAll[0] + "T" + contentAll[timenr], "yyyy-MM-ddThh:mm:ss");

                // add to vector
                m_datetimes.at(recordDay).push_back(newentry);
            }
        }
    }
}

double MainWindow::CalcSum(int day)
{
    // Calculate time
    double sum = 0;
    for(int row=1; row<m_datetimes.at(day).size(); row+=2)
    {
        sum += m_datetimes.at(day).at(row-1).secsTo(m_datetimes.at(day).at(row));
    }

    // show calculated time on ui
    if(sum)
    {
        sum /= 3600;
    }

    return sum;
}

double MainWindow::CalcSumStampedIn()
{
    double sum = 0;
    int day = GetWeekDay();

    for(int row=1; row<m_datetimes.at(day).size(); row+=2)
    {
        sum += m_datetimes.at(day).at(row-1).secsTo(m_datetimes.at(day).at(row));
    }

    // if timestamp number is uneven the use is stamped in
    if(m_datetimes.at(day).size()%2)
    {
        // get current time
        QDateTime now = QDateTime::currentDateTime();
        sum += m_datetimes.at(day).at(m_datetimes.at(day).size()-1).secsTo(now);
    }

    // show calculated time on ui
    if(sum)
    {
        sum /= 3600;
    }

    return sum;
}


void MainWindow::SetSums()
{
    double sum;
    int currentDay;
    QDateTime now = QDateTime::currentDateTime();
    currentDay = now.date().dayOfWeek();
    currentDay--;

    for(int day=0; day<7; day++)
    {
        if(currentDay==day)
        {
            sum = CalcSumStampedIn();
        }
        else
        {
            sum = CalcSum(day);
        }
        m_timesums.at(day)->setText(QString::number(sum, 'f', 2) + " h");
    }
}

void MainWindow::on_pushButton_released()
{
    double sum;
    int day;
    QDateTime now = QDateTime::currentDateTime();
    day = now.date().dayOfWeek();
    day--;

    // reset counter
    m_counter = 0;

    // store time
    m_datetimes.at(day).push_back(now);

    // calculate and set sum for current day
    sum = CalcSum(day);
    m_timesums.at(day)->setText(QString::number(sum, 'f', 2) + " h");

    // set col marking
    MarkColumn();

    // refresh ui times
    SetTables();

    // set button text
    SetBtn();

    // store time into file
    Save(now.toString("yyyy-MM-dd"), sum);
}

void MainWindow::UpdateUI()
{
    SetUI();
}

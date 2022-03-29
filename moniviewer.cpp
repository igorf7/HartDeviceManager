#include "moniviewer.h"
#include "convert.h"

#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief MoniViewer::MoniViewer constructor.
 * @param parent
 */
MoniViewer::MoniViewer(QObject *parent) : QObject(parent)
{
    currentDevice = nullptr;
    pollTimeout = 0;
    chartRangeSec = 300; // sec.
    spinAll = 1150;
    spinPV = 380;

    QStringList stringList;
    int i;

    moniGroupBox = new QGroupBox();
    moniGroupBox->setTitle("Работа с устройствами в режиме мониторинга");
    moniGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* GroupBox "Identificator" */
    QGroupBox* idGroupBox = new QGroupBox("Идентификатор", moniGroupBox);
    QLabel* idLabel[3];
    QHBoxLayout* idLayout = new QHBoxLayout;
    stringList << "Серийный номер" << "Адрес" << "Тип";
    for (i = 0; i < 3; i++) {
        idLabel[i] = new QLabel;
        idLabel[i]->setText(stringList.at(i));
        idValueEdit[i] = new QLineEdit;
        idValueEdit[i]->setReadOnly(true);
        idLayout->addWidget(idLabel[i]);
        idLayout->addWidget(idValueEdit[i], 0, Qt::AlignLeft);
        idLayout->addStretch();
    }
    idGroupBox->setLayout(idLayout);

    /* GroupBox "Parameters" */
    QGroupBox* prmGroupBox = new QGroupBox("Данные", moniGroupBox);
    prmGroupBox->setMaximumWidth(210);

    QLabel* valueLabel[prmNum];
    QVBoxLayout* labelLayout2 = new QVBoxLayout;
    QVBoxLayout* editLayout1 = new QVBoxLayout;
    stringList.clear();
    stringList<<"1-я переменная"<<"2-я переменная"<<"3-я переменная"
                <<"4-я переменная"<<"% диапазона"<<"Ток";
    QVBoxLayout* labelLayout1 = new QVBoxLayout;
    for (i = 0; i < prmNum; i++) {
        valueLabel[i] = new QLabel;
        valueLabel[i]->setText(stringList.at(i));
        labelLayout1->addStretch();
        labelLayout1->addWidget(valueLabel[i]);
        varUnitLabel[i] = new QLabel("");
        varLineEdit[i] = new QLineEdit;
        varLineEdit[i]->setReadOnly(true);
        editLayout1->addStretch();
        editLayout1->addWidget(varLineEdit[i]);
        labelLayout2->addStretch();
        labelLayout2->addWidget(varUnitLabel[i]);
    }
    labelLayout1->addStretch();
    editLayout1->addStretch();
    labelLayout2->addStretch();

    QHBoxLayout* h1Lauout = new QHBoxLayout;
    h1Lauout->addLayout(labelLayout1);
    h1Lauout->addLayout(editLayout1);
    h1Lauout->addLayout(labelLayout2);
    h1Lauout->addStretch();

    startButton = new QPushButton("Старт");
    startButton->setAutoDefault(true);
    QPushButton* stopButton = new QPushButton("Стоп");
    stopButton->setAutoDefault(true);
    QHBoxLayout* h2Lauout = new QHBoxLayout;
    h2Lauout->addWidget(startButton);
    h2Lauout->addWidget(stopButton);

    QLabel* pollTimeLabel = new QLabel("Период опроса, мсек.");
    QHBoxLayout* h3Lauout = new QHBoxLayout;
    pollTimeSpinBox = new QSpinBox();
    pollTimeSpinBox->setMinimum(1000);
    pollTimeSpinBox->setMaximum(30000);
    pollTimeSpinBox->setSingleStep(50);
    pollTimeSpinBox->setValue(spinAll);
    h3Lauout->addWidget(pollTimeLabel);
    h3Lauout->addStretch();
    h3Lauout->addWidget(pollTimeSpinBox);

    QLabel* queueSizeLabel = new QLabel("Пакетов в очереди:");
    queueSizeValue = new QLabel("");
    QFont font;
    font.setItalic(true);
    queueSizeLabel->setFont(font);
    queueSizeValue->setFont(font);
    QHBoxLayout* h4Lauout = new QHBoxLayout;
    h4Lauout->addWidget(queueSizeLabel);
    h4Lauout->addWidget(queueSizeValue);

    pvOnlyCheckBox = new QCheckBox("Только 1-я переменная");
    pvOnlyCheckBox->setChecked(false);

    QVBoxLayout* prmLayout = new QVBoxLayout;
    prmLayout->addWidget(pvOnlyCheckBox);
    prmLayout->addLayout(h1Lauout);
    prmLayout->addLayout(h3Lauout);
    prmLayout->addLayout(h2Lauout);
    prmLayout->addLayout(h4Lauout);
    prmGroupBox->setLayout(prmLayout);

    /* GroupBox "Chart" */
    QGroupBox* chartGroupBox = new QGroupBox("График", moniGroupBox);
    series = new QLineSeries(this);
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Основной параметр");
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chart->setAnimationOptions(QChart::AllAnimations);
    axisX = new QDateTimeAxis(this);
    startDateTime = QDateTime::currentDateTime();
    nextDateTime = startDateTime.addSecs(chartRangeSec);
    axisX->setRange(startDateTime, nextDateTime);
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("Время");
    axisY = new QValueAxis(this);
    axisY->setRange(0, 100);
    axisY->setTitleText("Значение");
    axisY->setTickCount(9);
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    QVBoxLayout* chartLayout = new QVBoxLayout;
    chartLayout->addWidget(chartView);
    chartGroupBox->setLayout(chartLayout);

    /*  */
    QVBoxLayout* commonLayout = new QVBoxLayout;
    QHBoxLayout* monitorLayout = new QHBoxLayout;
    commonLayout->addWidget(idGroupBox);
    monitorLayout->addWidget(prmGroupBox);
    monitorLayout->addWidget(chartGroupBox);
    commonLayout->addLayout(monitorLayout);
    moniGroupBox->setLayout(commonLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(onStartButton_clicked()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(onStopButton_clicked()));
    connect(pvOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(onPvOnlyToggled(bool)));
    connect(pollTimeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

    if (currentDevice != nullptr) {
        emit sendCommand(COMMAND_15, nullptr); // to get high and low PV limits
        emit clearPacketQueue();
    }
    else {
        pvLowLim = qQNaN();
        pvHighLim = qQNaN();
    }
    queueSizeValue->setText("");
}

/**
 * @brief MoniViewer::~MoniViewer destructor.
 */
MoniViewer::~MoniViewer()
{    
    delete moniGroupBox;
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief MoniViewer::onPortError
 * @param msg
 */
void MoniViewer::onPortError()
{
    onStopButton_clicked();
}

/**
 * @brief MoniViewer::onSelectDevice
 * @param list - pointer to the device list
 * @param index - device index in the list
 */
void MoniViewer::onSelectDevice(Device* device)
{
    currentDevice = device;
    emit setDeviceAddress(currentDevice->getFullAddress());

    idValueEdit[serial]->setText(QString::number(currentDevice->getSerial()));
    idValueEdit[address]->setText("0x"+QString::number(currentDevice->getFullAddress(), 16).toUpper());

    quint16 utmp = currentDevice->getTypeCode();
    idValueEdit[type]->setText(currentDevice->getTypeStr(utmp)+
                           "   (0x"+QString::number(utmp, 16).toUpper()+")");

    emit sendCommand(COMMAND_15, nullptr); // for to get high and low PV limits

    if (pollTimeout != 0) {
        killTimer(pollTimeout);
        pollTimeout = 0;
    }

    startButton->setEnabled(true);
}

/**
 * @brief MoniViewer::onStartButton_clicked
 */
void MoniViewer::onStartButton_clicked()
{
    if (currentDevice == nullptr) {
//        QMessageBox::warning(moniGroupBox, tr("Не выбрано устройство"),
//                             "Выберите устройство в списке слева.");
        return;
    }

    startDateTime = QDateTime::currentDateTime();
    nextDateTime = startDateTime.addSecs(chartRangeSec);
    axisX->setRange(startDateTime, nextDateTime);
    axisY->setRange(pvLowLim, pvHighLim);

    for (int i = 0; i < prmNum; i++) {
        varLineEdit[i]->setText("");
        varUnitLabel[i]->setText("");
    }

    pollingTime = pollTimeSpinBox->value();
    pollTimeout = startTimer(pollingTime);
    startButton->setEnabled(false);
}

/**
 * @brief MoniViewer::onStopButton_clicked
 */
void MoniViewer::onStopButton_clicked()
{
    if (pollTimeout != 0) {
        killTimer(pollTimeout);
        pollTimeout = 0;
    }
    startButton->setEnabled(true);
    emit sendCommand(STOP_CMD, nullptr);
}

/**
 * @brief ConfigViewer::onTransactionComplete
 * @param response - response code
 * @param prm2 - extra parameter
 */
void MoniViewer::onTransactionComplete(quint8 response, const QVariant &prm2)
{
    QByteArray tmpBuffer;
    QString str;    
    QDateTime dateTimeNow;

    switch(response)
    {
        case COMMAND_1:
            tmpBuffer.append(prm2.toByteArray());
            varUnitLabel[pv_unit]->setText(currentDevice->getUnitStr(static_cast<quint8>(tmpBuffer[0])));
            pvValue = static_cast<double>(ByteArrayToFloat(tmpBuffer, 1));
            varLineEdit[pv]->setText(str.sprintf("%.3f", pvValue));
            break;
        case COMMAND_2:
            if (pvOnlyCheckBox->isChecked()) break;
            tmpBuffer.append(prm2.toByteArray());
            varLineEdit[current]->setText(str.sprintf("%.2f", static_cast<double>(ByteArrayToFloat(tmpBuffer, 0))));
            varLineEdit[percent]->setText(str.sprintf("%.1f", static_cast<double>(ByteArrayToFloat(tmpBuffer, 4))));
            varUnitLabel[curr_unit]->setText("mA");
            varUnitLabel[perc_unit]->setText("%");
            break;
        case COMMAND_9:
            if (pvOnlyCheckBox->isChecked()) break;
            tmpBuffer.append(prm2.toByteArray());
            pvValue = static_cast<double>(ByteArrayToFloat(tmpBuffer, 4));
            svValue = static_cast<double>(ByteArrayToFloat(tmpBuffer, 12));
            tvValue = static_cast<double>(ByteArrayToFloat(tmpBuffer, 20));
            qvValue = static_cast<double>(ByteArrayToFloat(tmpBuffer, 28));
            varLineEdit[pv]->setText(str.sprintf("%.3f", pvValue));
            varLineEdit[sv]->setText(str.sprintf("%.3f", svValue));
            varLineEdit[tv]->setText(str.sprintf("%.3f", tvValue));
            varLineEdit[qv]->setText(str.sprintf("%.3f", qvValue));
            varUnitLabel[pv_unit]->setText(currentDevice->getUnitStr(static_cast<quint8>(tmpBuffer[3])));
            varUnitLabel[sv_unit]->setText(currentDevice->getUnitStr(static_cast<quint8>(tmpBuffer[11])));
            varUnitLabel[tv_unit]->setText(currentDevice->getUnitStr(static_cast<quint8>(tmpBuffer[19])));
            varUnitLabel[qv_unit]->setText(currentDevice->getUnitStr(static_cast<quint8>(tmpBuffer[27])));
            break;
        case COMMAND_15:
            tmpBuffer.append(prm2.toByteArray());
            pvHighLim = static_cast<qreal>(ByteArrayToFloat(tmpBuffer, 3));
            pvLowLim = static_cast<qreal>(ByteArrayToFloat(tmpBuffer, 7));
            break;
        default:
            break;
    }
    /* Add data to the chart */
    if ((response == COMMAND_9) || (response == COMMAND_1)) {
        dateTimeNow = QDateTime::currentDateTime();
        if (dateTimeNow >= nextDateTime) {
            nextDateTime = dateTimeNow.addSecs(chartRangeSec);
            qreal x = chart->plotArea().width();
            chart->scroll(x, 0);
        }
        series->append(dateTimeNow.toMSecsSinceEpoch(), pvValue);
    }
}

/**
 * @brief MoniViewer::onShowPacketSize
 */
void MoniViewer::onShowPacketSize(int size)
{
    queueSizeValue->setText(QString::number(size));
}

/**
 * @brief MoniViewer::onPvOnlyToggled
 */
void MoniViewer::onPvOnlyToggled(bool checked)
{
    onStopButton_clicked();

    pollTimeSpinBox->blockSignals(true);

    if (!checked) {
        pollTimeSpinBox->setMinimum(1000);
        pollTimeSpinBox->setMaximum(10000);
        pollTimeSpinBox->setSingleStep(50);
        pollTimeSpinBox->setValue(spinAll);

    }
    else {
        pollTimeSpinBox->setMinimum(350);
        pollTimeSpinBox->setMaximum(1000);
        pollTimeSpinBox->setSingleStep(10);
        pollTimeSpinBox->setValue(spinPV);
    }
    pollTimeSpinBox->blockSignals(false);
    onStartButton_clicked();
}

/**
 * @brief MoniViewer::onValueChanged
 * @param i
 */
void MoniViewer::onValueChanged(int i)
{
    if (pvOnlyCheckBox->isChecked()) {
        spinPV = i;
    }
    else {
        spinAll = i;
    }
}

/**
 * @brief MoniViewer::onResetDeviceData
 */
void MoniViewer::onResetDeviceData()
{
    for (int i = 0; i < prmNum; i++) {
        varLineEdit[i]->setText("");
    }
    for (int i = 0; i < prmNum; i++) {
        varUnitLabel[i]->setText("");
    }
    for (int i = 0; i < idNum; i++) {
        idValueEdit[i]->setText("");
    }
}

/**
 * @brief HartPro::timerEvent. Handles timer events.
 * @param event.  Timer event
 */
void MoniViewer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == pollTimeout) {
        if (pollTimeout != 0) {
            killTimer(pollTimeout);
            pollTimeout = 0;
        }
        QByteArray cmdData;
        for (char i = 0; i < 4; i++) {
            cmdData.append(i);
        }

        if (pvOnlyCheckBox->isChecked()) {
            emit sendCommand(COMMAND_1, nullptr);
        }
        else {
            emit sendCommand(COMMAND_9, cmdData);
            emit sendCommand(COMMAND_2, nullptr);
        }
        pollingTime = pollTimeSpinBox->value();
        pollTimeout = startTimer(pollingTime);
    }
}
//eof

#ifndef MONIVIEWER_H
#define MONIVIEWER_H

#include "hart.h"

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QTime>
#include <QDateTimeAxis>
#include "device.h"

QT_CHARTS_USE_NAMESPACE

class MoniViewer : public QObject
{
    Q_OBJECT
public:
    explicit MoniViewer(QObject *parent = nullptr);
    ~MoniViewer();
    QGroupBox* moniGroupBox;

private:
    Device* currentDevice;
    QChart* chart;
    QLineSeries* series;

    static const quint8 prmNum = 6;
    static const quint8 idNum = 3;
    QLineEdit* idValueEdit[idNum];
    QLineEdit* varLineEdit[prmNum];
    QLabel* varUnitLabel[prmNum];
    QSpinBox* pollTimeSpinBox;
    QPushButton* startButton;
    QLabel* queueSizeValue;
    QCheckBox* pvOnlyCheckBox;

    QDateTimeAxis* axisX;
    QValueAxis* axisY;
    QDateTime startDateTime, nextDateTime;
    quint32 chartRangeSec;
    qreal pvLowLim, pvHighLim,
          pvValue, svValue, tvValue, qvValue;
    int spinPV, spinAll;

    enum{
        serial = 0,
        address,
        type,
        pv = 0,
        sv,
        tv,
        qv,
        percent,
        current,
        pv_unit = 0,
        sv_unit,
        tv_unit,
        qv_unit,
        perc_unit,
        curr_unit
    }widgetIndex;

protected:
    int pollingTime;
    int pollTimeout;
    void timerEvent(QTimerEvent *event);
    bool isPortOpened();

signals:
    void sendCommand(HartCommand_t cmd, const QByteArray &cmd_data);
    void clearPacketQueue();
    void setDeviceAddress(quint64 addr);

public slots:
    void onPortError();
    void onShowPacketSize(int size);
    void onTransactionComplete(quint8 response, const QVariant &prm2);
    void onSelectDevice(Device *device);
    void onResetDeviceData();

private slots:
    void onStartButton_clicked();
    void onStopButton_clicked();
    void onPvOnlyToggled(bool);
    void onValueChanged(int i);
};

#endif // MONIVIEWER_H

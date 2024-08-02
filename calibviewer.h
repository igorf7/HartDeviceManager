#ifndef CALIBVIEWER_H
#define CALIBVIEWER_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>

class CalibViewer : public QObject
{
    Q_OBJECT
public:
    explicit CalibViewer(QObject *parent = nullptr);
    ~CalibViewer();
    QGroupBox* calibGroupBox;
    //int myTabIndex;

private:    
    //int currentTabIndex;

signals:

public slots:
    void onResetDeviceData();

private slots:

};
#endif // CALIBVIEWER_H

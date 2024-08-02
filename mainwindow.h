#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "port.h"
#include "hart.h"
#include "hartpro.h"
#include "loadviewer.h"
#include "configviewer.h"
#include "calibviewer.h"
#include "moniviewer.h"
#include <QMainWindow>
#include <QComboBox>
#include <QListWidget>
#include <QListView>
#include <QPointer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QList<Device*> deviceListPointer;

private:
    ComPort* comPort = nullptr;
    QThread* threadPort;
    HartPro* hartPro = nullptr;
    QGroupBox* listGroupBox;
    QAction* primaryAct;
    QAction* secondaryAct;
    QTabWidget* mainTabWidget;
    QPushButton* connPushButton;
    QPushButton* findPushButton;
    QProgressBar* findBar;
    QComboBox* portComboBox;
    QListWidget* listWidget;

    QPointer<LoadViewer> loadViewer;
    QPointer<ConfigViewer> configViewer;
    QPointer<CalibViewer> calibViewer;
    QPointer<MoniViewer> moniViewer;
    QPointer<QVBoxLayout> tabLayout;

    int currentTabIndex = 0;

    enum{
        LOADER_TAB_INDEX = 0,
        CONFIG_TAB_INDEX,
        CALIBR_TAB_INDEX,
        MONITOR_TAB_INDEX
    }mainTabIndexes;

signals:
    void connectClicked(const QString &port);
    void disconnectClicked();
    void sendCommand(HartCommand_t cmd, const QByteArray &cmd_data);
    void selectDevice(Device* device);
    void clearPacketQueue();
    void resetDeviceData();
    void setMainWidget(QWidget* widget);

public slots:
    void onPortError(const QString &msg);
    void onPortOpened(const QString &str);
    void onPortClosed();
    void updatePortList();
    void onShowPolligProgress(quint8 val);
    void onSearchCompleted();
    void onFillTableWidget(const QList<Device*> &list);
    void onWriteStatusBar(const QString &str, int timeout);

private slots:
    void createFileAct_triggered();
    void openFileAct_triggered();
    void onQuitAct_triggered();
    void onPrimaryAct_triggered();
    void onPrimaryAct_toggled(bool checked);
    void onSecondaryAct_triggered();
    void onReset_triggered();
    void onSelfDiaAct_triggered();
    void onHardReset_triggered();
    void onManual_triggered();
    void onAbout_triggered();
    void onConnPushButton_clicked();
    void onFindPushButton_clicked();
    void onItemSelectionChanged();
    void onMainTabWidget_currentChanged(int index);
};

#endif // MAINWINDOW_H

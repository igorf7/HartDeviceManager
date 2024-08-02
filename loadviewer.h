#ifndef LOADVIEWER_H
#define LOADVIEWER_H

#include "hart.h"

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPointer>

class LoadViewer : public QObject
{
    Q_OBJECT
public:
    explicit LoadViewer(QObject *parent = nullptr);
    ~LoadViewer();
    QGroupBox* loadGroupBox;

private:
    void createLoadStateWindow();

    QPointer<QDialog> loadStateWindow;
    QLineEdit* serialLineEdit;
    QCheckBox* incCheckBox;
    QString fileName;
    QByteArray fwBuffer;
    QProgressBar* writeBar;
    quint32 serialNumber;
    quint32 fwHashSum;
    bool isLoadingComplete;

signals:
    void sendCommand(HartCommand_t cmd, const QByteArray &cmd_data);

public slots:
    void onEraseComplete();
    void onTransactionComplete(quint8 response, const QVariant &prm2);
    void onLoadingComplete();

private slots:
    void onFopenButton_clicked();
    void onCheckMemButton_clicked();
    void onloadStartButton_clicked();
    void onAppStartButton_clicked();
    void onLoadWinCloseButton_clicked();
    void onDialogFinished(int code);
};

#endif // LOADVIEWER_H

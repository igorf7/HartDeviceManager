#include "calibviewer.h"

#include <QDebug>

/**
 * @brief CalibViewer::CalibViewer constructor
 * @param parent
 */
CalibViewer::CalibViewer(QObject *parent) : QObject(parent)
{
    calibGroupBox = new QGroupBox();
    calibGroupBox->setTitle("Работа с устройствами в режиме калибровки");
    calibGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * @brief CalibViewer::~CalibViewer destructor
 */
CalibViewer::~CalibViewer()
{
    delete calibGroupBox;
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief CalibViewer::onResetDeviceData
 */
void CalibViewer::onResetDeviceData()
{
}
//eof

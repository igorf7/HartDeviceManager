#include "calibviewer.h"

//#include <QDebug>

/**
 * @brief CalibViewer::CalibViewer constructor
 * @param parent
 */
CalibViewer::CalibViewer(QObject *parent) : QObject(parent)
{
    calibGroupBox = new QGroupBox();
    calibGroupBox->setTitle(tr("Working with devices in calibration mode"));
    calibGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * @brief CalibViewer::~CalibViewer destructor
 */
CalibViewer::~CalibViewer()
{
    delete calibGroupBox;
    //qDebug()<<"~By-by from"<<this;
}

/**
 * @brief CalibViewer::onResetDeviceData
 */
void CalibViewer::onResetDeviceData()
{
}
//eof

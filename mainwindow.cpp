#include "mainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QStatusBar>
#include <QSerialPortInfo>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QApplication>
#include <QDesktopWidget>

/**
 * @brief constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* Main window widget */
    QWidget* mainWidget = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

    setWindowIcon(QIcon(":/images/alc64x64_32.png"));

    /* Create menu bar */
    QAction* createFileAct = new QAction(tr("&Создать"), this);
    connect(createFileAct, SIGNAL(triggered()), this, SLOT(createFileAct_triggered()));
    QAction* openFileAct = new QAction(tr("&Открыть"), this);
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFileAct_triggered()));
    QAction* quitAct = new QAction(tr("&Выход"), this);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(onQuitAct_triggered()));
    QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));
    openFileAct->setEnabled(false);
    createFileAct->setEnabled(false);
    fileMenu->addAction(createFileAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    primaryAct = new QAction(tr("&Первичный мастер"), this);
    primaryAct->setCheckable(true);
    primaryAct->setChecked(true);
    connect(primaryAct, SIGNAL(triggered()), this, SLOT(onPrimaryAct_triggered()));
    connect(primaryAct, SIGNAL(toggled(bool)), this, SLOT(onPrimaryAct_toggled(bool)));
    secondaryAct = new QAction(tr("&Вторичный мастер"), this);
    secondaryAct->setCheckable(true);
    secondaryAct->setChecked(false);
    connect(secondaryAct, SIGNAL(triggered()), this, SLOT(onSecondaryAct_triggered()));
    QMenu* optionsMenu = menuBar()->addMenu(tr("&Параметры"));
    optionsMenu->addAction(primaryAct);
    optionsMenu->addAction(secondaryAct);

    QAction* resetAct = new QAction(tr("&Рестарт"), this);
    connect(resetAct, SIGNAL(triggered()), this, SLOT(onReset_triggered()));
    QAction* selfDiaAct = new QAction(tr("&Самодиагностика"), this);
    connect(selfDiaAct, SIGNAL(triggered()), this, SLOT(onSelfDiaAct_triggered()));
    QAction* hardResetAct = new QAction(tr("&Аппаратный сброс"), this);
    connect(hardResetAct, SIGNAL(triggered()), this, SLOT(onHardReset_triggered()));
    QMenu* commandsMenu = menuBar()->addMenu(tr("&Команды"));
    commandsMenu->addAction(resetAct);
    commandsMenu->addAction(selfDiaAct);
    commandsMenu->addAction(hardResetAct);

    QAction* manualAct = new QAction(tr("&Инструкция"), this);
    connect(manualAct, SIGNAL(triggered()), this, SLOT(onManual_triggered()));
    QAction* aboutAct = new QAction(tr("&О программе"), this);    
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(onAbout_triggered()));
    QMenu* helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpMenu->addAction(manualAct);
    helpMenu->addAction(aboutAct);

    /* Activate status bar */
    QFont font;
    font.setBold(true);
    font.setItalic(true);
    statusBar()->setFont(font);
    statusBar()->showMessage("Порт закрыт");

    /* Setup left panel group widgets */
    QWidget* leftPanel = new QGroupBox(mainWidget); // container for left panel
///    leftPanel->setFixedWidth(255);

    portComboBox = new QComboBox;
    connPushButton = new QPushButton("Открыть");
    connPushButton->setAutoDefault(true);
    connect(connPushButton, SIGNAL(clicked()), this, SLOT(onConnPushButton_clicked()));

    QHBoxLayout* connHlayout1 = new QHBoxLayout;    
    connHlayout1->addWidget(portComboBox);
    connHlayout1->addWidget(connPushButton);        
    QGroupBox* portGroupBox= new QGroupBox("Соединение");
    portGroupBox->setFlat(true);
    portGroupBox->setAlignment(Qt::AlignCenter);
    portGroupBox->setLayout(connHlayout1);

    listGroupBox = new QGroupBox("Список устройств");
    listGroupBox->setFlat(true);
    listGroupBox->setAlignment(Qt::AlignCenter);
    QVBoxLayout* listVLayout = new QVBoxLayout;
    listWidget = new QListWidget();
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    findPushButton = new QPushButton("Поиск устройств");
    findPushButton->setAutoDefault(true);
    connect(findPushButton, SIGNAL(clicked()), this, SLOT(onFindPushButton_clicked()));
    findBar = new QProgressBar;
    findBar->setMinimum(0);
    findBar->setMaximum(63);
    findBar->setValue(0);
    findBar->setFixedHeight(15);
    findBar->setVisible(false);
    listVLayout->addWidget(listWidget);
    listVLayout->addWidget(findBar);
    listVLayout->addWidget(findPushButton);
    listGroupBox->setLayout(listVLayout);

    QVBoxLayout* leftVLayout = new QVBoxLayout;
    leftVLayout->addWidget(portGroupBox);
    leftVLayout->addWidget(listGroupBox);
    leftPanel->setLayout(leftVLayout);

    /* Setup Tab widgets */
    mainTabWidget = new QTabWidget(mainWidget); // container for the viewers
    QWidget* loaderTab = new QWidget;   // loader viewer
    QWidget* configTab = new QWidget;   // configuration viewer
    QWidget* calibTab = new QWidget;    // calibration viewer
    QWidget* moniTab = new QWidget;     // monitor viewer
    mainTabWidget->addTab(loaderTab, "Загрузчик");
    mainTabWidget->addTab(configTab, "Конфигурация");
    mainTabWidget->addTab(calibTab, "Калибровка");
    mainTabWidget->addTab(moniTab, "Монитор");

    /* Set mainWidget as central widget */
///    mainLayout->addWidget(leftPanel, 0, Qt::AlignLeft);
///    mainLayout->addWidget(mainTabWidget);
///    mainWidget->setLayout(mainLayout);
    QSplitter *splitter = new QSplitter(mainWidget);
    splitter->addWidget(leftPanel);
    splitter->addWidget(mainTabWidget);
    mainLayout->addWidget(splitter);
    setCentralWidget(mainWidget);

    QList<int> defaultSizes;
    defaultSizes << 255 << 600;
    splitter->setSizes(defaultSizes);

    /* HartPro class instance */
    hartPro = new HartPro(this);

    connect(mainTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onMainTabWidget_currentChanged(int)));

    /* Create ComPort thread */
    comPort = new ComPort();
    threadPort = new QThread;
    threadPort->setObjectName("threadPort");
    comPort->moveToThread(threadPort);
    comPort->serialPort.moveToThread(threadPort);

    /* Connect signals with slots */
    QObject::connect(threadPort, &QThread::started, comPort, &ComPort::onPortStart);
    QObject::connect(comPort, &ComPort::quitComPort, threadPort, &QThread::quit);
    QObject::connect(comPort, &ComPort::quitComPort, threadPort, &QThread::deleteLater);
    QObject::connect(threadPort, &QThread::finished, comPort, &ComPort::deleteLater);
    QObject::connect(comPort, &ComPort::portError, this, &MainWindow::onPortError);
    QObject::connect(comPort, &ComPort::portError, hartPro, &HartPro::onPortError);
    QObject::connect(comPort, &ComPort::portOpened, this, &MainWindow::onPortOpened);
    QObject::connect(comPort, &ComPort::portClosed, this, &MainWindow::onPortClosed);
    QObject::connect(this, &MainWindow::connectClicked, comPort, &ComPort::onConnectClicked);
    QObject::connect(this, &MainWindow::disconnectClicked, comPort, &ComPort::onDisconnectClicked);
    QObject::connect(comPort, &ComPort::parsingPacket, hartPro, &HartPro::onParsingPacket);
    QObject::connect(hartPro, &HartPro::sendDataPacket, comPort, &ComPort::onSendDataPacket);
    QObject::connect(this, &MainWindow::sendCommand, hartPro, &HartPro::onSendCommand);
    QObject::connect(hartPro, &HartPro::showPolligProgress, this, &MainWindow::onShowPolligProgress);
    QObject::connect(hartPro, &HartPro::searchCompleted, this, &MainWindow::onSearchCompleted);
    QObject::connect(hartPro, &HartPro::fillTableWidget, this, &MainWindow::onFillTableWidget);
    QObject::connect(hartPro, &HartPro::writeStatusBar, this, &MainWindow::onWriteStatusBar);
    QObject::connect(this, &MainWindow::clearPacketQueue, hartPro, &HartPro::onClearPacketQueue);
    QObject::connect(this, &MainWindow::setMainWidget, hartPro, &HartPro::onSetMainWidget);

    emit setMainWidget(mainWidget);

    threadPort->start(); // Start ComPort thread
    this->updatePortList();

    this->setWindowTitle("HART® Device Manager");
    this->onMainTabWidget_currentChanged(LOADER_TAB_INDEX);
}

/**
 * @brief destructor
 */
MainWindow::~MainWindow()
{        
    if(loadViewer) delete loadViewer;
    if(configViewer) delete configViewer;
    if(calibViewer) delete calibViewer;
    if(moniViewer) delete moniViewer;
    if(tabLayout) delete tabLayout;
    threadPort->quit();
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief Tab change event handler
 * @param index - tab index
 */
void MainWindow::onMainTabWidget_currentChanged(int index)
{
    if( findPushButton->text() != "Поиск устройств" ){
        if( mainTabWidget->currentIndex() == LOADER_TAB_INDEX ){
            mainTabWidget->setCurrentIndex(currentTabIndex);
            return;
        }
    }

    if( loadViewer ){
        delete loadViewer;  // Delete LoadViewer class instance if it exists
    }
    if( configViewer ){
        delete configViewer; // Delete ConfigViewer class instance if it exists
    }
    if( calibViewer ){
        delete calibViewer; // Delete CalibViewer class instance if it exists
    }
    if( moniViewer ){
        delete moniViewer; // Delete MoniViewer class instance if it exists
    }
    if( tabLayout ){
        delete tabLayout; // Delete layout if it exists
    }

    tabLayout = new QVBoxLayout; // Create layout
    findPushButton->setEnabled(true);
    for(int i = 0; i < listWidget->count(); i++){
        listWidget->setRowHidden(i, false);
    }

    switch(index)
    {
    case LOADER_TAB_INDEX:
        for(int i = 0; i < listWidget->count(); i++){
            listWidget->setRowHidden(i, true);
        }
        findPushButton->setEnabled(false);
        loadViewer = new LoadViewer; // Create LoadViewer class instance
        tabLayout->addWidget(loadViewer->loadGroupBox);
        mainTabWidget->widget(LOADER_TAB_INDEX)->setLayout(tabLayout);
        QObject::connect(loadViewer, &LoadViewer::sendCommand, hartPro, &HartPro::onSendCommand);
        QObject::connect(hartPro, &HartPro::eraseComplete, loadViewer, &LoadViewer::onEraseComplete);
        QObject::connect(hartPro, &HartPro::transactionComplete, loadViewer, &LoadViewer::onTransactionComplete);
        QObject::connect(hartPro, &HartPro::loadingComplete, loadViewer, &LoadViewer::onLoadingComplete);
        currentTabIndex = LOADER_TAB_INDEX;
        break;
    case CONFIG_TAB_INDEX:
        configViewer = new ConfigViewer; // Create ConfigViewer class instance
        tabLayout->addWidget(configViewer->confGroupBox);
        mainTabWidget->widget(CONFIG_TAB_INDEX)->setLayout(tabLayout);
        QObject::connect(configViewer, &ConfigViewer::sendCommand, hartPro, &HartPro::onSendCommand);
        QObject::connect(hartPro, &HartPro::transactionComplete, configViewer, &ConfigViewer::onTransactionComplete);
        QObject::connect(this, &MainWindow::selectDevice, configViewer, &ConfigViewer::onSelectDevice);
        QObject::connect(configViewer, &ConfigViewer::setDeviceAddress, hartPro, &HartPro::onSetDeviceAddress);
        QObject::connect(configViewer, &ConfigViewer::clearPacketQueue, hartPro, &HartPro::onClearPacketQueue);
        QObject::connect(this, &MainWindow::resetDeviceData, configViewer, &ConfigViewer::onResetDeviceData);
        currentTabIndex = CONFIG_TAB_INDEX;
        break;
    case CALIBR_TAB_INDEX:
        calibViewer = new CalibViewer; // Create CalibViewer class instance
        tabLayout->addWidget(calibViewer->calibGroupBox);
        mainTabWidget->widget(CALIBR_TAB_INDEX)->setLayout(tabLayout);
        QObject::connect(this, &MainWindow::resetDeviceData, calibViewer, &CalibViewer::onResetDeviceData);
        currentTabIndex = CALIBR_TAB_INDEX;
        break;
    case MONITOR_TAB_INDEX:
        moniViewer = new MoniViewer; // Create MoniViewer class instance
        tabLayout->addWidget(moniViewer->moniGroupBox);
        mainTabWidget->widget(MONITOR_TAB_INDEX)->setLayout(tabLayout);
        if( comPort != nullptr ){
            QObject::connect(comPort, &ComPort::portError, moniViewer, &MoniViewer::onPortError);
            QObject::connect(this, &MainWindow::disconnectClicked, moniViewer, &MoniViewer::onPortError);
        }
        QObject::connect(moniViewer, &MoniViewer::setDeviceAddress, hartPro, &HartPro::onSetDeviceAddress);
        QObject::connect(moniViewer, &MoniViewer::sendCommand, hartPro, &HartPro::onSendCommand);
        QObject::connect(hartPro, &HartPro::transactionComplete, moniViewer, &MoniViewer::onTransactionComplete);
        QObject::connect(this, &MainWindow::selectDevice, moniViewer, &MoniViewer::onSelectDevice);
        QObject::connect(moniViewer, &MoniViewer::clearPacketQueue, hartPro, &HartPro::onClearPacketQueue);
        QObject::connect(hartPro, &HartPro::showPacketSize, moniViewer, &MoniViewer::onShowPacketSize);
        QObject::connect(this, &MainWindow::resetDeviceData, moniViewer, &MoniViewer::onResetDeviceData);
        currentTabIndex = MONITOR_TAB_INDEX;
        break;
    default:
        qDebug()<<"New Tab?";
        break;
    }
    mainTabWidget->widget(index)->setLayout(tabLayout);

    if( !deviceListPointer.isEmpty() ){
        if( listWidget->currentItem() )
            emit selectDevice(deviceListPointer.at(listWidget->currentRow()));
    }
}

void MainWindow::onPortError(const QString &msg)
{
    QMessageBox::warning(this, tr("Ошибка"), msg);
}

/**
 * @brief MainWindow::updatePortList
 */
void MainWindow::updatePortList()
{
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    while(infoList.size() > 0) {
        QSerialPortInfo info = infoList.takeFirst();
        portComboBox->addItem(info.portName());
    }
}

/**
 * @brief MainWindow::onPortOpened
 * @param str - port name
 */
void MainWindow::onPortOpened(const QString &str)
{
    connPushButton->setText("Закрыть");
    qDebug() << "Port opened";
    statusBar()->showMessage("Порт открыт: " + str);
}

/**
 * @brief MainWindow::onPortClosed
 */
void MainWindow::onPortClosed()
{
    connPushButton->setText("Открыть");
    qDebug() << "Port closed";

    statusBar()->showMessage("Порт закрыт");
}

/**
 * @brief MainWindow::onWriteStatusBar
 * @param str
 * @param cmd
 */
void MainWindow::onWriteStatusBar(const QString &str, int timeout)
{
    statusBar()->showMessage(str, timeout);
}

/**
 * @brief MainWindow::onConnPushButton_clicked
 */
void MainWindow::onConnPushButton_clicked()
{
    if( connPushButton->text() != "Открыть" ) {
        emit disconnectClicked();
    }
    else{
        emit connectClicked(portComboBox->currentText());
    }
}

/**
 * @brief MainWindow::onFindPushButton_clicked
 */
void MainWindow::onFindPushButton_clicked()
{
    if( connPushButton->text() == "Открыть" ){
        QMessageBox::warning(this, tr("Порт закрыт"), "Откройте порт.");
    }
    else{
        if( findPushButton->text() == "Поиск устройств" ){
            listWidget->blockSignals(true);
            listWidget->clear();
            deviceListPointer.clear();
            findPushButton->setText("Остановить поиск");
            connPushButton->setEnabled(false);
            findBar->setVisible(true);
            emit clearPacketQueue();
            emit resetDeviceData();
            listWidget->blockSignals(false);
            statusBar()->showMessage("Выполняется поиск устройств...");
            for(int i = 0; i < DEV_MAX_NUMBER; i++){
                emit sendCommand(COMMAND_0, nullptr);
            }
        }
        else{
            findPushButton->setText("Поиск устройств");
            findBar->setValue(0);
            findBar->setVisible(false);
            if( listWidget->count() == 0 ){
                listWidget->addItem("Устройств не обнаружено");
            }
            connPushButton->setEnabled(true);
            statusBar()->showMessage("Поиск устройств остановлен", 3000);
            emit sendCommand(STOP_CMD, nullptr);
        }
    }
}

/**
 * @brief MainWindow::onOpt1_triggered
 */
void MainWindow::createFileAct_triggered()
{

}

/**
 * @brief MainWindow::onOpt2_triggered
 */
void MainWindow::openFileAct_triggered()
{

}

/**
 * @brief MainWindow::onOpt3_triggered
 */
void MainWindow::onQuitAct_triggered()
{
    close();
}

/**
 * @brief MainWindow::onPrimaryAct_triggered
 */
void MainWindow::onPrimaryAct_triggered()
{
    secondaryAct->setChecked(false);
}

/**
 * @brief MainWindow::onPrimaryAct_toggled
 */
void MainWindow::onPrimaryAct_toggled(bool checked)
{
    if( checked ){
        hartPro->masterType = PRIMARY_MASTER;
    }
    else{
        hartPro->masterType = SECONDARY_MASTER;
    }

}

/**
 * @brief MainWindow::onSecondaryAct_triggered
 */
void MainWindow::onSecondaryAct_triggered()
{
    primaryAct->setChecked(false);
}

/**
 * @brief MainWindow::onReset_triggered
 */
void MainWindow::onReset_triggered()
{
    emit sendCommand(COMMAND_42, nullptr);
}

/**
 * @brief MainWindow::onSelfDiaAct_triggered
 */
void MainWindow::onSelfDiaAct_triggered()
{
    emit sendCommand(COMMAND_41, nullptr);
}

/**
 * @brief MainWindow::onHardReset_triggered
 */
void MainWindow::onHardReset_triggered()
{
    emit sendCommand(CLEAR_CONFIG, nullptr);
}

/**
 * @brief MainWindow::onManual_triggered
 */
void MainWindow::onManual_triggered()
{
    QDialog* oldWindow = this->findChild<QDialog*>("manualWindow");
    if( oldWindow != nullptr ){
        delete oldWindow;
    }

    QFile file(":text/manual.txt");
    file.open(QIODevice::ReadOnly);
    QDialog* manualWindow = new QDialog(this);
    manualWindow->setObjectName("manualWindow");
    manualWindow->setWindowTitle("Инструкция");
    manualWindow->resize(700, 500);
    manualWindow->setWindowFlags(Qt::Drawer);
    manualWindow->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* manualLayot = new QVBoxLayout;
    QTextEdit* textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    manualLayot->addWidget(textEdit);
    manualWindow->setLayout(manualLayot);
    QTextStream in(&file);
    QString line = in.readAll();
    textEdit->setPlainText(line);
    file.close();

    manualWindow->show();
}

/**
 * @brief MainWindow::onAbout_triggered
 */
void MainWindow::onAbout_triggered()
{
    QDialog* aboutWindow = new QDialog(this);
    aboutWindow->setWindowTitle("О программе");
    aboutWindow->resize(370, 200);
    aboutWindow->setModal(true);
    aboutWindow->setWindowFlags(Qt::Drawer);
    QLabel* logoLabel = new QLabel;
    QPixmap logoPixmap(":/images/alc64x64_32.png");
    logoLabel->setPixmap(logoPixmap);
    aboutWindow->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* textLabel = new QLabel;
    QVBoxLayout* aboutLayot = new QVBoxLayout;
    textLabel->setText(tr("<h1>HART® Device Manager</h1>"
                          "<p>Версия 1.5.1</p>"
                          "<p>Программа для проверки и настройки приборов с протоколом HART.</p>"
                          "<p>Copyright &copy; ООО Альконт, 2018.</p>"));

    aboutLayot->addWidget(logoLabel, 0, Qt::AlignCenter);
    aboutLayot->addWidget(textLabel, 0, Qt::AlignCenter);
    aboutWindow->setLayout(aboutLayot);

    aboutWindow->show();
}

/**
 * @brief MainWindow::onShowPolligAddress
 * @param addr
 */
void MainWindow::onShowPolligProgress(quint8 val)
{
    findBar->setValue(val);
    statusBar()->showMessage("Адрес опроса: "+QString::number(val), POLLING_TIMEOUT);
}

/**
 * @brief MainWindow::onItemSelectionChanged
 */
void MainWindow::onItemSelectionChanged()
{
    findBar->setVisible(false);
    connPushButton->setEnabled(true);
    findPushButton->setText("Поиск устройств");
    statusBar()->showMessage("");

    emit clearPacketQueue();
    emit selectDevice(deviceListPointer.at(listWidget->currentRow()));
}

/**
 * @brief MainWindow::onSearchCompleted
 */
void MainWindow::onSearchCompleted()
{
    findPushButton->setText("Поиск устройств");
    connPushButton->setEnabled(true);
    findBar->setValue(0);
    findBar->setVisible(false);
    if( listWidget->count() == 0 ){
        listWidget->addItem("Устройств не обнаружено");
    }
    statusBar()->showMessage("Поиск устройств завершен", 3000);
    QMessageBox::information(this, tr("Информация"), "Поиск завершен.");
}

/**
 * @brief MainWindow::onFillTableWidget
 * @param type
 * @param serial
 */
void MainWindow::onFillTableWidget(const QList<Device*> &list)
{
    int index = list.size()-1;

    deviceListPointer = list;

    listWidget->addItem(QString::number(list.at(index)->getShortAddress())+
                        ": Серийный № " +QString::number(list.at(index)->getSerial()) +
                        "  Тип: "+QString::number(list.at(index)->getTypeCode(), 16).toUpper());
}
//eof

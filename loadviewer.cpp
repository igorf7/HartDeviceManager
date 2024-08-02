#include "loadviewer.h"
#include "mycrc.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QIntValidator>

/**
 * @brief LoadViewer::LoadViewer constructor
 * @param parent
 */
LoadViewer::LoadViewer(QObject *parent) : QObject(parent)
{
    isLoadingComplete = true;

    loadGroupBox = new QGroupBox();
    QPushButton* fopenButton = new QPushButton("Файл:");
    fopenButton->setAutoDefault(true);
    QLineEdit* fpathLineEdit = new QLineEdit();
    fpathLineEdit->setObjectName("fpathLineEdit");
    QLabel* uniqIdLabel = new QLabel("Серийный номер устройства:");
    serialLineEdit = new QLineEdit;
    QIntValidator* validator = new QIntValidator(0, 16777215);
    serialLineEdit->setValidator(validator);
    incCheckBox = new QCheckBox("Автоинкркмент");
    QPushButton* checkMemButton = new QPushButton("   Проверить память   ");
    checkMemButton->setAutoDefault(true);
    QPushButton* loadStartButton = new QPushButton("   Загрузить прошивку   ");
    loadStartButton->setAutoDefault(true);
    QPushButton* appStartButton = new QPushButton("   Старт устройства   ");
    appStartButton->setAutoDefault(true);

    loadGroupBox->setTitle("Работа с устройствами в режиме загрузчика");
    loadGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(fopenButton);
    hLayout1->addWidget(fpathLineEdit);
    QHBoxLayout* hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(uniqIdLabel);
    hLayout2->addWidget(serialLineEdit, 0, Qt::AlignLeft);
    hLayout2->addStretch();
    QHBoxLayout* hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(checkMemButton);
    hLayout3->addStretch();
    hLayout3->addWidget(loadStartButton);
    hLayout3->addStretch();
    hLayout3->addWidget(appStartButton);
    QVBoxLayout* loaderLayout = new QVBoxLayout();
    loaderLayout->addStretch();
    loaderLayout->addLayout(hLayout2);
    loaderLayout->addWidget(incCheckBox);
    loaderLayout->addStretch();
    loaderLayout->addLayout(hLayout1);
    loaderLayout->addLayout(hLayout3);
    loadGroupBox->setLayout(loaderLayout);

    connect(fopenButton, SIGNAL(clicked()), this, SLOT(onFopenButton_clicked()));
    connect(checkMemButton, SIGNAL(clicked()), this, SLOT(onCheckMemButton_clicked()));
    connect(loadStartButton, SIGNAL(clicked()), this, SLOT(onloadStartButton_clicked()));
    connect(appStartButton, SIGNAL(clicked()), this, SLOT(onAppStartButton_clicked()));
}

/**
 * @brief LoadViewer::~LoadViewer destructor
 */
LoadViewer::~LoadViewer()
{    
    delete loadGroupBox;
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief LoadViewer::onChangeTabIndex. Sets information about current tab index.
 * @param index - current tab index

void LoadViewer::onChangeTabIndex(int index)
{
    currentTabIndex = index;
} */

/**
 * @brief LoadViewer::createLoadStateWindow
 */
void LoadViewer::createLoadStateWindow()
{
    loadStateWindow = new QDialog(loadGroupBox);
    QVBoxLayout* vLayout = new QVBoxLayout();
    QHBoxLayout* hLayout = new QHBoxLayout();
    QLabel* eraseLabel = new QLabel("Стирание:");
    eraseLabel->setObjectName("eraseLabel");
    QLabel* writeLabel = new QLabel("Запись:");
    writeLabel->setObjectName("writeLabel");
    QLabel* resultLabel = new QLabel("Результат:");
    resultLabel->setObjectName("resultLabel");
    QProgressBar* eraseBar = new QProgressBar();
    eraseBar->setObjectName("eraseBar");
    //eraseBar->setFixedHeight(16);
    writeBar = new QProgressBar();
    //writeBar->setFixedHeight(16);

    QPushButton* closeButton = new QPushButton("Закрыть");
    closeButton->setEnabled(false);
    closeButton->setObjectName("closeButton");
    loadStateWindow->setWindowTitle("Загрузка прошивки");
    //loadStateWindow->setGeometry(0, 0, 550, 156);
    loadStateWindow->resize(550, 156);
    loadStateWindow->setModal(true);
    loadStateWindow->setWindowFlags(Qt::Tool);//( Qt::Window|Qt::WindowTitleHint|Qt::CustomizeWindowHint );
    closeButton->setFixedWidth(80);
    vLayout->addWidget(eraseLabel);
    vLayout->addWidget(eraseBar);
    vLayout->addWidget(writeLabel);
    vLayout->addWidget(writeBar);
    vLayout->addWidget(resultLabel);
    hLayout->addWidget(closeButton);
    vLayout->addLayout(hLayout);
    loadStateWindow->setLayout(vLayout);

    /* Show the window */
    loadStateWindow->show();

    connect(closeButton, SIGNAL(clicked()), this, SLOT(onLoadWinCloseButton_clicked()));
    connect(loadStateWindow, SIGNAL(finished(int)), this, SLOT(onDialogFinished(int)));
}

/**
 * @brief LoadViewer::onFopenButton_clicked slot implemettaion.
 */
void LoadViewer::onFopenButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(nullptr, tr("Open File"),
                                                    tr(""), tr("Binary (*.bin)"));
    if( !fileName.isNull() ) {
        QLineEdit* fpathLineEdit =
                this->loadGroupBox->findChild<QLineEdit*>("fpathLineEdit");
        if( fpathLineEdit != nullptr ){
            fpathLineEdit->setText(fileName);
        }
    }
}

/**
 * @brief LoadViewer::onloadStartButton_clicked slot implemettaion.
 */
void LoadViewer::onloadStartButton_clicked()
{
    QFile fwFile;
    quint32 serial;
    QByteArray serialNum;

    if( serialLineEdit->text() != "" ){
        serial = serialLineEdit->text().toUInt();
    }
    else{
        QMessageBox::warning(loadGroupBox, tr("Ошибка"), "Не задан серийный номер.");
        return;
    }

    serialNum.append(static_cast<char>((serial>>16) & 0xFF));
    serialNum.append(static_cast<char>((serial>>8) & 0xFF));
    serialNum.append(static_cast<char>(serial & 0xFF));

    if( !fileName.isNull() ){
        fwFile.setFileName(fileName);
        emit sendCommand(SET_UNIQUE_ID, serialNum);
    }
    else{
        QMessageBox::warning(loadGroupBox, tr("Ошибка"), "Файл не выбран.");
        return;
    }

    if( !fwFile.open(QIODevice::ReadOnly) ) {
        QMessageBox::warning(loadGroupBox, tr("Ошибка"), "Невозможно открыть файл.");
    }
    else{
        qDebug()<<"Открыт файл: "<<fileName;
        fwBuffer.clear();
        if( !fwFile.seek(0) ) {
            // error!!!
        }
        fwBuffer = fwFile.readAll();
        while( fwBuffer.size() % 4 ){
            fwBuffer.append(static_cast<char>(0xFF));
        }
        fwFile.close();
        quint32 *buff = reinterpret_cast<quint32*>(fwBuffer.data());
        fwHashSum = calcBufCRC32(buff, fwBuffer.size() / 4);
        isLoadingComplete = false;
        if( incCheckBox->isChecked() ){
            serial++;
            serialLineEdit->setText(QString::number(serial));
        }
    }
}

/**
 * @brief LoadViewer::onEraseComplete slot implemettaion.
 */
void LoadViewer::onEraseComplete()
{
    QProgressBar* eraseBar =
            this->loadStateWindow->findChild<QProgressBar*>("eraseBar");
    QLabel* eraseLabel =
            this->loadStateWindow->findChild<QLabel*>("eraseLabel");

    /* Fill the erase progress bar */
    eraseBar->setValue(100);
    eraseLabel->setText(eraseLabel->text()+" выполнено.");

    emit sendCommand(WRITE_FLASH_FIRST, fwBuffer);
}

/**
 * @brief LoadViewer::onDialogFinished. Closes and deletes load progress window.
 * @param code - not used
 */
void LoadViewer::onDialogFinished(int code)
{
    Q_UNUSED(code);

    if( isLoadingComplete != true ){
        QMessageBox msg;
        msg.setText("Ну что вы хулиганиити...ети..та?!");
        msg.setInformativeText("Хотите прервать загрузку?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setIcon(QMessageBox::Question);
        msg.setDefaultButton(QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Да");
        msg.setButtonText(QMessageBox::No, "Нет");

        int res = msg.exec();
        if (res != QMessageBox::Yes){
            loadStateWindow->show();
        }
        else{
            quint32 serial = serialLineEdit->text().toUInt();
            serialLineEdit->setText(QString::number(serial-1));
            onLoadWinCloseButton_clicked();
            emit sendCommand(STOP_CMD, nullptr);
            emit sendCommand(CHECK_MEMORY, nullptr);
            if( loadStateWindow ) {
                delete loadStateWindow;
            }
        }
    }
}

/**
 * @brief LoadViewer::onTransactionComplete. Handles responses from the device.
 * @param response - response command code
 * @param prm2 - extra parameter
 */
void LoadViewer::onTransactionComplete(quint8 response, const QVariant &prm2)
{
    QString cnf_str, clb_str, app_str;

    QPushButton* closeButton =
            this->loadStateWindow->findChild<QPushButton*>("closeButton");
    QLabel* resultLabel =
            this->loadStateWindow->findChild<QLabel*>("resultLabel");

    switch(response)
    {
        case SET_UNIQUE_ID:
            serialNumber = static_cast<quint32>(prm2.toInt());
            createLoadStateWindow();
            writeBar->setMinimum(0);
            writeBar->setMaximum(fwBuffer.size());
            emit sendCommand(ERASE_APP_FLASH, nullptr);
            break;
        case WRITE_FLASH_FIRST:
        case WRITE_FLASH_NEXT:
            writeBar->setValue(writeBar->value()+DATA_CHUNCK_SIZE);
            if( loadStateWindow ) {
                emit sendCommand(WRITE_FLASH_NEXT, nullptr);
            }
            break;
        case CHECK_HASH_SUM:
            if( closeButton != nullptr ) {
                closeButton->setEnabled(true);
            }
            if( fwHashSum == static_cast<quint32>(prm2.toInt()) ){
                if( resultLabel!= nullptr ) {
                    resultLabel->setText(resultLabel->text()+" загрузка завершена. Верификация пройдена."
                                         +" Присвоен серийный номер: "+QString::number(serialNumber));                    
                }
            }
            else{
                QMessageBox::critical(loadGroupBox, tr("Ошибка"), "Ошибка верификации! Необходима перезапись.");
                if( resultLabel != nullptr ) {
                    resultLabel->setText(resultLabel->text()+" Ошибка верификации! Необходима перезапись.");
                }
            }
            break;
        case CHECK_MEMORY:
            if( static_cast<quint8>(prm2.toInt()) & 0x01 )
                cnf_str = "ДА";
            else
                cnf_str = "НЕТ";
            if( static_cast<quint8>(prm2.toInt()) & 0x02 )
                clb_str = "ДА";
            else
                clb_str = "НЕТ";
            if( static_cast<quint8>(prm2.toInt()) & 0x04 )
                app_str = "ДА";
            else
                app_str = "НЕТ";

            QMessageBox::information(loadGroupBox, tr("Проверка памяти"),
                                     "Данные в памяти устройства:\n"
                                     "\n"
                                     "Прошивка: " + app_str + "\n"
                                     "Калибровка: " + clb_str + "\n"
                                     "Конфигурация: " + cnf_str + "\n");
            break;
        default:
            break;
    }
}

/**
 * @brief LoadViewer::onLoadingComplete. Handles loading complete event.
 */
void LoadViewer::onLoadingComplete()
{
    QLabel* writeLabel =
            this->loadStateWindow->findChild<QLabel*>("writeLabel");

    writeBar->setValue(fwBuffer.size());
    fwBuffer.clear();

    if( writeLabel != nullptr ) {
        writeLabel->setText(writeLabel->text()+" завершена.");
    }
    isLoadingComplete = true;
    emit sendCommand(CHECK_HASH_SUM, nullptr);
}

/**
 * @brief LoadViewer::onLoadWinCloseButton_clicked slot implementation.
 */
void LoadViewer::onLoadWinCloseButton_clicked()
{
    if( loadStateWindow ) {
        delete loadStateWindow;
    }
}

/**
 * @brief LoadViewer::onCheckMemButton_clicked slot implementation.
 */
void LoadViewer::onCheckMemButton_clicked()
{
    emit sendCommand(CHECK_MEMORY, nullptr);
}

/**
 * @brief LoadViewer::onAppStartButton_clicked slot implementation.
 */
void LoadViewer::onAppStartButton_clicked()
{
    emit sendCommand(START_APPLICATION, nullptr);
}
//eof

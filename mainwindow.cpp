#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textToSpeech = new QTextToSpeech(this);
    QStringList engines;
    QMetaObject::invokeMethod(textToSpeech, "availableEngines",
                              Qt::DirectConnection,qReturnArg(engines));
    ui->comboBoxEngines->addItems(engines);
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon = this->style()->standardIcon(QStyle::SP_DirOpenIcon);
    trayIcon->setIcon(icon);
    trayIcon->setVisible(true);
    status = this->trayShow();
    rateSpin = new QDoubleSpinBox(this);
    volumeSpin = new QDoubleSpinBox(this);
    pitchSpin = new QDoubleSpinBox(this);
    this->setSpinBox(rateSpin);
    this->setSpinBox(volumeSpin);
    this->setSpinBox(pitchSpin);
    rateSpin->setSuffix("rate");
    volumeSpin->setSuffix("volume");
    pitchSpin->setSuffix("pitch");
    rateSpin->setValue(textToSpeech->rate());
    volumeSpin->setValue(textToSpeech->volume());
    pitchSpin->setValue(textToSpeech->pitch());
    connect(rateSpin,&QDoubleSpinBox::valueChanged,this,&MainWindow::do_rateSpinValueChanged);
    connect(volumeSpin,&QDoubleSpinBox::valueChanged,this,&MainWindow::do_volumeSpinValueChanged);
    connect(pitchSpin,&QDoubleSpinBox::valueChanged,this,&MainWindow::do_pitchSpinValueChanged);
    connect(textToSpeech,&QTextToSpeech::errorOccurred,this,&MainWindow::do_textToSpeechErrorOccurred);
    connect(textToSpeech,&QTextToSpeech::stateChanged,this,&MainWindow::do_textToSpeechStateChanged);
    connect(textToSpeech,&QTextToSpeech::engineChanged,this,&MainWindow::do_textToSpeechEngineChanged);
}

void MainWindow::do_rateSpinValueChanged(double value)
{
    textToSpeech->setRate(value);
}

void MainWindow::do_volumeSpinValueChanged(double value)
{
    textToSpeech->setVolume(value);
}

void MainWindow::do_pitchSpinValueChanged(double value)
{
    textToSpeech->setPitch(value);
}

void MainWindow::do_textToSpeechEngineChanged(const QString& engine)
{
    QList<QVoice> voices = textToSpeech->availableVoices();
    for(auto voice:voices)
    {
        QString name = voice.name();
        QString age = QVoice::ageName(voice.age());
        QString gender = QVoice::genderName(voice.gender());
        qDebug()<<"name:"<<name<<" age:"<<age<<" gender:"<<gender;
    }
    QList<QLocale> locales = textToSpeech->availableLocales();
    for(auto locale:locales)
    {
        QString country = QLocale::countryToString(locale.country());
        QString language = QLocale::languageToString(locale.language());
        qDebug()<<"country:"<<country<<" language:"<<language;
    }
}

void MainWindow::do_textToSpeechStateChanged(QTextToSpeech::State state)
{
    switch (state) {
    case QTextToSpeech::Ready:
        trayIcon->showMessage("提示","已经准备就绪！",QSystemTrayIcon::Information,2000);
        break;
    case QTextToSpeech::Speaking:
        trayIcon->showMessage("提示","正在说话!",QSystemTrayIcon::Information,2000);
                              break;
    case QTextToSpeech::Paused:
        trayIcon->showMessage("提示","暂停中",QSystemTrayIcon::Information,2000);
                              break;
    case QTextToSpeech::State::Error:
                              trayIcon->showMessage("提示","发生错误!",QSystemTrayIcon::Critical,2000);
                              break;
    }
}

void MainWindow::do_textToSpeechErrorOccurred(QTextToSpeech::ErrorReason error,const QString& errorString)
{
    if(error!=QTextToSpeech::ErrorReason::NoError)
    {

        trayIcon->showMessage("错误",errorString,QSystemTrayIcon::Critical,2000);
        return;
    }
}

bool MainWindow::trayShow()
{
    if(!QSystemTrayIcon::isSystemTrayAvailable())
    {
        qDebug()<<"system tray is invaild";
        return false;
    }
    if(!QSystemTrayIcon::supportsMessages())
    {
        qWarning()<<"system tray cannot show message";
        return false;
    }
    return true;
}

void MainWindow::setSpinBox(QDoubleSpinBox* spinBox)
{
    spinBox->setRange(0,1);
    spinBox->setDecimals(2);
    spinBox->setSingleStep(0.01);
    ui->toolBar->addWidget(spinBox);
}

MainWindow::~MainWindow()
{
    delete textToSpeech;
    delete trayIcon;
    delete ui;
}

void MainWindow::on_comboBoxEngines_currentTextChanged(const QString &arg1)
{
    textToSpeech->setEngine(arg1);
    QList<QVoice> voices = textToSpeech->availableVoices();
    ui->comboBoxVoices->clear();
    ui->comboBoxLocales->clear();
    for(auto voice:voices)
    {
        QString name = voice.name();
        ui->comboBoxVoices->addItem(name,QVariant::fromValue<QVoice>(voice));
    }
    QList<QLocale> locales = textToSpeech->availableLocales();
    for(auto locale:locales)
    {
        QString nowTime = QTime::currentTime().toString("HH:mm:ss");
        QString localeNowTime = locale.toTime(nowTime).toString("HH:mm:ss");
        qDebug()<<"localeNowTime:"<<localeNowTime;
        QString country = QLocale::countryToString(locale.country());
        ui->comboBoxLocales->addItem(country,QVariant::fromValue<QLocale>(locale));
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
    QString text = ui->lineEditText->text();
    if(textToSpeech->engine().isEmpty())
    {
        trayIcon->showMessage("提示","<i><h1>没有引擎可用!</h1></i>",QSystemTrayIcon::Warning,2000);
        return;
    }
    QVariant voiceVariant = ui->comboBoxVoices->currentData();
    QVoice voice = voiceVariant.value<QVoice>();
    QVariant localeVariant = ui->comboBoxLocales->currentData();
    QLocale locale = localeVariant.value<QLocale>();
    textToSpeech->setLocale(locale);
    textToSpeech->setVoice(voice);

    textToSpeech->say(text);
}

void MainWindow::on_pushButtonPause_clicked()
{
    if(textToSpeech->engine().isEmpty())
    {
        if(status)
            trayIcon->showMessage("提示","<i><h1>没有引擎可用!</h1></i>",QSystemTrayIcon::Warning,2000);
        return;
    }
    if(textToSpeech->state() == QTextToSpeech::Paused)
    {
        if(status)
            trayIcon->showMessage("提示","<i><h1>继续讲话!</h1></i>",QSystemTrayIcon::Information,2000);
        textToSpeech->resume();
    }
    else if(textToSpeech->state() == QTextToSpeech::Speaking)
    {
        if(status)
            trayIcon->showMessage("提示","<i><h1>暂停中!</h1></i>",QSystemTrayIcon::Information,2000);
        textToSpeech->pause(QTextToSpeech::BoundaryHint::Word);
    }
}

void MainWindow::on_pushButtonStop_clicked()
{
    if(textToSpeech->engine().isEmpty())
    {
        if(status)
            trayIcon->showMessage("提示","<i><h1>没有引擎可用!</h1></i>",QSystemTrayIcon::Warning,2000);
        return;
    }
    textToSpeech->stop(QTextToSpeech::BoundaryHint::Word);
    if(status)
        trayIcon->showMessage("提示","<i><h1>停止了!</h1></i>",QSystemTrayIcon::Information,2000);
}


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextToSpeech>
#include <QTime>
#include <QSystemTrayIcon>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWidgetAction>
#include <QStyle>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBoxEngines_currentTextChanged(const QString &arg1);

    void on_pushButtonStart_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonStop_clicked();

    void do_textToSpeechStateChanged(QTextToSpeech::State state);

    void do_textToSpeechEngineChanged(const QString& engine);

    void do_textToSpeechErrorOccurred(QTextToSpeech::ErrorReason error,const QString& errorString);

    void do_rateSpinValueChanged(double value);

    void do_pitchSpinValueChanged(double value);

    void do_volumeSpinValueChanged(double value);
private:
    bool trayShow();
    void setSpinBox(QDoubleSpinBox* spinBox);
    Ui::MainWindow *ui;
    bool status;
    QSystemTrayIcon* trayIcon;
    QTextToSpeech* textToSpeech;
    QDoubleSpinBox* volumeSpin;
    QDoubleSpinBox* rateSpin;
    QDoubleSpinBox* pitchSpin;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include "midifile-library/MidiFile.h"
#include <cstdlib>
#include <ctime>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QTemporaryFile>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);  // constructor 
    ~MainWindow();  // destructor 

    virtual void paintEvent(QPaintEvent* event);  // paints piano roll graphics 

    void MakeMelody(); // generates random melody 
    void MakeChords(); // generates random chords 
    void Transpose(); // transposes melody and chords 
    void UnTranspose(); // reverses transposition 
    void Grow(int x); // dma for arpify 
    void Arpify();  // arpeggiates chords 
    void UnArpify(); // reverses arpeggio 
    void UpdateMidi(); // writes midi file 
    void newMidi(); // creates new midi file 

  private slots:
    void on_createMidi_clicked(); // updates midi file 
    void on_selectInst1_textActivated(const QString &arg1); // changes melody instrument
    void on_selectInst2_textActivated(const QString &arg1); // changes chord instrument
    void on_selectTemp_valueChanged(int value); // changes melody temperature  
    void on_newChords_clicked();  // generates new chords 
    void on_newMelody_clicked();  // generates new melody 
    void on_selectArp_stateChanged(int arg1); // arpeggiates chords 
    void on_changeKey_currentIndexChanged(int index); // changes key of midi file 
    void on_newMidiButton_clicked();  // creates new midi file 

  private:
    Ui::MainWindow *ui;
    
    // midi instrument for melody and chords 
    int inst1;
    int inst2;

    // temperature of melody (i.e. amount of notes)
    int temperature;

    bool arpOn; // if chords are arpeggiated 

    // array for chord melodies + rhythms as well as their size and capacity
    int** chords;
    double** rchords;
    int rowSize, rowCapacity;
    int columnSize, columnCapacity;

    // array for main melody + rhythms as well as its size and capacity 
    int** melody;
    double** mrhythm;
    int mcolumnSize, mcolumnCapacity;

    // musical key of the midi 
    int key;
    int nkey;

    QString filePath; // where user exports midi file 
};
#endif // MAINWINDOW_H

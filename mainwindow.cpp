#include "mainwindow.h"
#include "ui_mainwindow.h"

// default constructor 
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  this->setFixedSize(QSize(850,600)); // sets display window to fixed size 
  ui->setupUi(this);

  // midi instrument for melody + chords (0 = piano) 
  inst1 = 0;
  inst2 = 0;

  // temperature of melody (i.e. amount of notes)
  temperature = 3;

  arpOn = false;  // if chords are arpeggiated 

  // rows represent the amount tracks and columns represent the amount of notes in each track
  // in this case, the rowSize for chords is set to 3 since there are 3 tracks (1st, 3rd, 5th)
  // size and capacity are for growing and decreasing size of melodies 
  rowSize = 3;
  rowCapacity = 3;
  columnSize = 5;
  columnCapacity = 5;
  mcolumnSize = 50;
  mcolumnCapacity = 50;

  // musical key of the midi 
  key = 0;
  nkey = 0;

  // initializes chords arrays 
  // for melodies in this program, {71, 72, 74, 76, -1} represents the notes B, C, D, E
  // {1, 1, 1, 1, -1} for the rhythms represents four quarter notes 
  chords = new int*[rowSize];
  rchords = new double*[rowSize];
  for (int i = 0; i < rowSize; i++) {
    chords[i] = new int[columnSize];
    rchords[i] = new double[columnSize];
  }
  for (int i = 0; i < rowSize; i++) {
    for (int j = 0; j < columnSize; j++) {
      chords[i][j] = 72;
      rchords[i][j] = 4;
    }
  }
  for (int i = 0; i < rowSize; i++) {
    chords[i][4] = -1;
    rchords[i][4] = -1;
  }

  // initializes melody 
  melody = new int*[1];
  mrhythm = new double*[1];
  melody[0] = new int[mcolumnSize];
  mrhythm[0] = new double[mcolumnSize];
}

// destructor 
MainWindow::~MainWindow() {
  delete ui;
}

// writes the midifile to the user's desired file location
void MainWindow::UpdateMidi() {
  // transposes midi to user-selected key 
  this->UnTranspose();
  key = nkey;
  this->Transpose();

  update(); // updates graphics for midi 

  smf::MidiFile midifile;   // creates midi file object 
  midifile.addTracks(4);    // adds 4 tracks for melody plus 3 notes for each chord  
  int tpq = 120;            // ticks per quarter note
  midifile.setTicksPerQuarterNote(tpq);

  double actiontick = 0;
  int track = 0;

  // adds track name and tempo to to midi file  
  midifile.addTrackName(track, actiontick, filePath.toStdString());
  midifile.addTempo(track, actiontick, 130.0);

  // stores melody in channel 0 track 1 
  int i = 0;
  actiontick   = 0;
  int velocity     = 90;
  int channel      = 0;
  track        = 1;

  // adds track name and instrument for the melody
  midifile.addTrackName(track, actiontick, "Melody");
  midifile.addPatchChange(track, actiontick, channel, inst1); 

  // adds melody notes to midifile object until sentinel value is reached 
  while (melody[0][i] >= 0) {
    midifile.addNoteOn(track, actiontick, channel, melody[0][i], velocity);
    actiontick += tpq * mrhythm[0][i];
    midifile.addNoteOff(track, actiontick, channel, melody[0][i], velocity);
    i++;
  }

  // for-loop runs 3 times (3 notes in chord)
  for (int j = 0; j < 3; j++) {
    // stores chords in channel 1 track 2 
    i = 0;
    actiontick   = 0;
    velocity     = 90;
    channel      = 1;
    track        = 2;

    // adds track name and instrument for the chords
    midifile.addTrackName(track, actiontick, "Chords");
    midifile.addPatchChange(track, actiontick, channel, inst2); 

    // adds chords notes to midifile object until sentinel value is reached 
    while (chords[j][i] >= 0) {
      midifile.addNoteOn(track, actiontick, channel, chords[j][i], velocity);
      actiontick += tpq * rchords[j][i];
      midifile.addNoteOff(track, actiontick, channel, chords[j][i], velocity);
      i++;
    }
  }

  midifile.sortTracks();  // ensures tick times are in correct order

  midifile.write(filePath.toStdString().c_str()); // writes midifile to user-selected filepath
}

void MainWindow::on_createMidi_clicked() {
  this->UpdateMidi();
}

// sets melody instrument 
void MainWindow::on_selectInst1_textActivated(const QString &arg1) {
  if (arg1 == "Piano")
    inst1 = 0;
  else if (arg1 == "Violin")
    inst1 = 40;
  else
    inst1 = 56;
}

// sets chords instrument  
void MainWindow::on_selectInst2_textActivated(const QString &arg1) {
  if (arg1 == "Piano")
    inst2 = 0;
  else if (arg1 == "Violin")
    inst2 = 40;
  else
    inst2 = 56;
}

// changes termperature of melody (3 - 9)
void MainWindow::on_selectTemp_valueChanged(int value) {
  temperature = (value + 1) * 3;
}

// creates randomized melody 
void MainWindow::MakeMelody() {
  srand(time(0)); // seeds rng

  double melodyTotal = 8; // the rhythms of 1/2 of a track should add up to 8

  // every note in the c-major scale 
  int possibleNotes[] = {67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84}; 

  // rhythms of each note appear at different frequencies for controlled random effect 
  double possibleRhythms[] = {0.5, 0.5, 0.5, 1, 1, 1, 1.5, 1.5, 2, 2, 2.5, 3, 3, 3.5, 4}; 

  // keeps track of current note so next note is nearby 
  int currentNote = possibleNotes[rand() % 10];

  // amount of notes in each half of the melody  
  int noteAmountOne = rand() % 2 + temperature;
  int noteAmountTwo = rand() % 2 + temperature;
  mcolumnSize = noteAmountOne + noteAmountTwo + 1;

  // sets rhythms for first half of melody 
  do {
    melodyTotal = 8;
    for (int i = 0; i < noteAmountOne; i++) {
      mrhythm[0][i] = possibleRhythms[rand() % 14];
      melodyTotal -= mrhythm[0][i];
    }
  } while (melodyTotal != 0);

  // sets rhythms for second half of melody 
  do {
    melodyTotal = 8;
    for (int i = noteAmountOne; i < noteAmountOne + noteAmountTwo; i++) {
      mrhythm[0][i] = possibleRhythms[rand() % 14];
      melodyTotal -= mrhythm[0][i];
    }
  } while (melodyTotal != 0);

  // assigns random note to each rhythm -- note cannot be > 6 notes away from previous ntoe 
  for (int i = 0; i < noteAmountOne + noteAmountTwo; i++) {
    melody[0][i] = currentNote;
    do {
      currentNote = possibleNotes[rand() % 10];
    } while (abs(currentNote - melody[0][i]) > 6);
  }

  // sets sentinel value for melody and rhythm arrays 
  melody[0][noteAmountOne + noteAmountTwo] = -1;
  mrhythm[0][noteAmountOne + noteAmountTwo] = -1;

  // transposes melody to user-selected key 
  for (int i = 0; i < mcolumnSize - 1; i++)
    melody[0][i] += key;
}

void MainWindow::MakeChords() {
  // these are the root or first note of every chord in the c-major scale 
  int possibleRoots[] = {50, 52, 53, 55, 57, 59, 60, 50, 52, 53, 55, 57, 59, 60};

  // based on the root note, this loop adds the third and fifth to each chord 
  for (int i = 0; i < 4; i++) {
    chords[0][i] = possibleRoots[rand() % 13];
    switch (chords[0][i]) {
      // creates minor chords 
      case 50: case 52: case 57:
        chords[1][i] = chords[0][i] + 3;
        chords[2][i] = chords[0][i] + 7;
        break;
        // creates major chords 
      case 53: case 55: case 60:
        chords[1][i] = chords[0][i] + 4;
        chords[2][i] = chords[0][i] + 7;
        break;
        // creates vii. chord 
      case 59:
        chords[1][i] = chords[0][i] + 3;
        chords[2][i] = chords[0][i] + 6;
        break;
    }
    // lowers fifth down an octave for really high chords 
    if (chords[0][i] >= 57)
      chords[2][i] -= 12;
  }

  // sets sentinel value for chord arrays
  for (int i = 0; i < 3; i++)
    chords[i][4] = -1;

  // transposes chords to user-selected key 
  for (int i = 0; i < rowSize; i++) {
    if (i == 0) {
      for (int j = 0; j < columnSize - 5; j++)
        chords[i][j]  += key;
    }
    else {
      for (int j = 0; j < columnSize - 1; j++)
        chords[i][j] += key;
    }
  }

  // arpeggiate chords if user checks arp box 
  if (arpOn == true)
    this->Arpify();
}

// transposes all melodies to user selected key 
void MainWindow::Transpose() {
  // chord melodies 
  for (int i = 0; i < rowSize; i++) {
    if (i == 0) {
      for (int j = 0; j < columnSize - 5; j++)
        chords[i][j] += key;
    }
    else {
      for (int j = 0; j < columnSize - 1; j++)
        chords[i][j] += key;
    }
  }
  // main melody
  for (int i = 0; i < mcolumnSize - 1; i++)
    melody[0][i] += key;
}

// reverses transposition so a new transposition can occur 
void MainWindow::UnTranspose() {
  // chord melodies 
  for (int i = 0; i < rowSize; i++) {
      if (i == 0) {
          for (int j = 0; j < columnSize - 5; j++)
              chords[i][j] -= key;
      }
      else {
    for (int j = 0; j < columnSize - 1; j++)
      chords[i][j] -= key;
      }
  }
  // main melody
  for (int i = 0; i < mcolumnSize - 1; i++)
    melody[0][i] -= key;
}

// array memory allocation when more chords notes are added 
void MainWindow::Grow(int x) {
  // increases chord capacities 
  columnCapacity += x;

  // dma  
  for (int i = 0; i < rowSize; i++) {
    // creates temp arrays of new size 
    int* tempMel = new int[columnCapacity];
    double* tempRhy = new double[columnCapacity];

    // copies over array contents to temp arrays
    for (int j = 0; j < columnSize; j++) {
      tempMel[j] = chords[i][j];
      tempRhy[j] = rchords[i][j];
    }

    // deallocates arrays 
    delete [] chords[i];
    delete [] rchords[i];

    // change pointers 
    chords[i] = tempMel;
    rchords[i] = tempRhy;
  }
  // increases chord sizes 
  columnSize += x;
}

// arpeggiates chords 
void MainWindow::Arpify() {
  // creates room for new notes 
  if (columnCapacity - columnSize < 4)
    this->Grow(4);
  else
    columnSize += 4;

  srand(time(0)); // seeds rng 

  // adds additional third and fifth notes in their correct locations 
  for (int i = columnSize - 6; i >= 0; i--) {
    chords[1][i*2] = chords[1][i];
    chords[1][(i*2)+1] = chords[1][i];
    chords[2][i*2] = chords[2][i];
    chords[2][(i*2)+1] = chords[2][i];
  }

  // adds sentinel values 
  chords[1][columnSize-1] = -1;
  chords[2][columnSize-1] = -1;

  // randomizes rhythms for new notes for variations in arpeggios 
  for (int i = 0; i < columnSize / 2; i++) {
    double temp = (rand() % 4 + 1);
    temp /= 2;
    rchords[1][i*2] = temp;
    rchords[1][(i*2)+1] = 4 - temp;
    temp = (rand() % 4 + 1);
    temp /= 2;
    rchords[2][i*2] = temp;
    rchords[2][(i*2)+1] = 4 - temp;
  }

  // adds sentinel values
  rchords[1][columnSize-1] = -1;
  rchords[2][columnSize-1] = -1;
}

// reverses arpeggio effect 
void MainWindow::UnArpify() {
  // removes the additional third and fifth notes added 
  for (int i = 0; i < columnSize / 2; i++) {
    chords[1][i] = chords[1][i*2];
    rchords[1][i] = 4;
    chords[2][i] = chords[2][i*2];
    rchords[2][i] = 4;
  }

  // adds sentinel values
  chords[1][4] = -1;
  chords[2][4] = -1;
  rchords[1][4] = -1;
  rchords[2][4] = -1;

  columnSize -= 4;
}

// user keeps same melody but adds new chords 
void MainWindow::on_newChords_clicked() {
  this->MakeChords();
}

// user keeps same chords but adds new melody 
void MainWindow::on_newMelody_clicked() {
  this->MakeMelody();
}

// calls arp functions for arp checkbox 
void MainWindow::on_selectArp_stateChanged(int arg1) {
  if (arg1 == 2) {
    this->Arpify();
    arpOn = true;
  }
  else {
    this->UnArpify();
    arpOn = false;
  }
}

// creates graphics for piano roll 
void MainWindow::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);  // "QPaintEvent* event parameter is necessary but is not explicitly used  
  QPainter painter(this); // creates QPainter object 

  // offset-x and offset-y for moving piano roll 
  const int OX = 105;
  const int OY = 20;

  // draws border around piano roll 
  painter.drawRect(QRect(0 + OX, 0 + OY, 640, 470));

  // draws light grid lines 
  painter.setOpacity(0.3);
  for (int i = 0; i < 16; i++)  // vertical lines
    painter.drawLine((40 * i) + OX, 0 + OY, (40 * i) + OX, 470 + OY);
  for (int i = 0; i <= 46; i++) // horizontal lines 
    painter.drawLine(0 + OX, (10 * i) + OY, 640 + OX, (10 * i) + OY);

  // adds label for c note 
  // note -- this is kept as a switch statement for more labels to be added if desired 
  painter.setOpacity(1);
  for (int i = 0; i <= 46; i++) {
    switch((96-i) % 12) {
      case 0:
        painter.drawText(0 + OX - 20, (i * 10) + OY + 10, " C");
    }
  }

  // paints black rectangles to represent the black notes 
  painter.setOpacity(0.2);
  painter.fillRect(0+OX, 0 + OY, 640,470,"white");
  for (int i = 0; i <= 46; i++) {
    switch((96-i) % 12) {
      case 1: case 3: case 6: case 8: case 10: // black notes relative to note c5 
        painter.fillRect(0+OX, (i*10) + OY, 640, 10, "black");
    }
  }
  painter.setOpacity(1);


  // iterates through melody array and paints the notes 
  // the placement offsets the notes by the the cumulative length for correct positioning 
  int i = 0;
  double placement = 0;
  while (melody[0][i] != -1) {
    // paints red rectangle for each note 
    painter.fillRect((placement*40) + OX, ((96 - melody[0][i]) * 10) + OY, mrhythm[0][i]*40, 10, "red");

    // draws border around each note 
    painter.drawLine((placement*40) + OX, ((97 - melody[0][i]) * 10) + OY, (placement*40) + OX, (((97 - melody[0][i]) * 10) - 10) + OY);
    painter.drawLine((placement*40) + (mrhythm[0][i]*40) + OX, ((97 - melody[0][i]) * 10) + OY, ((placement*40) + (mrhythm[0][i]*40)) + OX, (((97 - melody[0][i]) * 10) - 10) + OY);
    painter.drawLine((placement*40) + OX, ((97 - melody[0][i]) * 10) + OY, ((placement*40) + (mrhythm[0][i]*40)) + OX, ((97 - melody[0][i]) * 10) + OY);
    painter.drawLine((placement*40) + OX, (((97 - melody[0][i]) * 10) - 10) + OY, ((placement*40) + (mrhythm[0][i]*40)) + OX, (((97 - melody[0][i]) * 10) - 10) + OY);

    // accumulates total note length 
    placement += mrhythm[0][i];
    i++;
  }

  // iterates through chord arrays and paints the notes 
  for (int j = 0; j < rowSize; j++) {
    i = 0;
    placement = 0;
    while (chords[j][i] != -1) {
      // paints blue rectangles for each note 
      painter.fillRect((placement*40)+OX, ((96 - chords[j][i]) * 10)+OY, rchords[j][i]*40, 10, "blue");

      // draws border around each note 
      painter.drawLine((placement*40) + OX, ((97 - chords[j][i]) * 10) + OY, (placement*40) + OX, (((97 - chords[j][i]) * 10) - 10) + OY);
      painter.drawLine((placement*40) + (rchords[j][i]*40) + OX, ((97 - chords[j][i]) * 10) + OY, ((placement*40) + (rchords[j][i]*40)) + OX, (((97 - chords[j][i]) * 10) - 10) + OY);
      painter.drawLine((placement*40) + OX, ((97 - chords[j][i]) * 10) + OY, ((placement*40) + (rchords[j][i]*40)) + OX, ((97 - chords[j][i]) * 10) + OY);
      painter.drawLine((placement*40) + OX, (((97 - chords[j][i]) * 10) - 10) + OY, ((placement*40) + (rchords[j][i]*40)) + OX, (((97 - chords[j][i]) * 10) - 10) + OY);

      // accumulates total note length 
      placement += rchords[j][i];
      i++;
    }
  }
}

// user selects desired key for midi 
void MainWindow::on_changeKey_currentIndexChanged(int index) {
  switch (index) {
    case 0: nkey = 0; break;
    case 1: nkey = 1; break;
    case 2: nkey = 2; break;
    case 3: nkey = 3; break;
    case 4: nkey = 4; break;
    case 5: nkey = 5; break;
    case 6: nkey = 6; break;
    case 7: nkey = 7; break;
    case 8: nkey = 8; break;
    case 9: nkey = 9; break;
    case 10: nkey = 10; break;
    case 11: nkey = 11; break;
  }
}

// creates a new midi file 
void MainWindow::on_newMidiButton_clicked() {
  this->newMidi();
}

// framework for creating a new midi -- new melody, chords, filepath, updates graphics
void MainWindow::newMidi() {
  this->MakeChords();
  this->MakeMelody();
  do {
    filePath = QFileDialog::getSaveFileName(this, tr("Save MIDI File"), QDir::homePath(), tr("MIDI Files (*.mid)"));
  } while (filePath.isEmpty());
  this->UpdateMidi();
}


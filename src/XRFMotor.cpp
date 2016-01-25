#include "XRFMotor.h"


#include <QtGui> 
#include <QTimer>      
#include <QString>                	
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/mman.h>
#include <math.h>
#include <sys/ioctl.h>
#include <SerialStream.h>
#include <cstdlib>
#include <time.h>

#include <iostream>
//#include <ostream> 
#include <sstream> 

using namespace std;
using namespace LibSerial;


#define SHMSZ       204800//200000
#define SHMSZBIG 122880000 //120000000

#define MAX_LINE_LENGTH 256



int onlyOne=0;
double V=1;


string posZ;
QString current_posZ; QString line1;

float temp;
//int n=1; 
int EventOffset=0; int missing=1;
int uno=0;
double Zmin1,Zmax1;
double Z_goto=30000.;
double Zmin=30000.;
double Zmax=60000.;

int Clock=0, Clock2=0;
bool aborted=false;
bool ask=false;
bool FirstRun=true;
bool opened=false;
bool ZConnected=false;
double valueZ;
int casenumber=4;
int interval=100;
double tempoPos=1000;
int NscanZ=0;
bool TimerActive=false;
double positionZ;
int m=1, pluto=0, pluto_copy=0;
float Zo,vZ;
int *shm; int *shmCommand;
int *shm2;int *shmCommand2;



/*******************************/
//int value;
/******************************/
int eventionline=0;
char process[30];

MyWidget::MyWidget(QWidget *parent)
  : QWidget(parent),
    NowZ("Z= "),
    IniZready(0),
    fPortX(0),
    fPortY(2),
    fPortZ(1),
    XOnTarget(false), 
    XHasMoved(true), 
    Xmoving(false),
    YOnTarget(false),
    YHasMoved(true),
    Ymoving(false),
    ZOnTarget(false),
    ZHasMoved(true),
    Zmoving(false),
    fStepX(1000),
    fStepY(1000),
    fStepZ(1000)
{
  
  if ( !SetSharedMemory() ){
    qDebug() << "Something went wrong with the shared memory allocation " 
	     << endl;
    exit(-1);
  }
  
  setupUi(this); // this sets up GUI
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
  
  timerS = new QTimer(this);
  connect(timerS, SIGNAL(timeout()), this, SLOT(timerEventS()));
  timerS->start(1000);////qui???
  
  spinBoxSerialX->setValue(fPortX);
  spinBoxSerialY->setValue(fPortY);
  spinBoxSerialZ->setValue(fPortZ);

  connect(spinBoxSerialX, SIGNAL(valueChanged(int)), 
	  this, SLOT(NameX(int)));
  connect(spinBoxSerialY, SIGNAL(valueChanged(int)), 
	  this, SLOT(NameY(int)));
  connect(spinBoxSerialZ, SIGNAL(valueChanged(int)), 
	  this, SLOT(NameZ(int)));

  connect(pushButtonSerialX, SIGNAL(released()),
	  this, SLOT(AssignX()));
  connect(pushButtonSerialY, SIGNAL(released()),
	  this, SLOT(AssignY()));
  connect(pushButtonSerialZ, SIGNAL(released()),
	  this, SLOT(AssignZ()));
    
  connect(pushButtonInitX, SIGNAL(released()),
	  this, SLOT(StartX()));
  connect(pushButtonInitY, SIGNAL(released()),
	  this, SLOT(StartY()));
  connect(pushButtonInitZ, SIGNAL(released()),
	  this, SLOT(StartZ()));
  
    
  connect(quit, SIGNAL(clicked()), this, SLOT(Exit())); ///aggiungere check se è fermo qApp, SLOT(quit())
    
  //connect(quit_3, SIGNAL(clicked()), this, SLOT(Abort()));
    
  connect(quit_4, SIGNAL(clicked()), this, SLOT(Stop()));
    
  connect(quit_5, SIGNAL(clicked()), this, SLOT(Stop()));

  connect(doubleSpinBox, SIGNAL(valueChanged(double)), 
	  this, SLOT(Velocity(double)));
    
  connect(spinBoxStepZ, SIGNAL(valueChanged(double)), 
          this, SLOT(PassoZ(double)));
  
  //connect(doubleSpinBox_6, SIGNAL(valueChanged(double)), 
  //this, SLOT(PassoY(double)));
     
  //connect(doubleSpinBox_3, SIGNAL(valueChanged(double)), 
  //this, SLOT(Zminimo(double)));

  //connect(doubleSpinBox_4, SIGNAL(valueChanged(double)), 
  //this, SLOT(Zmassimo(double)));

     
  //connect(doubleSpinBox_8, SIGNAL(valueChanged(double)), 
  //this, SLOT(Yminimo(double)));

  //connect(doubleSpinBox_7, SIGNAL(valueChanged(double)), 
  //this, SLOT(Ymassimo(double)));
  
  // =========> spin boxes are read just when move button is pushed
 
  //  connect(doubleSpinBox_9, SIGNAL(valueChanged(double)), 
  //	  this, SLOT(Z_to(double)));

  //connect(doubleSpinBox_10, SIGNAL(valueChanged(double)), 
  //this, SLOT(Y_to(double)));


  // connect(scan_3, SIGNAL(clicked()), this, SLOT(StartXYScan()));

  //connect(scan_4, SIGNAL(clicked()), this, SLOT(StartYXScan()));


  connect(pushButtonMoveLeft, SIGNAL(released()),
	  this, SLOT(MoveLeft()));
  connect(pushButtonMoveRight, SIGNAL(released()),
	  this, SLOT(MoveRight()));
  
  connect(pushButtonMoveUp, SIGNAL(released()),
	  this, SLOT(MoveUp()));
  connect(pushButtonMoveDown, SIGNAL(released()),
	  this, SLOT(MoveDown()));

  connect(pushButtonMoveF, SIGNAL(released()),
	  this, SLOT(MoveForward()));
  connect(pushButtonMoveB, SIGNAL(released()),
	  this, SLOT(MoveBackwards()));

  connect(pushButtonMoveXTo, SIGNAL(released()),
	  this, SLOT(MoveXTo()));
  connect(pushButtonMoveYTo, SIGNAL(released()),
	  this, SLOT(MoveYTo()));
  connect(pushButtonMoveZTo, SIGNAL(released()),
	  this, SLOT(MoveZTo()));
    
  //     connect(pushButton_9, SIGNAL(released()),
  //            this, SLOT(MoveY_To()));
  lineEditPosX->setText("Not initialized"); 
  lineEditPosY->setText("Not initialized"); 
  lineEditPosZ->setText("Not initialized"); 
      
  ReadArduino();
}


bool 
MyWidget::SetSharedMemory(){
  
  //// setting the shared memory initialization for x, y, z
  const key_t key = 7000;
  const int shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);
  fShared_memoryZ = (int *) shmat(shmid, NULL, 0);
  if (fShared_memoryZ == NULL){
    qDebug() << "Could not attribute the shared memory for Z" << endl;
    return false;
  }
  
  qDebug() << "Data Memory attached for Z at :" 
	   << (int *)fShared_memoryZ << endl 
	   << " with shmid: " << shmid << endl;

  const key_t key2 = 7200;
  const int shmid2 = shmget(key2, SHMSZBIG, IPC_CREAT | 0666);
  if (shmid2 == -1)
    qDebug() << " Errore  "<< strerror(errno)<<'\n';
  fShared_memoryZ2 =(int *) shmat(shmid2, NULL, 0);
  if (fShared_memoryZ2 == NULL){
    qDebug() << "Could not attribute the shared memory for Z2" << endl;
    return false;
  }

  qDebug() << "Data Memory for Z attached at :"
	   << (int *)fShared_memoryZ2 << endl << " with shmid: " 
	   << shmid2 << endl;


#warning: Changed the keys, does this influence the code?

  const int shmidxy = shmget (7300, SHMSZ, IPC_CREAT | 0666);
  fShared_memoryXY = (int *) shmat(shmidxy, NULL, 0);
  if (fShared_memoryXY == NULL || shmidxy== -1){
    qDebug() << "Could not attribute the shared memory for XY" << endl;
    return false;
  }

  qDebug() << "Data Memory attached at :" 
	   << (int *)fShared_memoryXY << endl 
	   << " with shmid: " << shmidxy << endl;
  
  
  const int shmidxy2 = shmget(7100, SHMSZBIG, IPC_CREAT | 0666);
  if (shmidxy2 == -1) 
    qDebug() << "Errore  " << strerror(errno)<<'\n';
  fShared_memoryXY2 = (int *) shmat(shmid2, NULL, 0);
  if (fShared_memoryXY2 == NULL){
    qDebug() << "Could not attribute the shared memory for XY2" << endl;
    return false;
  }

  qDebug() << "Data Memory attached for XY2 at :" 
	   << (int *)fShared_memoryXY2 << endl 
	   << " with shmid: " << shmidxy2 << endl;

  ///  SHARED MEMORY STARTING CONFIGURATION  for Z ///
  ////////////////////////////////////////////////////
  fMotoreWindowStatusZ = 1;           
  *fShared_memoryZ = fMotoreWindowStatusZ;       /// Finestra motore ON   ////
  fCentralWindowStatusZ = 1;           
  *(fShared_memoryZ+1) = fCentralWindowStatusZ; /// Finestra main ON      ////
  *(fShared_memoryZ+2) = fPortZ;             ///Legge la porta X
  IniZ = 0;                     
  *(fShared_memoryZ+3) = IniZ;           ///Inizializzazione off
  IniZready = 0;                      
  *(fShared_memoryZ+4) = IniZready;      ///Motore non inizializzato
  fSerialiStatusZ = 1;                 
  *(fShared_memoryZ+5) = fSerialiStatusZ;  ///finestra delle seriali abili

  
  IniY = 0;                          
  //*(fShared_memoryZ+6)=IniY;                ///Inizializzazione off
  IniYready = 0;                      
  //*(fShared_memoryZ+7)=IniYready;          ///Motore non inizializzato
  *(fShared_memoryZ+10) = 0;            ///Posizione X
  //*(fShared_memoryZ+11)=0;          ///Posizione Y
  //*(fShared_memoryZ+12)=0;          ///Integrale  (contenuto singolo pixel mappa online)
  *(fShared_memoryZ+13) = 0;          ///...News (sono pronte le nuove posizioni)
  *(fShared_memoryZ+14) = 0;          ///...Zmin
  *(fShared_memoryZ+15) = 0;          ///...Zmax
  //*(fShared_memoryZ+16)=0;        ///...Ymin
  //*(fShared_memoryZ+17)=0;        ///...Ymax
  fVmeStatusZ = *(fShared_memoryZ+18);  ///...Status Vme
  *(fShared_memoryZ+19)=0;          /// Avviso spostamento motore
  *(fShared_memoryZ+20)=0;          /// spostamento Z
  //*(fShared_memoryZ+21)=0;        /// spostamento y
  //*(fShared_memoryZ+22)=0;        ///Status MergePos Z sommare diverse scansioni
  //*(fShared_memoryZ+24)=0;        ///Spettro in canale o energia (0 canale) (1 energia)
  *(fShared_memoryZ+25)=0;          ///Passo Z
  // *(fShared_memoryZ+26)=0;       ///Passo Y
  //*(fShared_memoryZ+27)=0;        ///numero pixel letti NO!

  //*(fShared_memoryZ2);   ///Pid ADCXRF  
  *(fShared_memoryZ2+1)=0;    //// Posiz Z
  //*(fShared_memoryZ2+2)=0;    //// Posiz Y
  *(fShared_memoryZ2+3)=0;    //// Posizione Ready
  //*(fShared_memoryZ2+4);    //// Numero dati in memoria (posizioni e energie)
  //*(fShared_memoryZ2+5);     ///NEvents totali ADC per mappa online
  //*(fShared_memoryZ2+6);     ///N vicino a SegFault
  //*(fShared_memoryZ2+7);     ///Eventi ADC mentre avanza la scansione (per rate)
  //*(fShared_memoryZ2+8-10);    //// Posizioni libere 
  //*(fShared_memoryZ2+11);      //// Da qui inizia a scrivere Position
  //*(fShared_memoryZ2+4)=0;    //// Numero dati in memoria
  
  ///  SHARED MEMORY STARTING CONFIGURATION  for XY ///
  /////////////////////////////////////////////////////
  fMotoreWindowStatusXY = 1;            
  *fShared_memoryXY = fMotoreWindowStatusXY;         /// Finestra motore ON   ////
  fCentralWindowStatusXY = 1;           
  *(fShared_memoryXY+1) = fCentralWindowStatusXY;   /// Finestra main ON      ////
  *(fShared_memoryXY+2) = fPortX;                 ///Legge la porta X
  *(fShared_memoryXY+8) = fPortY;                 ///Legge la porta Y
  IniX = 0;                           
  *(fShared_memoryXY+3) = IniX;                ///Inizializzazione off
  IniXready=0;                      
  *(fShared_memoryXY+4) = IniXready;          ///Motore non inizializzato
  IniY = 0;          
  *(fShared_memoryXY+6) = IniY;                ///Inizializzazione off
  IniYready = 0;
  *(fShared_memoryXY+7) = IniYready;          ///Motore non inizializzato
  fSerialiStatusXY = 1;
  *(fShared_memoryXY+5) = fSerialiStatusXY;    ///finestra delle seriali abilitata
  *(fShared_memoryXY+10) = 0;          ///Posizione X
  *(fShared_memoryXY+11) = 0;          ///Posizione Y
  *(fShared_memoryXY+12) = 0;          ///Integrale  (contenuto singolo pixel mappa online)
  *(fShared_memoryXY+13) = 0;          ///...News (sono pronte le nuove posizioni)
  *(fShared_memoryXY+14) = 0;          ///...Xmin
  *(fShared_memoryXY+15) = 0;          ///...Xmax
  *(fShared_memoryXY+16) = 0;          ///...Ymin
  *(fShared_memoryXY+17) = 0;          ///...Ymax
  fVmeStatusXY = *(fShared_memoryXY+18);          ///...Status Vme
  *(fShared_memoryXY+19) = 0;         //Avviso spostamento motore
  *(fShared_memoryXY+20) = 0;          //spostamento x
  *(fShared_memoryXY+21) = 0;          //spostamento y
  *(fShared_memoryXY+22) = 0;          //Status MergePos x sommare diverse scansioni
  //*(fShared_memoryXY+24) = 0;        ///Spettro in canale o energia (0 canale) (1 energia)
  *(fShared_memoryXY+25) = 0; //Passo X
  *(fShared_memoryXY+26) = 0; //Passo Y
   // *(fShared_memoryXY+27)=0; //numero pixel letti NO!
  
   //  *(fShared_memoryXY2);   ///Pid ADCXRF  
  *(fShared_memoryXY2+1) = 0;    //// Posiz X
  *(fShared_memoryXY2+2) = 0;    //// Posiz Y
  *(fShared_memoryXY2+3) = 0;    //// Posizione Ready
  //*(fShared_memoryXY2+4);    //// Numero dati in memoria (posizioni e energie)
  //*(fShared_memoryXY2+5);     ///NEvents totali ADC per mappa online
  //*(fShared_memoryXY2+6);     ///N vicino a SegFault
  //*(fShared_memoryXY2+7);     ///Eventi ADC mentre avanza la scansione (per rate)
  //*(fShared_memoryXY2+8-10);    //// Posizioni libere 
  //*(fShared_memoryXY2+11);      //// Da qui inizia a scrivere Position
  return true;
}


int 
MyWidget::ReadArduino() {
  SerialStream arduino_serial;
  
  arduino_serial.Open("/dev/ttyACM1");
  arduino_serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
  if (!arduino_serial.IsOpen()){
    cout << "could not open Arduino :( " << endl;
    return 0;
  } 
  
  char line[MAX_LINE_LENGTH];
  int k = 0;
  unsigned int value = 0;
  const unsigned int outrange = 123;
  while (!value && k < 10) {
    arduino_serial.getline(line, MAX_LINE_LENGTH);
    value = atoi(line);
    if (value == outrange)
      cout << " Out of Range... " << value << endl;
    else 
      cout << " Current position: " << value << endl;

    ++k;
  }
  ostringstream textInside;
  textInside << value;
  
  NowZ = textInside.str().c_str();
  lineEdit_2->setText(NowZ); 
  
  return value;
}






void  MyWidget::timerEvent(){
  Clock++;
  CheckOnTarget();
  
  //CheckYOnTarget();
  //ScanXY();
  //ScanYX();
  //MoveDoubleClick();
  //CheckSegFault();
}


int 
MyWidget::SendCommand(const int serial, 
		      int chan, const char *comando, 
		      const char *parametri){
  char canale=chan+'0';
  int r;
  string cm=comando;
  if(parametri==NULL) 
    cm=cm+'\n';
  else
    cm=cm+' '+canale+' '+parametri+'\n';
  //qDebug()<<cm.data()<<'\n';
  
  if(write(serial, cm.data(), cm.size()))  
    r=0;
  else {
    r=1;
    qDebug()<<"Errore in scrittura  "<< strerror(errno)<<'\n';
  }
  return r;
}



char* 
MyWidget::read_answer(const int  serial) const {
  //    printf("In attesa di risposta...\n");
  char c[100];
  int n = 0;
  string rest;
  while((n=read(serial, &c, sizeof(c)))>0)      
    {
      c[n]=0;
      rest=rest+c;      
      if(c[n-1]=='\n')
	break;        
    }
  // qDebug()<<"RISPOSTA: "<< rest.data()<<'\n';
  return c;  
}

string 
MyWidget::read_answer2(const int serial) const{
  char c[100];
  int n = 0;
  string rest;
  while((n=read(serial, &c, sizeof(c)))>0){
    c[n]=0;
    rest=rest+c;      
    if(c[n-1]=='\n')
      break;        
  }
  return rest;  
}



void MyWidget::CheckOnTarget()
{
  string a;
  QString Qa;
  if (fIsXInitialized){
    SendCommand(fSerialX, 1,"ONT?",NULL);
    a = read_answer2(fSerialX);
    Qa = a.data();
    if(!Qa.contains("0", Qt::CaseInsensitive)){ 
      XOnTarget = true; 
      Xmoving = false; 
    }  else {
      XOnTarget = false; 
      Xmoving = true;
    }
    SendCommand(fSerialX, 1,"POS?",NULL);                                                      //scrive la posizione X sulla line edit
    const string checkX = read_answer2(fSerialX);
    NowX="X= ";
    NowX.append(checkX.data());
    NowX.remove(3,2); 
    lineEditPosX->setText(NowX); 
  } else 
    lineEditPosY->setText("Not initialized"); 
  
  ////// Y part //////
  
  if (fIsYInitialized){
    SendCommand(fSerialY, 1,"ONT?",NULL);
    a = read_answer2(fSerialY);
    Qa = a.data();
    if (!Qa.contains("0", Qt::CaseInsensitive)){ 
      YOnTarget = true; 
      Ymoving = false; 
    } else {
      YOnTarget = false; 
      Ymoving = true;
    }
    SendCommand(fSerialY, 1,"POS?",NULL);                                                      //scrive la posizione X sulla line edit
    string checkY = read_answer2(fSerialY);
    NowY = "Y= ";
    NowY.append(checkY.data());
    NowY.remove(3,2); 
    lineEditPosY->setText(NowY); 
  } else
    lineEditPosY->setText("Not initialized"); 
  
  ////// Z part //////
  if (fIsZInitialized){
    SendCommand(fSerialZ, 1,"ONT?",NULL);
    a = read_answer2(fSerialZ);
    Qa = a.data();
    if(Qa.contains("0", Qt::CaseInsensitive)==false){ 
      ZOnTarget = true; 
      Zmoving = false; 
    } else {
      ZOnTarget = false; 
      Zmoving = true;
    }
    SendCommand(fSerialZ, 1,"POS?",NULL); //scrive la posizione X sulla line edit
    string checkZ = read_answer2(fSerialZ);
    NowZ="Z= ";
    NowZ.append(checkZ.data());
    NowZ.remove(3,2); 
    lineEditPosZ->setText(NowZ); 
  } else 
    lineEditPosZ->setText("Not initialized"); 

}


void MyWidget::Velocity(double number)
{
  V = number;
  tempoPos = fStepZ/V;
  qDebug() << "velocità " << V << "mm/s" 
	   << "Scrittura posizione ogni " << tempoPos << " ms";
  char v[10];
  sprintf(v,"%f",V);
  SendCommand(fSerialZ, 1,"VEL",v);  
  //invia_comando_Y(1,"VEL",v);  
}


///////////da qui micron//////////////////////////////
void MyWidget::PassoZ(double number1){
  fStepZ = number1*1000; 
  *(fShared_memoryZ+25) = fStepZ;
}

void MyWidget::Zminimo(double number2){
  Zmin1 = number2*1000;  
  positionZ = Zmin1; 
  *(fShared_memoryZ+14) = Zmin1;
}

void MyWidget::Zmassimo(double number3){ 
  Zmax1 = number3*1000; 
  *(fShared_memoryZ+15) = Zmax1;
}


void MyWidget::Z_to(double number9){ 
 Z_goto = number9*1000;
}

//void MyWidget::Y_to(double number10) 
//{Y_goto=number10*1000;}



void MyWidget::StartZ(){
  
  if (!fIsZInitialized){
    fIsZInitialized = true; 
    InizializzazioneZ();
  } else{ 
    qDebug() <<"Inizializzazione già in corso";
  }
}  //ok è vero solo se l'inizializzazione 

void MyWidget::StartX(){ 
  if (!fIsXInitialized){
    qDebug() <<" =====> Inizialiazing X ";
    InizializzazioneX();
  } else{ 
    qDebug() <<"Inizializzazione già in corso";
  }
}  //ok è vero solo se l'inizializzazione 

void MyWidget::StartY(){ 
  if (!fIsYInitialized){
    qDebug() <<" =====> Inizialiazing Y ";
    InizializzazioneY();
  } else{ 
    qDebug() <<"Inizializzazione già in corso";
  }
}  //ok è vero solo se l'inizializzazione 


void MyWidget::Stop(){
  
  SendCommand(fSerialZ, 1,"HLT",NULL); 
  SendCommand(fSerialZ, 1,"ERR?",NULL);
}




void MyWidget::Exit(){
  if( !Zmoving ) {// && Ymoving==false && XYscanning==false){
    fMotoreWindowStatusZ = 0;
    ZShmMotoreStatusWrite();
    //IniY=0; ShmIniYStatusWrite(); 
    //IniYready=0; ShmReadyYStatusWrite();
    IniZ = 0; 
    ZShmIniStatusWrite(); 
    IniZready=0; 
    ZShmReadyStatusWrite();
    if ( TimerActive ){
      timer->stop();
      TimerActive = false;
    }
    qApp->quit();
  }  else {
    //Abort();
    fMotoreWindowStatusZ = 0;
    ZShmMotoreStatusWrite();
    //IniY=0; ShmIniYStatusWrite(); 
    //IniYready=0; ShmReadyYStatusWrite();
    IniZ=0; 
    ZShmIniStatusWrite(); 
    IniZready=0; 
    ZShmReadyStatusWrite();
    if( TimerActive ){
      timer->stop();
      TimerActive=false;
    }
    qApp->quit();
  }
 
}




//void MyWidget::SaveTxt()                                                 //scrive Position.txt leggendo i dati in memoria
//{

//QString percorso = QFileDialog::getSaveFileName(this,tr("Save as"), QDir::currentPath());
/*QString percorso="/home/frao/XRF/XRF_Data/Position";
  int num=1;
  bool esiste=true;
  QString numerazione, stepx, stepy;
  numerazione.setNum(num);
  stepx.setNum(Px);
  stepy.setNum(Py);
  percorso.append(numerazione);
  percorso.append(".txt");



  while (esiste==true)
  {
  QFile file2(percorso); 
  if(!file2.exists()) {esiste=false;}
  else
  {
  num++;
  numerazione.setNum(num);
  if (num<10)
  percorso.remove(32,5);                                         //fino a 9 files
  else
  percorso.remove(32,6);                                         //da 10 a 99 files...
  percorso.append(numerazione);
  percorso.append("_px");
  percorso.append(stepx);
  percorso.append("_py");
  percorso.append(stepy);
  percorso.append(".txt");
  qDebug()<<"Percorso" <<percorso<<"\n";
  }  

  }*/

/*  QFile file2(percorso); 

    qDebug()<<"Inizio creazione di" <<percorso<<"\n";
    file2.open(QIODevice::ReadWrite);
    QTextStream out2(&file2);

    int Ntot=*(fShared_memoryZ2+4);    //// Numero dati in memoria 

    for(int i=1;i<=Ntot;i++)
    {      
    out2<<*(fShared_memoryZ2+10+i)<<'\n';
    }

    file2.close();
    qDebug()<<percorso<<" pronto!\n";*/
//}


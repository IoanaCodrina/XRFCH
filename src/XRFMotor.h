#include <QObject> 
#include <QApplication>
#include <QFont>
#include <QLCDNumber>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
//#include </usr/include/kde4/kled.h>
#include <QObject>
#include <QFile>
#include <QDir>
#include <QTextStream> 
#include <QString> 
#include <QtGui> //necessaria per il qDebug
#include <QLineEdit>
#include <QFont>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>
#include <QTimerEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QByteArray>
#include <QLineEdit>
#include "ui_XRFGui.h"

// Added for SharedMemory
#include <QBuffer>
#include <qsharedmemory.h>
#include <string.h>
#ifndef PROVA_H
#define PROVA_H



class QAction;
class QMenu;
class QTextEdit;
class QTimer;

class MyWidget : public QWidget, private Ui::MyWidget
{
  Q_OBJECT
  
 public:
     MyWidget(QWidget *parent = 0);
     int ReadArduino();

   public slots:
     
       void Velocity(double);
       void PassoZ(double);
       void Zminimo(double);
       void Zmassimo(double);
       //void Tempo(double);
       void Z_to(double);

       
       void StartX();
       void StartY();
       void StartZ();
       
       void MoveZ(double);
       void MoveToPosition(const int serial, const double where);

       void MoveXTo();
       void MoveYTo();
       void MoveZTo();

       
       void MoveLeft(); /// x movements
       void MoveRight(); /// x movements
       
       void MoveUp(); /// y movements
       void MoveDown(); /// y movements

       void MoveForward(); /// Z movements
       void MoveBackwards(); /// Z movements

       void Stop();

       void CheckOnTarget(); // check the initialization and the pos
       
       void timerEvent(); 
       void timerEventS();
       void Exit();


       void AssignX();
       void NameX(int number);
       void AssignY();
       void NameY(int number);
       void AssignZ();
       void NameZ(int number);

private:
       
       QString SetSerialXName(int number);
       QString SetSerialYName(int number);
       QString SetSerialZName(int number);
       
       void InizializzazioneX();
       void InizializzazioneY();
       void InizializzazioneZ();
       
       int SendCommand(int serial, int chan, 
		       const char *comando, const char *parametri);
              
       bool SetSharedMemory();

       void XYShmPortsStatusWrite(){ 
	 *(fShared_memoryXY+2) = fPortX;
	 *(fShared_memoryXY+8) = fPortY;
       }
       
       void XYShmPortsStatusRead(){ 
	 fPortX = *(fShared_memoryXY+2);
	 fPortY = *(fShared_memoryXY+8);
       }
       
       void XYShmMotoreStatusWrite(){ 
	 *(fShared_memoryXY) = fMotoreWindowStatusXY;}
       void XYShmMotoreStatusRead(){ 
	 fMotoreWindowStatusXY = *(fShared_memoryXY); }
       void XShmIniStatusWrite(){ *(fShared_memoryXY+3) = IniX;}
       void XShmIniStatusRead(){ IniX = *(fShared_memoryXY+3);}
       void XShmReadyStatusWrite(){ *(fShared_memoryXY+4) = IniXready;}
       void XShmReadyStatusRead(){ IniXready = *(fShared_memoryXY+4);}
       void XYShmVmeStatusWrite(){ *(fShared_memoryXY+18) = fVmeStatusXY; }
       void XYShmVmeStatusRead(){ fVmeStatusXY = *(fShared_memoryXY+18);}
     
       void YShmIniStatusWrite(){ *(fShared_memoryXY+6) = IniY;}
       void YShmIniStatusRead(){ IniY=*(fShared_memoryXY+6); }
       void YShmReadyStatusWrite(){ *(fShared_memoryXY+7) = IniYready; }
       void YShmReadyStatusRead(){ IniYready =* (fShared_memoryXY+7); }
      
       void ZShmPortsStatusWrite(){ *(fShared_memoryZ+2) = fPortZ;}
       void ZShmPortsStatusRead(){ fPortZ = *(fShared_memoryZ+2);}
       void ZShmMotoreStatusWrite(){ *(fShared_memoryZ) = fMotoreWindowStatusZ;}
       void ZShmMotoreStatusRead(){ fMotoreWindowStatusZ = *(fShared_memoryZ);}
       void ZShmIniStatusWrite(){ *(fShared_memoryZ+3) = IniZ;}
       void ZShmIniStatusRead(){ IniZ = *(fShared_memoryZ+3);}
       void ZShmReadyStatusWrite(){ *(fShared_memoryZ+4) = IniZready;}
       void ZShmReadyStatusRead(){ IniZready = *(fShared_memoryZ+4);}
       void ZShmVmeStatusWrite(){ *(fShared_memoryZ+18) = fVmeStatusZ;}
       void ZShmVmeStatusRead(){   fVmeStatusZ = *(fShared_memoryZ+18);}


       std::string read_answer2(const int serial) const;
       char*  read_answer(const int serial) const;


       
       QString NowX;
       QString NowY;
       QString NowZ;
       
       QTimer *timer;
       QTimer *timerPos;
       QTimer *timerS;
       

       int IniX;
       int IniY;
       int IniZ;
       int IniXready;
       int IniYready;
       int IniZready;
       
       
       int *fShared_memoryZ; // memory to be shared with the Z motor
       int *fShared_memoryZ2;// memory to be shared with the Z motor
       int *fShared_memoryXY; // memory to be shared with the Z motor
       int *fShared_memoryXY2;// memory to be shared with the Z motor

       
       int fSerialX;
       int fSerialY;
       int fSerialZ;
       
       int fPortX; 
       int fPortY; 
       int fPortZ; 


       int fSerialiStatusXY;
       int fCentralWindowStatusXY;
       int fMotoreWindowStatusXY;
       int fVmeStatusXY;

       int fSerialiStatusZ;
       int fCentralWindowStatusZ;
       int fMotoreWindowStatusZ;
       int fVmeStatusZ;

       bool XConnected; 
       bool YConnected; 
       bool ZConnected; 
       
       /* bool okX; */
       /* bool okY; */
       /* bool okZ; */
       bool XOnTarget; 
       bool XHasMoved; 
       bool Xmoving;
       bool YOnTarget; 
       bool YHasMoved; 
       bool Ymoving;
       bool ZOnTarget; 
       bool ZHasMoved; 
       bool Zmoving;

       bool fIsXInitialized;
       bool fIsYInitialized;
       bool fIsZInitialized;

       double fStepX;
       double fStepY;
       double fStepZ;
};
 

#endif

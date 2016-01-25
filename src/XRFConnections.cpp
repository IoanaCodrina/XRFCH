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
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
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
#include <sstream> 

using namespace std;

/////////variabili e funzioni definite altrove ma necessarie/////////////////////////
extern int MotoreWindowStatus, CentralWindowStatus, portY, IniZ, IniY, IniZready, IniYready, SerialiStatus;

//extern bool ZConnected; //extern bool YConnected;
//////////////////////////////////////////////////////////////////////

//era /dev/
//QString TTY_Y="/dev/";
int RispPortX=2, RispPortY= 2, RispPortZ=2;
int Sx=0;
int Sy=2;
int Sz=1;

bool go=true;

int i=0, k=0; int j=0, l=0; int ClockS;




void MyWidget::NameX(int number){ 
  Sx = number;
}

void MyWidget::NameY(int number){ 
  Sy = number;
}

void MyWidget::NameZ(int number){ 
  Sz = number;
}


void MyWidget::AssignZ(){
  
  i=0;
  j=0;
  ZShmMotoreStatusRead();
  ZShmIniStatusRead();
  ZShmReadyStatusRead();
  qDebug() << "AssignZ::" << IniZ << " " << IniZready << endl;
  if( IniZ == 0 && IniZready == 0){
    Sz = spinBoxSerialZ->value();
    QString NameTTY_Z = SetSerialZName(Sz);
  }
  else
    qDebug()<<"Porta Z gia' assegnata!\n";
}

void MyWidget::AssignX(){
  
  i=0;
  j=0;
      
  XYShmMotoreStatusRead();
  XShmIniStatusRead();
  XShmReadyStatusRead();
  qDebug() << "AssignX::" << IniX << " " << IniXready << endl;
  if( IniX == 0 && IniXready == 0){
    Sx = spinBoxSerialX->value();
    QString NameTTY = SetSerialXName(Sx);
  }
  else
    qDebug()<<"Porta X gia' assegnata!\n";

}


void MyWidget::AssignY(){
  
  i=0;
  j=0;
  
  XYShmMotoreStatusRead();
  YShmIniStatusRead();
  YShmReadyStatusRead();
  qDebug() << "AssignX::" << IniY << " " << IniYready << endl;
  if( IniY == 0 && IniYready == 0){
    Sy = spinBoxSerialY->value();
    QString NameTTY_Z = SetSerialYName(Sy);
  }
  else
    qDebug()<<"Porta X gia' assegnata!\n";
}




QString 
MyWidget::SetSerialZName(int number){
  

  ostringstream MYTTY_Z;
  //  const char *MYTTY_Z;
  fPortZ = number;
  ZShmPortsStatusWrite();

  QString NameNumber;
  NameNumber.setNum(number);
  QString TTY_Z = "/dev/";
  TTY_Z.append(NameNumber); 
  MYTTY_Z << "/dev/ttyUSB" << fPortZ; 
  
  errno = 0; 
  RispPortZ = 2; 
  j = 0;
  
  qDebug()<< "SetSerialZName::" << MYTTY_Z.str().c_str() <<'\n';
  
  fSerialZ = open(MYTTY_Z.str().c_str(),O_RDWR,0677); 
  
  if ( fSerialZ < 0){
    qDebug() << "ERROR opening" << MYTTY_Z << strerror(errno)<<'\n';
    RispPortZ = 3;
  }  else  {
    fd_set rfds;
    struct timeval tv;
    int retval,Nfd;
    
    struct termios my_termios;
    struct termios new_termios;
    
    tcgetattr( fSerialZ, &my_termios );
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
    
    //    my_termios.c_flag &= ~CBAUD;
    //    my_termios.c_flag |= B9600;
    
    int h=cfsetospeed(&my_termios,B9600);
    int h1=cfgetospeed(&my_termios);
    //printf("speed=%d \n",h1);
    
    my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
    my_termios.c_cflag |= CLOCAL;
    //  my_termios.c_iflag = IXON | IXOFF;
    my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
    my_termios.c_cc[VMIN] = 1;
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
    
    tcsetattr( fSerialZ, TCSANOW, &my_termios );
    tcgetattr( fSerialZ, &new_termios );
    
    
    FD_ZERO(&rfds);
    FD_SET(fSerialZ, &rfds);
    
    tv.tv_sec=3;
    tv.tv_usec=0;
    
    SendCommand(fSerialZ, 1,"*IDN?",NULL);
    Nfd=fSerialZ+1;
    retval=select(Nfd, &rfds, NULL,NULL, &tv);
    string vediamo = read_answer2(fSerialZ);
    RispPortZ = (retval==0) ? 0:1;
  }
  return TTY_Z;
}



QString 
MyWidget::SetSerialXName(int number){

  qDebug() << "Setting the serial X name" << endl;

  QString TTY_X="/dev/";
  fPortX = number;
  XYShmPortsStatusWrite();

  QString NameNumber;
  NameNumber.setNum(number);
  TTY_X.append(NameNumber); 

  ostringstream MYTTY;
  MYTTY << "/dev/ttyUSB" << fPortX; 
  
  errno = 0; 
  RispPortX = 2; 
  j = 0;
  
  qDebug()<< "SetSerialXName::" << MYTTY.str().c_str() <<'\n';
  
  fSerialX = open(MYTTY.str().c_str(),O_RDWR,0677); 
  
  if ( fSerialX < 0){
    qDebug() << "ERROR opening" << MYTTY << strerror(errno)<<'\n';
    RispPortX = 3;
  } else {
    fd_set rfds;
    struct timeval tv;
    int retval,Nfd;
      
    struct termios my_termios;
    struct termios new_termios;
    
    tcgetattr( fSerialX, &my_termios );
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
      
    //    my_termios.c_flag &= ~CBAUD;
    //    my_termios.c_flag |= B9600;
    
    int h=cfsetospeed(&my_termios,B9600);
    int h1=cfgetospeed(&my_termios);
    //printf("speed=%d \n",h1);
      
    my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
    my_termios.c_cflag |= CLOCAL;
    //  my_termios.c_iflag = IXON | IXOFF;
    my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
    my_termios.c_cc[VMIN] = 1;
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
      
    tcsetattr( fSerialX, TCSANOW, &my_termios );
    tcgetattr( fSerialX, &new_termios );
    FD_ZERO(&rfds);
    FD_SET(fSerialX, &rfds);
    tv.tv_sec=3;
    tv.tv_usec=0;
    SendCommand(fSerialX, 1,"*IDN?",NULL);
    Nfd = fSerialX+1;
    retval = select(Nfd, &rfds, NULL,NULL, &tv);
    string vediamo = read_answer2(fSerialX);
    RispPortX = (retval==0) ? 0:1;
  }
  
  return TTY_X;
}




QString 
MyWidget::SetSerialYName(int number){

  qDebug() << "Setting the serial X name" << endl;

  QString TTY_Y="/dev/";
  fPortY = number;
  XYShmPortsStatusWrite();

  QString NameNumber;
  NameNumber.setNum(number);
  TTY_Y.append(NameNumber); 

  ostringstream MYTTY;
  MYTTY << "/dev/ttyUSB" << fPortY; 
  
  errno = 0; 
  RispPortY = 2; 
  j = 0;
  
  qDebug()<< "SetSerialYName::" << MYTTY.str().c_str() <<'\n';
  
  fSerialY = open(MYTTY.str().c_str(),O_RDWR,0677); 
  
  if ( fSerialY < 0){
    qDebug() << "ERROR opening" << MYTTY << strerror(errno)<<'\n';
    RispPortY = 3;
  } else {
    fd_set rfds;
    struct timeval tv;
    int retval,Nfd;
      
    struct termios my_termios;
    struct termios new_termios;
    
    tcgetattr( fSerialY, &my_termios );
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
      
    //    my_termios.c_flag &= ~CBAUD;
    //    my_termios.c_flag |= B9600;
    
    int h=cfsetospeed(&my_termios,B9600);
    int h1=cfgetospeed(&my_termios);
    //printf("speed=%d \n",h1);
      
    my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
    my_termios.c_cflag |= CLOCAL;
    //  my_termios.c_iflag = IXON | IXOFF;
    my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
    my_termios.c_cc[VMIN] = 1;
    //printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
      
    tcsetattr( fSerialY, TCSANOW, &my_termios );
    tcgetattr( fSerialY, &new_termios );
    FD_ZERO(&rfds);
    FD_SET(fSerialY, &rfds);
    tv.tv_sec=3;
    tv.tv_usec=0;
    SendCommand(fSerialY, 1,"*IDN?",NULL);
    Nfd = fSerialY+1;
    retval = select(Nfd, &rfds, NULL,NULL, &tv);
    string vediamo = read_answer2(fSerialY);
    RispPortY = (retval==0) ? 0:1;
  }
  
  return TTY_Y;
}


void MyWidget::timerEventS(){
  ClockS++;
  QString commentoX;//, commentoY;
  QString commentoY;//, commentoY;
  QString commentoZ;//, commentoY;

  XShmIniStatusRead();
  YShmIniStatusRead();
  ZShmIniStatusRead();
  
  //cout << "Ports: " << fPortX << " " << fPortY << " " << fPortZ << endl;
  
  
  if ( IniZ == 0 ){         //cioe lavora solo se non siamo in inizializzazione
    //qDebug()<<"TimerEvent!!";
    commentoZ = "Port ";
    ZShmPortsStatusRead();
    ostringstream portName;
    portName << "ttyUSB" << fPortZ << ": ";
    commentoZ.append(portName.str().c_str());
    
    if ( RispPortZ==1 && j<4){
      j++; 
      commentoZ.append(" connecting ");
      if (j==1) ZConnected=true;
    }
    if(RispPortZ==0 && j<4){
      j++; 
      commentoZ.append(" not connecting!");
    }
    if( RispPortZ==3 && j<4){
      j++; 
      commentoZ.append(" Impossibile accedere a z!");
    }
    if( RispPortZ==6 && j<4){
      j++; 
      commentoZ.append(" hola ");
    }
    
    ZShmReadyStatusRead();
    if ( IniZready == 1 && i < 4) {
      i++; 
      commentoZ.append("  Motore z inizializzato!");
    }
    
  } else {
    commentoZ.append("  Inizializzazione z...");
  }
  
  lineEditSerialZ->setText(commentoZ);



   if ( IniX == 0 ){         //cioe lavora solo se non siamo in inizializzazione
    //qDebug()<<"TimerEvent!!";
    commentoX = "Port ";
    XYShmPortsStatusRead();
    ostringstream portName;
    portName << "ttyUSB" << fPortX << ": ";
    commentoX.append(portName.str().c_str());
    
    if ( RispPortX==1 && j<4){
      j++; 
      commentoX.append(" connecting ");
      if (j==1) 
	XConnected=true;
    }
    if(RispPortX == 0 && j<4){
      j++; 
      commentoX.append(" not connecting!");
    }
    if( RispPortX==3 && j<4){
      j++; 
      commentoX.append(" Impossibile accedere a x!");
    }
    if( RispPortX==6 && j<4){
      j++; 
      commentoZ.append(" hola ");
    }
    
    XShmReadyStatusRead();
    if ( IniXready == 1 && i < 4) {
      i++; 
      commentoZ.append("  Motore x inizializzato!");
    }
    
   } else {
     commentoZ.append("  Inizializzazione X...");
   }
   
   lineEditSerialX->setText(commentoX);
   
   if ( IniY == 0 ){         //cioe lavora solo se non siamo in inizializzazione
    //qDebug()<<"TimerEvent!!";
    commentoY = "Port ";
    XYShmPortsStatusRead();
    ostringstream portName;
    portName << "ttyUSB" << fPortY << ": ";
    commentoY.append(portName.str().c_str());
    
    if ( RispPortY==1 && j<4){
      j++; 
      commentoY.append(" connecting ");
      if (j==1) YConnected=true;
    }
    if(RispPortY==0 && j<4){
      j++; 
      commentoY.append(" not connecting!");
    }
    if( RispPortY==3 && j<4){
      j++; 
      commentoY.append(" Impossibile accedere a z!");
    }
    if( RispPortY==6 && j<4){
      j++; 
      commentoY.append(" hola ");
    }
    
    YShmReadyStatusRead();
    if ( IniYready == 1 && i < 4) {
      i++; 
      commentoY.append("  Motore z inizializzato!");
    }
    
  } else {
    commentoY.append("  Inizializzazione z...");
  }
  
  lineEditSerialY->setText(commentoY);

}





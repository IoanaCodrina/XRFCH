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
using namespace std;

/////////variabili e funzioni definite altrove ma necessarie/////////////////////////

extern double Z_goto, Y_goto,Zx, Pz;
extern bool ZOnTarget,YOntarget;
extern int IniZready, IniYready;
extern bool XYscanning, ZOnTarget, YOnTarget;

//extern int invia_comando_Z(int chan,const char *comando, const char *parametri);
//extern int invia_comando_Y(int chan,const char *comando, const char *parametri);
//extern string read_Zanswer2();
//extern string read_Yanswer2();
//////////////////////////////////////////////////////////////////////////////////////

void MyWidget::MoveToPosition(const int serial,
			      const double where){  ////move vuole micron
 
  const double destination = where;
  qDebug() << " Moving to = " << destination;
  
  char s[100];
  sprintf(s,"%f",destination);
  SendCommand(serial, 1, "MOV", s);
}

void MyWidget::MoveZ(double toZ){  ////move vuole micron
  
  double destinationZ=toZ/1000;
  
  qDebug() << " Spostamento a Z= " << destinationZ << '\n';
  if (destinationZ > 50){ 
    cout << " I am not going to move to: " << destinationZ << endl;
    return;
  }
  char sz[100];
  sprintf(sz,"%f",destinationZ);
  SendCommand(fSerialZ, 1,"MOV",sz);
}


void MyWidget::MoveZTo(){
  double value = spinBoxMoveZ->value();
  if (ZOnTarget) {
    MoveToPosition(fSerialZ, value);
    ZOnTarget=false; 
  }
}

void MyWidget::MoveXTo(){
  
  double value = spinBoxMoveX->value();
  
  if (XOnTarget) {
    MoveToPosition(fSerialX,value);
    XOnTarget=false; 
  }
}

void MyWidget::MoveYTo(){
  double value = spinBoxMoveY->value();
  if (YOnTarget) {
    MoveToPosition(fSerialY,value);
    YOnTarget=false; 
  }
}


void MyWidget::MoveRight(){
  string posX;
  QString current_posX;
  double value;
  if( XOnTarget){
    SendCommand(fSerialX, 1,"POS?",NULL);
    posX = read_answer2(fSerialX);
    current_posX = posX.data();
    current_posX.remove(0,2);
    //qDebug()<<"Stringa ottenuta......"<<current_pos;
    value = current_posX.toDouble();
    value = (value - fStepX/1000);
    //sleep(T);
    MoveToPosition(fSerialX, value);
    XOnTarget=false;
  }
}

void MyWidget::MoveLeft(){
  string posX;
  QString current_posX;
  double value;
  if(XOnTarget){
    SendCommand(fSerialX,1, "POS?", NULL);
    posX = read_answer2(fSerialX);
    current_posX = posX.data();
    current_posX.remove(0,2);
    value = current_posX.toDouble();
    value = (value + fStepX/1000);
    MoveToPosition(fSerialX, value);
    XOnTarget=false;
    //qDebug()<<"MoveDown Done......";
  }
}

void MyWidget::MoveUp(){
  string posX;
  QString current_posX;
  double value;
  if(YOnTarget){
    SendCommand(fSerialY,1, "POS?", NULL);
    posX = read_answer2(fSerialY);
    current_posX = posX.data();
    current_posX.remove(0,2);
    value = current_posX.toDouble();
    value = (value + fStepY/1000);
    MoveToPosition(fSerialY, value);
    YOnTarget=false;
    //qDebug()<<"MoveDown Done......";
  }


}
void MyWidget::MoveDown(){

  string posX;
  QString current_posX;
  double value;
  if(YOnTarget){
    SendCommand(fSerialY,1, "POS?", NULL);
    posX = read_answer2(fSerialY);
    current_posX = posX.data();
    current_posX.remove(0,2);
    value = current_posX.toDouble();
    value = (value - fStepY/1000);
    MoveToPosition(fSerialY, value);
    YOnTarget=false;
    //qDebug()<<"MoveDown Done......";
  }
}

void MyWidget::MoveBackwards()
{
  string posZ;
  QString current_posZ;
  double value;

  if (ZOnTarget) {	
    SendCommand(fSerialZ, 1,"POS?",NULL);
    posZ = read_answer2(fSerialZ);
    current_posZ =posZ.data();
    current_posZ.remove(0,2);
    //qDebug()<<"Stringa ottenuta......"<<current_pos;
    value=current_posZ.toDouble();
    value=(value - fStepZ/1000);
    //sleep(T);
    MoveToPosition(fSerialX, value);
    ZOnTarget=false;
  }
}

void MyWidget::MoveForward()
{
  string posZ;
  QString current_posZ;
  double value;
  if (ZOnTarget) {
    SendCommand(fSerialZ, 1,"POS?",NULL);
    posZ = read_answer2(fSerialZ);
    current_posZ = posZ.data();
    current_posZ.remove(0,2);
    value = current_posZ.toDouble();
    value = (value + fStepZ/1000);
    MoveToPosition(fSerialZ, value);
    ZOnTarget = false;
    //qDebug()<<"MoveDown Done......";
  }
}



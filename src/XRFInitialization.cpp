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

////////////variabili e funzioni definite altrove ma necessarie////////////////////////////

extern int n,m, interval;
extern bool TimerActive;

// extern int invia_comando_Z(int chan, const char *comando,
// 			   const char *parametri);
//extern int invia_comando_Y(int chan,const char *comando, const char *parametri);
//extern char *read_Zanswer();
//extern char *read_Yanswer();
//////////////////////////////////////////////////////////////////////////////////////////////////



void MyWidget::InizializzazioneZ()
{
  const char *MYTTY_Z;
  string b,c;
  fIsZInitialized = false;
  
  IniZ=1; 
  IniZready=0;
  ZShmIniStatusWrite();
  
  ZShmPortsStatusRead();
  
  switch (fPortZ) {
  case 0:
    MYTTY_Z="/dev/ttyUSB0";
    break;
  case 1:
    MYTTY_Z="/dev/ttyUSB1"; ////////////// sostituire USB con s se si vuole collegare a una seriale
    break;
  case 2:
    MYTTY_Z="/dev/ttyUSB2";
    break;
  case 3:
    MYTTY_Z="/dev/ttyUSB3";
    break;
  default: 
    cout << "Port " << fPortZ << " not implemented " 
	 <<  " chose another one (< 4) " << endl;
  };
  

  errno=0;


  fSerialZ = open(MYTTY_Z,O_RDWR);  
  if (fSerialZ<0) {
    qDebug() << " ERROR opening " << MYTTY_Z << strerror(errno)<<'\n';
    exit(-1);
  }
  struct termios my_termios;
  struct termios new_termios;
  
  tcgetattr( fSerialZ, &my_termios );
 // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  //  my_termios.c_flag &= ~CBAUD;
  //  my_termios.c_flag |= B19200;

 // int h=cfsetospeed(&my_termios,B9600);
//  printf("speed=%d \n",h);
  my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
  my_termios.c_cflag |= CLOCAL;
  //  my_termios.c_iflag = IXON | IXOFF;
  my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
  my_termios.c_cc[VMIN] = 1;
  // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  
  tcsetattr( fSerialZ, TCSANOW, &my_termios );
  tcgetattr( fSerialZ, &new_termios );
  //  printf("  speed=%d \n",h);
  
  //Inizializzazione Z
  SendCommand(fSerialZ, 1,"SPA"," 60 M-404.2PD");  sleep(1);
  SendCommand(fSerialZ, 1,"SPA"," 1 180"); //primo numero canale SPA secondo numero valore
  SendCommand(fSerialZ, 1,"SPA"," 2 45");          
  SendCommand(fSerialZ, 1,"SPA"," 3 300");         
  SendCommand(fSerialZ, 1,"SPA"," 4 2000");       
  SendCommand(fSerialZ, 1,"SPA"," 14 4000");     
  SendCommand(fSerialZ, 1,"SPA"," 15 1"); ////0.064285257
  SendCommand(fSerialZ, 1,"SPA"," 19 0");          
  SendCommand(fSerialZ, 1,"SPA"," 20 1");        //si usano i limit switches dell'hardware (top e bottom) 
  SendCommand(fSerialZ, 1,"SPA"," 21 50.");       //Max spostamento in positivo da home (0)  
  SendCommand(fSerialZ, 1,"SPA"," 22 25.");   //Value at Reference position
  SendCommand(fSerialZ, 1,"SPA"," 23 25.");  //Distanza tra Ref e Neg-lim
  SendCommand(fSerialZ, 1,"SPA"," 47 25.");    //Distanza tra Ref e Pos-lim
  SendCommand(fSerialZ, 1,"SPA"," 24 0");          //0 i limit switches sono attivi alti...va come 49
  SendCommand(fSerialZ, 1,"SPA"," 48 0.000000");  //Max spostamento in negativo da home (0) 
  SendCommand(fSerialZ, 1,"SPA"," 49 0");        //0 non si inverte il riferimento 
  SendCommand(fSerialZ, 1,"SPA"," 50 0");      //0 Abilita lo stop ai limit switches dell'hardware    
  SendCommand(fSerialZ, 1,"SPA"," 8 0.5");    //8 10 e 11 per ultimi senno' li ricambia 0.035156
  SendCommand(fSerialZ, 1,"SPA"," 10 50");   // velocita'
  SendCommand(fSerialZ, 1,"SPA"," 11 30");  //Accelerazione
  SendCommand(fSerialZ, 1,"SPA"," 117442049 MM");  //Unita'

  
  
  SendCommand(fSerialZ, 1,"SPA? 1 1",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 2",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 3",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 4",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 8",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 10",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 11",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 14",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 15",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 19",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 20",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 21",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 22",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 23",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 24",NULL);
  read_answer(fSerialZ);
  
  SendCommand(fSerialZ, 1,"SPA? 1 48",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 49",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 50",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 60",NULL);
  read_answer(fSerialZ);
  SendCommand(fSerialZ, 1,"SPA? 1 74",NULL); ///per sapere la max accelerazione
  read_answer(fSerialZ);


  sleep(0.5);
  //SendCommand(fSerialZ, 1,"RON 1 1",NULL); //RON e' 1 di default!! 
  //Va messo 0 se si vuole referenziare l'asse con POS
  SendCommand(fSerialZ, 1,"VEL?",NULL);
  read_answer(fSerialZ); 
    
  qDebug() << ("Cambio la velocita' Z.....") << '\n'; 
  SendCommand(fSerialZ, 1,"VEL 1 1",NULL);  
  sleep(0.5);
  
  qDebug() << ("Accendo servo Z.....")<<'\n'; 
  SendCommand(fSerialZ, 1,"SVO 1 1",NULL);  
  sleep(1);
 
  qDebug() << ("Vado alla posizione Z di riferimento ......")<<'\n'; 
  SendCommand(fSerialZ, 1,"FRF 1",NULL); 
  qDebug() << "Attendere........................"<<'\n';   
  
  

  if (!TimerActive){
    timer->start(interval); 
    TimerActive = true;
  }

  fIsZInitialized = true; 
  qDebug() << " Fine dell'inizializzazione Z" << "\n";
  IniZ = 0; 
  ZShmIniStatusWrite(); 
  IniZready = 1; 
  ZShmReadyStatusWrite();
  
   
}




void MyWidget::InizializzazioneX()
{
  const char *MYTTY_X;
  string b,c;
  fIsXInitialized = false;
  
  IniX=1; IniXready=0;
  XShmIniStatusWrite();
  
  XYShmPortsStatusRead();
  
  switch (fPortX) {
  case 0:
    MYTTY_X="/dev/ttyUSB0";
    break;
  case 1:
    MYTTY_X="/dev/ttyUSB1"; ////////////// sostituire USB con s se si vuole collegare a una seriale
    break;
  case 2:
    MYTTY_X="/dev/ttyUSB2";
    break;
  case 3:
    MYTTY_X="/dev/ttyUSB3";
    break;
  default: 
    cout << "Port " << fPortX << " not implemented " 
	 <<  " chose another one (< 4) " << endl;
  };
  
  errno=0;


  fSerialX = open(MYTTY_X,O_RDWR);  
  if ( fSerialX<0){
    qDebug()<<"ERROR opening"<< MYTTY_X<< strerror(errno)<<'\n';
    exit(-1);
  }

  struct termios my_termios;
  struct termios new_termios;

  tcgetattr(fSerialX, &my_termios );
 // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  //  my_termios.c_flag &= ~CBAUD;
  //  my_termios.c_flag |= B19200;

  int h=cfsetospeed(&my_termios,B9600);
  //  printf("speed=%d \n",h);
  my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
  my_termios.c_cflag |= CLOCAL;
  //  my_termios.c_iflag = IXON | IXOFF;
  my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
  my_termios.c_cc[VMIN] = 1;
  // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  
  tcsetattr( fSerialX, TCSANOW, &my_termios );
  tcgetattr( fSerialX, &new_termios );
  //  printf("  speed=%d \n",h);
  
  //Inizializzazione X
  SendCommand(fSerialX,1,"SPA"," 60 M-404.8PD");  sleep(1);
  SendCommand(fSerialX,1,"SPA"," 1 180"); //primo numero canale SPA secondo numero valore
  SendCommand(fSerialX,1,"SPA"," 2 45");          
  SendCommand(fSerialX,1,"SPA"," 3 300");         
  SendCommand(fSerialX,1,"SPA"," 4 2000");       
  SendCommand(fSerialX,1,"SPA"," 14 4000");     
  SendCommand(fSerialX,1,"SPA"," 15 1"); ////0.064285257
  SendCommand(fSerialX,1,"SPA"," 19 0");          
  SendCommand(fSerialX,1,"SPA"," 20 1");        //si usano i limit switches dell'hardware (top e bottom) 
  SendCommand(fSerialX,1,"SPA"," 21 200.");       //Max spostamento in positivo da home (0)  
  SendCommand(fSerialX,1,"SPA"," 22 100.");   //Value at Reference position
  SendCommand(fSerialX,1,"SPA"," 23 100.");  //Distanza tra Ref e Neg-lim
  SendCommand(fSerialX,1,"SPA"," 47 100");    //Distanza tra Ref e Pos-lim
  SendCommand(fSerialX,1,"SPA"," 24 0");          //0 i limit switches sono attivi alti...va come 49
  SendCommand(fSerialX,1,"SPA"," 48 0.000000");  //Max spostamento in negativo da home (0) 
  SendCommand(fSerialX,1,"SPA"," 49 0");        //0 non si inverte il riferimento 
  SendCommand(fSerialX,1,"SPA"," 50 0");      //0 Abilita lo stop ai limit switches dell'hardware    
  SendCommand(fSerialX,1,"SPA"," 8 0.5");    //8 10 e 11 per ultimi senno' li ricambia 0.035156
  SendCommand(fSerialX,1,"SPA"," 10 50");   // velocita'
  SendCommand(fSerialX,1,"SPA"," 11 30");  //Accelerazione
  SendCommand(fSerialX,1,"SPA"," 117442049 MM");  //Unita'

  
  
  SendCommand(fSerialX,1,"SPA? 1 1",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 2",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 3",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 4",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 8",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 10",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 11",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 14",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 15",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 19",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 20",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 21",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 22",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 23",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 24",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 48",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 49",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 50",NULL);
  read_answer(fSerialX);
  SendCommand(fSerialX,1,"SPA? 1 60",NULL);
  read_answer(fSerialX);
  //SendCommand(fSerialX,1,"SPA? 1 74",NULL); ///per sapere la max accelerazione
  //read_answer(fSerialX);
  

  sleep(0.5);
  //SendCommand(fSerialX,1,"RON 1 1",NULL); //RON e' 1 di default!! 
  //Va messo 0 se si vuole referenziare l'asse con POS
  
  // SendCommand(fSerialX,1,"VEL?",NULL);
  //read_answer(fSerialX); 
  
  
  qDebug()<< "Cambio la velocita' X....."; 
  SendCommand(fSerialX,1,"VEL 1 1",NULL);   
  sleep(0.5);
  
  qDebug() << "Accendo servo X....."; 
  SendCommand(fSerialX,1,"SVO 1 1",NULL);   sleep(1);
  
  qDebug() << "Vado alla posizione X di riferimento ......"; 
  SendCommand(fSerialX,1,"FRF 1",NULL); 
  qDebug() << "Attendere........................";   
  

  if ( !TimerActive){
    timer->start(interval); 
    TimerActive = true;
  }
  fIsXInitialized = true; 
  qDebug() << " Fine dell'inizializzazione X" << "\n";
  IniX = 0; 
  XShmIniStatusWrite(); 
  IniXready = 1; 
  XShmReadyStatusWrite();
  
  
}




void MyWidget::InizializzazioneY(){

  const char *MYTTY_X;
  string b,c;
  fIsYInitialized = false;
  
  IniY=1; IniYready=0;
  YShmIniStatusWrite();
  
  XYShmPortsStatusRead();
  
  switch (fPortY) {
  case 0:
    MYTTY_X="/dev/ttyUSB0";
    break;
  case 1:
    MYTTY_X="/dev/ttyUSB1"; ////////////// sostituire USB con s se si vuole collegare a una seriale
    break;
  case 2:
    MYTTY_X="/dev/ttyUSB2";
    break;
  case 3:
    MYTTY_X="/dev/ttyUSB3";
    break;
  default: 
    cout << "Port " << fPortY << " not implemented " 
	 <<  " chose another one (< 4) " << endl;
  };
  
  errno=0;


  fSerialY = open(MYTTY_X,O_RDWR);  
  if ( fSerialY<0){
    qDebug()<<"ERROR opening"<< MYTTY_X<< strerror(errno)<<'\n';
    exit(-1);
  }

  struct termios my_termios;
  struct termios new_termios;

  tcgetattr(fSerialY, &my_termios );
 // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  //  my_termios.c_flag &= ~CBAUD;
  //  my_termios.c_flag |= B19200;

  int h=cfsetospeed(&my_termios,B9600);
  //  printf("speed=%d \n",h);
  my_termios.c_oflag &= (unsigned short)(~(ONLCR | OPOST));
  my_termios.c_cflag |= CLOCAL;
  //  my_termios.c_iflag = IXON | IXOFF;
  my_termios.c_lflag &= (unsigned short)(~(ICANON | ECHO | ISIG));  
  my_termios.c_cc[VMIN] = 1;
  // printf("oflag=%x\ncflag=%x\niflag=%x\nlflag=%x\n",my_termios.c_oflag,my_termios.c_cflag,my_termios.c_iflag, my_termios.c_lflag); 
  
  tcsetattr( fSerialY, TCSANOW, &my_termios );
  tcgetattr( fSerialY, &new_termios );
  //  printf("  speed=%d \n",h);
  
  //Inizializzazione X
  SendCommand(fSerialY,1,"SPA"," 60 M-404.8PD");  sleep(1);
  SendCommand(fSerialY,1,"SPA"," 1 180"); //primo numero canale SPA secondo numero valore
  SendCommand(fSerialY,1,"SPA"," 2 45");          
  SendCommand(fSerialY,1,"SPA"," 3 300");         
  SendCommand(fSerialY,1,"SPA"," 4 2000");       
  SendCommand(fSerialY,1,"SPA"," 14 4000");     
  SendCommand(fSerialY,1,"SPA"," 15 1"); ////0.064285257
  SendCommand(fSerialY,1,"SPA"," 19 0");          
  SendCommand(fSerialY,1,"SPA"," 20 1");        //si usano i limit switches dell'hardware (top e bottom) 
  SendCommand(fSerialY,1,"SPA"," 21 200.");       //Max spostamento in positivo da home (0)  
  SendCommand(fSerialY,1,"SPA"," 22 100.");   //Value at Reference position
  SendCommand(fSerialY,1,"SPA"," 23 100.");  //Distanza tra Ref e Neg-lim
  SendCommand(fSerialY,1,"SPA"," 47 100");    //Distanza tra Ref e Pos-lim
  SendCommand(fSerialY,1,"SPA"," 24 0");          //0 i limit switches sono attivi alti...va come 49
  SendCommand(fSerialY,1,"SPA"," 48 0.000000");  //Max spostamento in negativo da home (0) 
  SendCommand(fSerialY,1,"SPA"," 49 0");        //0 non si inverte il riferimento 
  SendCommand(fSerialY,1,"SPA"," 50 0");      //0 Abilita lo stop ai limit switches dell'hardware    
  SendCommand(fSerialY,1,"SPA"," 8 0.5");    //8 10 e 11 per ultimi senno' li ricambia 0.035156
  SendCommand(fSerialY,1,"SPA"," 10 50");   // velocita'
  SendCommand(fSerialY,1,"SPA"," 11 30");  //Accelerazione
  SendCommand(fSerialY,1,"SPA"," 117442049 MM");  //Unita'

  
  
  SendCommand(fSerialY,1,"SPA? 1 1",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 2",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 3",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 4",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 8",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 10",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 11",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 14",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 15",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 19",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 20",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 21",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 22",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 23",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 24",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 48",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 49",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 50",NULL);
  read_answer(fSerialY);
  SendCommand(fSerialY,1,"SPA? 1 60",NULL);
  read_answer(fSerialY);
  //SendCommand(fSerialY,1,"SPA? 1 74",NULL); ///per sapere la max accelerazione
  //read_answer(fSerialY);
  

  sleep(0.5);
  //SendCommand(fSerialY,1,"RON 1 1",NULL); //RON e' 1 di default!! 
  //Va messo 0 se si vuole referenziare l'asse con POS
  
  // SendCommand(fSerialY,1,"VEL?",NULL);
  //read_answer(fSerialY); 
  
  
  qDebug()<< "Cambio la velocita' Y....."; 
  SendCommand(fSerialY,1,"VEL 1 1",NULL);   
  sleep(0.5);
  
  qDebug() << "Accendo servo Y....."; 
  SendCommand(fSerialY,1,"SVO 1 1",NULL);   sleep(1);
  
  qDebug() << "Vado alla posizione Y di riferimento ......"; 
  SendCommand(fSerialY,1,"FRF 1",NULL); 
  qDebug() << "Attendere........................";   
  

  
  if ( !TimerActive){
    timer->start(interval); 
    TimerActive = true;
  }

  fIsYInitialized = true; 
  qDebug() << " Fine dell'inizializzazione Y";
  IniY = 0; 
  YShmIniStatusWrite(); 
  IniYready = 1; 
  YShmReadyStatusWrite();
  
}

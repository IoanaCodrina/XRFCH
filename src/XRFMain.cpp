#include <QApplication>
#include "XRFMotor.h"

////// NEW
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h>
#include <iostream>

#warning there is an funny variable here
#define SHMSZ     80


int main(int argc, char *argv[]){
  std::cout << "=====> Welcome to the XRF fun ! <====== " << std::endl;
  std::cout << "   Have a great experience with it! " << std::endl;
  
  QApplication app(argc, argv);
  MyWidget *widget = new MyWidget;
  widget->show();
  return app.exec();
}

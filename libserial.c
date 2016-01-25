#include <SerialStream.h>
#include <cstdlib>
#include <time.h>

using namespace LibSerial;

#define MAX_LINE_LENGTH 256

/**
 * Per farla funzionare installare libserial
 * 
 * sudo apt-get install libserial0 libserial-dev
 * 
 * Per compilare:
 * 
 * g++ libserial.c -lserial
 * 
 **/

int main (int argc, char** argv) {
		SerialStream arduino_serial;
		
		arduino_serial.Open("/dev/ttyACM0");
                arduino_serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
        
		char line[MAX_LINE_LENGTH];
        while(1) {	
			arduino_serial.getline(line, MAX_LINE_LENGTH);
			int value = atoi(line);
			printf("Value = %d  mm \n", value);


			//printf("Ciao \n");
		}
		
			
}

#include "X9C10X.h"
#include <SerialCommands.h>

const char* DEVICE_TYPE = "strikerMk2";
const int FW_VERSION = 10000; // 1.00.00
X9C103 pot(10000);

char serial_command_buffer[32];
SerialCommands serialCommands(&Serial, serial_command_buffer, sizeof(serial_command_buffer), "\n", " ");
int currentSpeed = 0;

void setup()
{
  pot.begin(2, 3, 4, 0);  // pulse, direction, select, position
  
  Serial.begin(9600);

  // Add commands
  serialCommands.SetDefaultHandler(commandUnrecognized);
  serialCommands.AddCommand(new SerialCommand("introduce", commandIntroduce));
  serialCommands.AddCommand(new SerialCommand("status", commandStatus));
  serialCommands.AddCommand(new SerialCommand("speed-get", commandSpeedGet));
  serialCommands.AddCommand(new SerialCommand("speed-set", commandSpeedSet));

  serialCommands.GetSerial()->write(0x07);
}


void loop() 
{
    serialCommands.ReadSerial();
}

void commandIntroduce(SerialCommands* sender)
{
    serial_printf(sender->GetSerial(), "%s,%d\n", DEVICE_TYPE, FW_VERSION);
}

void commandStatus(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "status,speed:%d\n", currentSpeed);
}

void commandUnrecognized(SerialCommands* sender, const char* cmd)
{
    serial_printf(sender->GetSerial(), "Unrecognized command [%s]\n", cmd);
}

void commandSpeedGet(SerialCommands* sender)
{
    serial_printf(sender->GetSerial(), "speed-get,%d\n", currentSpeed);
}

void commandSpeedSet(SerialCommands* sender)
{
    char* percentageStr = sender->Next();

    if (percentageStr == NULL) {
        sender->GetSerial()->println("Percentage parameter missing\n");
        return;
    }
  
    int percentage = atoi(percentageStr);
  
    pot.setPosition(percentage);
    currentSpeed = percentage;

    serial_printf(sender->GetSerial(), "speed-set,%d\n", currentSpeed);
}

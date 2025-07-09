/*
  MCF8316A Status Reading Example
  
  This example demonstrates how to read various status registers
  and monitor the motor controller's state in real-time.
  
  Features:
  - Read all fault registers
  - Monitor algorithm states
  - Display motor parameters
  - Show real-time voltage and current
  - Debug register access
*/

#include <MCF8316A_I2C.h>

MCF8316A_I2C motor;

// Timing variables
unsigned long lastStatusRead = 0;
unsigned long lastDetailedRead = 0;
const unsigned long STATUS_INTERVAL = 100;    // 100ms for basic status
const unsigned long DETAILED_INTERVAL = 1000; // 1s for detailed info

void setup() {
  Serial.begin(115200);
  Serial.println("MCF8316A Status Reading Example");
  Serial.println("================================");
  
  // Initialize with debug enabled
  if (!motor.begin()) {
    Serial.println("Failed to initialize MCF8316A!");
    while (1) delay(1000);
  }
  
  // Enable debug output to see register communications
  motor.enableDebug(Serial);
  
  Serial.println("\nReading Device ID...");
  uint16_t deviceId = motor.readRegister16(MCF8316A_DEVICE_ID);
  Serial.print("Device ID: 0x");
  Serial.println(deviceId, HEX);
  
  // Disable debug for cleaner output
  motor.disableDebug();
  
  // Configure motor for testing
  motor.clearFaults();
  motor.setMotorParameters(100, 100);
  motor.enableSpeedLoop();
  
  Serial.println("\nStarting continuous status monitoring...");
  Serial.println("Press any key to toggle motor on/off\n");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle serial input for motor control
  if (Serial.available()) {
    Serial.read(); // Clear the buffer
    toggleMotor();
  }
  
  // Fast status updates
  if (currentTime - lastStatusRead >= STATUS_INTERVAL) {
    lastStatusRead = currentTime;
    readBasicStatus();
  }
  
  // Detailed status updates
  if (currentTime - lastDetailedRead >= DETAILED_INTERVAL) {
    lastDetailedRead = currentTime;
    readDetailedStatus();
  }
}

void toggleMotor() {
  if (motor.isRunning()) {
    Serial.println("\n>>> Stopping motor...");
    motor.stop();
  } else {
    Serial.println("\n>>> Starting motor at 500 RPM...");
    motor.setSpeed(500);
    motor.start();
  }
}

void readBasicStatus() {
  // Create a status line that updates in place
  Serial.print("\r");
  
  // Algorithm state
  uint8_t algoState = motor.getAlgorithmState();
  Serial.print("State: ");
  switch(algoState) {
    case 0: Serial.print("IDLE    "); break;
    case 1: Serial.print("STARTUP "); break;
    case 2: Serial.print("RUNNING "); break;
    case 3: Serial.print("FAULT   "); break;
    default: Serial.print("UNKNOWN "); break;
  }
  
  // Speed
  Serial.print("| Speed: ");
  float speed = motor.getSpeed();
  Serial.print(speed, 0);
  Serial.print(" RPM   ");
  
  // Fault status
  Serial.print("| Fault: ");
  Serial.print(motor.isFault() ? "YES" : "NO ");
  
  Serial.print("     "); // Clear any remaining characters
}

void readDetailedStatus() {
  Serial.println("\n\n=== Detailed Status Report ===");
  
  // Read all status registers
  uint16_t gateFaults = motor.readRegister16(MCF8316A_GATE_DRIVER_FAULT_STATUS);
  uint16_t ctrlFaults = motor.readRegister16(MCF8316A_CONTROLLER_FAULT_STATUS);
  uint16_t algoStatus = motor.readRegister16(MCF8316A_ALGO_STATUS);
  uint16_t statStatus = motor.readRegister16(MCF8316A_STAT_STATUS);
  
  // Gate Driver Faults
  Serial.println("\nGate Driver Faults:");
  Serial.print("  Register value: 0x");
  Serial.println(gateFaults, HEX);
  if (gateFaults & 0x0001) Serial.println("  - VDS_OCP: Overcurrent detected");
  if (gateFaults & 0x0002) Serial.println("  - GDF: Gate driver fault");
  if (gateFaults & 0x0004) Serial.println("  - OTSD: Over-temperature shutdown");
  if (gateFaults & 0x0008) Serial.println("  - UVLO: Undervoltage lockout");
  if (gateFaults & 0x0010) Serial.println("  - VDS_HA: Phase A high-side VDS fault");
  if (gateFaults & 0x0020) Serial.println("  - VDS_LA: Phase A low-side VDS fault");
  if (gateFaults & 0x0040) Serial.println("  - VDS_HB: Phase B high-side VDS fault");
  if (gateFaults & 0x0080) Serial.println("  - VDS_LB: Phase B low-side VDS fault");
  if (gateFaults & 0x0100) Serial.println("  - VDS_HC: Phase C high-side VDS fault");
  if (gateFaults & 0x0200) Serial.println("  - VDS_LC: Phase C low-side VDS fault");
  
  // Controller Faults
  Serial.println("\nController Faults:");
  Serial.print("  Register value: 0x");
  Serial.println(ctrlFaults, HEX);
  if (ctrlFaults & 0x0001) Serial.println("  - LOCK: Motor lock detected");
  if (ctrlFaults & 0x0002) Serial.println("  - IPD_FAIL: Initial position detection failed");
  if (ctrlFaults & 0x0004) Serial.println("  - ABN_SPEED: Abnormal speed");
  if (ctrlFaults & 0x0008) Serial.println("  - NO_MTR: No motor detected");
  if (ctrlFaults & 0x0010) Serial.println("  - CBK_FLT: Current limit");
  
  // Algorithm Status
  Serial.println("\nAlgorithm Status:");
  Serial.print("  Register value: 0x");
  Serial.println(algoStatus, HEX);
  Serial.print("  State: ");
  uint8_t state = (algoStatus >> 13) & 0x07;
  switch(state) {
    case 0: Serial.println("IDLE"); break;
    case 1: Serial.println("STARTUP"); break;
    case 2: Serial.println("CLOSED_LOOP"); break;
    case 3: Serial.println("FAULT"); break;
    default: Serial.println("UNKNOWN"); break;
  }
  
  // Motor parameters
  uint16_t mtrParams = motor.readRegister16(MCF8316A_MTR_PARAMS);
  Serial.println("\nMotor Parameters:");
  Serial.print("  Resistance: ");
  Serial.print((mtrParams >> 8) & 0xFF);
  Serial.println(" mOhm");
  Serial.print("  Inductance: ");
  Serial.print(mtrParams & 0xFF);
  Serial.println(" uH");
  
  // Additional monitoring
  Serial.println("\nOperating Conditions:");
  Serial.print("  Voltage: ");
  Serial.print(motor.getVoltage());
  Serial.println(" V");
  Serial.print("  Current: ");
  Serial.print(motor.getCurrent());
  Serial.println(" A");
  
  // Statistical Status
  Serial.println("\nStatistical Status:");
  Serial.print("  Register value: 0x");
  Serial.println(statStatus, HEX);
  
  Serial.println("==============================\n");
}
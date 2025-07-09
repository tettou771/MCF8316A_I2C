/*
  MCF8316A Speed Control Example
  
  This example demonstrates advanced speed control features including:
  - Speed ramping
  - Speed profiles
  - Real-time speed monitoring
  - Dynamic speed adjustment via Serial input
  
  Serial Commands:
  - Enter a number (RPM) to set speed
  - 's' to stop
  - 'r' to reverse direction
  - 'f' to forward direction
*/

#include <MCF8316A_I2C.h>

MCF8316A_I2C motor;

// Speed control variables
float targetSpeed = 0;
float currentSpeed = 0;
bool motorRunning = false;
bool forwardDirection = true;

void setup() {
  Serial.begin(115200);
  Serial.println("MCF8316A Speed Control Example");
  Serial.println("Commands:");
  Serial.println("- Enter RPM value (0-3000)");
  Serial.println("- 's' to stop");
  Serial.println("- 'r' for reverse");
  Serial.println("- 'f' for forward");
  Serial.println("- 'i' for info");
  
  // Initialize motor controller
  if (!motor.begin()) {
    Serial.println("Failed to initialize MCF8316A!");
    while (1) delay(1000);
  }
  
  // Configure motor
  motor.clearFaults();
  motor.setMotorParameters(100, 100); // Adjust for your motor
  motor.enableSpeedLoop();
  motor.setSpeedRange(2); // 800Hz range for higher speeds
  
  // Configure speed profiles for smooth acceleration
  // Profile 0: Slow start (100 Hz/s acceleration)
  motor.setSpeedProfile(0, 100, 100);
  // Profile 1: Medium (200 Hz/s acceleration)
  motor.setSpeedProfile(1, 200, 200);
  // Profile 2: Fast (500 Hz/s acceleration)
  motor.setSpeedProfile(2, 500, 500);
  
  Serial.println("\nMotor initialized. Ready for commands.");
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    handleSerialCommand();
  }
  
  // Monitor motor status
  static unsigned long lastStatusTime = 0;
  if (millis() - lastStatusTime > 500) { // Update every 500ms
    lastStatusTime = millis();
    updateMotorStatus();
  }
  
  // Check for faults
  if (motor.isFault()) {
    handleFault();
  }
}

void handleSerialCommand() {
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  if (input.length() == 0) return;
  
  char firstChar = input.charAt(0);
  
  switch (firstChar) {
    case 's': // Stop
    case 'S':
      stopMotor();
      break;
      
    case 'r': // Reverse
    case 'R':
      setDirection(false);
      break;
      
    case 'f': // Forward
    case 'F':
      setDirection(true);
      break;
      
    case 'i': // Info
    case 'I':
      printMotorInfo();
      break;
      
    default:
      // Try to parse as speed value
      int speed = input.toInt();
      if (speed >= 0 && speed <= 3000) {
        setSpeed(speed);
      } else {
        Serial.println("Invalid command or speed value!");
      }
      break;
  }
}

void setSpeed(int rpm) {
  targetSpeed = rpm;
  
  Serial.print("Setting speed to ");
  Serial.print(rpm);
  Serial.println(" RPM");
  
  if (rpm > 0) {
    motor.setSpeed(rpm);
    if (!motorRunning) {
      motor.start();
      motorRunning = true;
      Serial.println("Motor started");
    }
  } else {
    stopMotor();
  }
}

void stopMotor() {
  Serial.println("Stopping motor...");
  motor.stop();
  motorRunning = false;
  targetSpeed = 0;
}

void setDirection(bool forward) {
  if (forward != forwardDirection) {
    // Stop motor before changing direction
    if (motorRunning) {
      stopMotor();
      delay(1000); // Wait for motor to stop
    }
    
    forwardDirection = forward;
    motor.setDirection(forward);
    Serial.print("Direction set to: ");
    Serial.println(forward ? "FORWARD" : "REVERSE");
    
    // Restart if we had a target speed
    if (targetSpeed > 0) {
      setSpeed(targetSpeed);
    }
  }
}

void updateMotorStatus() {
  if (!motorRunning) return;
  
  currentSpeed = motor.getSpeed();
  
  // Display status bar
  Serial.print("Speed: ");
  Serial.print(currentSpeed, 0);
  Serial.print(" / ");
  Serial.print(targetSpeed, 0);
  Serial.print(" RPM | Dir: ");
  Serial.print(forwardDirection ? "FWD" : "REV");
  Serial.print(" | State: ");
  
  uint8_t state = motor.getAlgorithmState();
  switch(state) {
    case 0: Serial.print("IDLE"); break;
    case 1: Serial.print("STARTUP"); break;
    case 2: Serial.print("RUNNING"); break;
    case 3: Serial.print("FAULT"); break;
    default: Serial.print("UNKNOWN"); break;
  }
  
  // Visual speed indicator
  Serial.print(" [");
  int barLength = map(currentSpeed, 0, 3000, 0, 20);
  for (int i = 0; i < 20; i++) {
    if (i < barLength) Serial.print("=");
    else Serial.print(" ");
  }
  Serial.println("]");
}

void handleFault() {
  Serial.println("\n!!! FAULT DETECTED !!!");
  
  uint16_t gateFaults = motor.getGateFaultStatus();
  uint16_t ctrlFaults = motor.getControllerFaultStatus();
  
  Serial.print("Gate driver faults: 0x");
  Serial.println(gateFaults, HEX);
  Serial.print("Controller faults: 0x");
  Serial.println(ctrlFaults, HEX);
  
  // Try to clear faults
  Serial.println("Attempting to clear faults...");
  motor.clearFaults();
  delay(100);
  
  // Stop motor
  stopMotor();
}

void printMotorInfo() {
  Serial.println("\n--- Motor Information ---");
  
  Serial.print("Current Speed: ");
  Serial.print(motor.getSpeed());
  Serial.println(" RPM");
  
  Serial.print("Direction: ");
  Serial.println(motor.getDirection() ? "FORWARD" : "REVERSE");
  
  Serial.print("Algorithm State: ");
  uint8_t state = motor.getAlgorithmState();
  switch(state) {
    case 0: Serial.println("IDLE"); break;
    case 1: Serial.println("STARTUP"); break;
    case 2: Serial.println("CLOSED_LOOP"); break;
    case 3: Serial.println("FAULT"); break;
    default: Serial.println("UNKNOWN"); break;
  }
  
  Serial.print("Running: ");
  Serial.println(motor.isRunning() ? "YES" : "NO");
  
  Serial.print("Fault: ");
  Serial.println(motor.isFault() ? "YES" : "NO");
  
  uint16_t resistance, inductance;
  motor.getMotorParameters(resistance, inductance);
  Serial.print("Motor Parameters - R: ");
  Serial.print(resistance);
  Serial.print(" mOhm, L: ");
  Serial.print(inductance);
  Serial.println(" uH");
  
  Serial.println("------------------------\n");
}
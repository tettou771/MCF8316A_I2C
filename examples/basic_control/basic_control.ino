/*
  MCF8316A Basic Control Example
  
  This example demonstrates basic motor control using the MCF8316A_I2C library.
  
  Wiring:
  - MCF8316A SDA -> Arduino SDA
  - MCF8316A SCL -> Arduino SCL
  - MCF8316A VCC -> 12-24V
  - MCF8316A GND -> GND (common with Arduino)
  - Motor phases -> OUTA, OUTB, OUTC
  
  Note: Make sure to have proper pull-up resistors (4.7k) on SDA and SCL lines.
*/

#include <MCF8316A_I2C.h>

// Create motor controller instance
MCF8316A_I2C motor;

void setup() {
  Serial.begin(115200);
  Serial.println("MCF8316A Basic Control Example");
  
  // Initialize I2C and motor controller
  if (!motor.begin()) {
    Serial.println("Failed to initialize MCF8316A!");
    Serial.println("Check wiring and I2C address.");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("MCF8316A initialized successfully!");
  
  // Enable debug output
  motor.enableDebug(Serial);
  
  // Clear any existing faults
  motor.clearFaults();
  
  // Set motor parameters (example values - adjust for your motor)
  // Resistance: 100 mOhm, Inductance: 100 uH
  motor.setMotorParameters(100, 100);
  
  // Enable speed control mode
  motor.enableSpeedLoop();
  
  // Set speed range (0 = 120Hz, 1 = 400Hz, 2 = 800Hz, 3 = 1200Hz)
  motor.setSpeedRange(1); // 400Hz range
  
  delay(1000);
}

void loop() {
  // Check for faults
  if (motor.isFault()) {
    Serial.println("Motor fault detected!");
    uint16_t faults = motor.getFaultStatus();
    Serial.print("Fault code: 0x");
    Serial.println(faults, HEX);
    
    // Clear faults and try again
    motor.clearFaults();
    delay(1000);
    return;
  }
  
  // Forward direction test
  Serial.println("\n--- Forward Direction Test ---");
  motor.setDirection(true);
  
  // Start motor at 500 RPM
  Serial.println("Starting motor at 500 RPM...");
  motor.setSpeed(500);
  motor.start();
  
  // Run for 5 seconds
  for (int i = 0; i < 5; i++) {
    delay(1000);
    float currentSpeed = motor.getSpeed();
    Serial.print("Current speed: ");
    Serial.print(currentSpeed);
    Serial.println(" RPM");
    
    // Print algorithm state
    uint8_t state = motor.getAlgorithmState();
    Serial.print("Algorithm state: ");
    switch(state) {
      case 0: Serial.println("IDLE"); break;
      case 1: Serial.println("STARTUP"); break;
      case 2: Serial.println("CLOSED_LOOP"); break;
      case 3: Serial.println("FAULT"); break;
      default: Serial.println("UNKNOWN"); break;
    }
  }
  
  // Increase speed to 1000 RPM
  Serial.println("\nIncreasing speed to 1000 RPM...");
  motor.setSpeed(1000);
  delay(3000);
  
  // Stop motor
  Serial.println("Stopping motor...");
  motor.stop();
  delay(2000);
  
  // Reverse direction test
  Serial.println("\n--- Reverse Direction Test ---");
  motor.setDirection(false);
  
  // Start motor at 300 RPM
  Serial.println("Starting motor at 300 RPM (reverse)...");
  motor.setSpeed(300);
  motor.start();
  
  // Run for 3 seconds
  delay(3000);
  
  // Brake test
  Serial.println("Applying brake...");
  motor.brake();
  delay(2000);
  
  // Coast test
  Serial.println("Coast to stop...");
  motor.coast();
  delay(3000);
  
  Serial.println("\n--- Test cycle complete ---\n");
  delay(5000);
}
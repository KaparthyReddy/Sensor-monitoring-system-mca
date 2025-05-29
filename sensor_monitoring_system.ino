const int GAS_SENSOR_PIN = A1;
const int FLAME_SENSOR_PIN = A3;
const int TEMP_SENSOR_PIN = A2;
const int LED_GREEN = 7;
const int LED_YELLOW = 6;
const int LED_RED1 = 5;
const int LED_RED2 = 4;
const int LED_FLAME = 9;
const int LED_TEMP = 12;
const int BUZZER_PIN = 3;

void setup() {
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED1, OUTPUT);
  pinMode(LED_RED2, OUTPUT);
  pinMode(LED_FLAME, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize all outputs to safe state
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED1, LOW);
  digitalWrite(LED_RED2, LOW);
  digitalWrite(LED_FLAME, LOW);
  digitalWrite(LED_TEMP, LOW);
  
  Serial.begin(9600);
  Serial.println("===== Sensor Monitoring System Started =====");
}

// Fixed gas level calculation
int computeGasLevel(int raw) {
  // Simple mapping from raw sensor value to percentage
  return map(raw, 0, 1023, 0, 100);
}

// Fixed gas hazard evaluation
bool evaluateGasHazard(int level) {
  // Hazard when gas level is high (above 60%)
  return level > 60;
}

// Fixed flame detection
bool interpretFlameSignal(int raw) {
  // Flame sensor typically gives LOW when flame detected
  // Adjust threshold based on your sensor
  return raw < 100;
}

// Fixed temperature calculation
float calculateTemperature(int pin) {
  // Read sensor multiple times for stability
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(pin);
    delay(10);
  }
  int avgReading = sum / 5;
  
  // Convert to voltage (assuming 5V reference)
  float voltage = (avgReading / 1024.0) * 5.0;
  
  // For LM35: 10mV per degree Celsius
  // For TMP36: (voltage - 0.5) * 100
  // Adjust formula based on your temperature sensor
  float temperature = voltage * 100.0; // For LM35
  
  return temperature;
}

// Fixed overheat check
bool checkOverheat(float temp) {
  // Overheat when temperature is above 40°C
  return temp > 40.0;
}

// Fixed LED indicators
void updateIndicators(int gasLevel, bool gasHazard, bool flameDetected, bool tempHigh) {
  // Green LED: All normal (no hazards)
  digitalWrite(LED_GREEN, !gasHazard && !flameDetected && !tempHigh);
  
  // Yellow LED: Low gas levels (warning)
  digitalWrite(LED_YELLOW, gasLevel < 20 && !gasHazard);
  
  // Red LEDs: Gas hazard levels
  digitalWrite(LED_RED1, gasLevel >= 40 && gasLevel < 70);
  digitalWrite(LED_RED2, gasLevel >= 70);
  
  // Flame LED: Flame detected
  digitalWrite(LED_FLAME, flameDetected);
  
  // Temperature LED: High temperature
  digitalWrite(LED_TEMP, tempHigh);
}

// Fixed alarm trigger
void triggerAlarm(bool gasHazard, bool flameDetected, bool tempHigh) {
  if (gasHazard || flameDetected || tempHigh) {
    // Sound alarm with beeping pattern
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
  } else {
    // No alarm needed
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// Fixed logging with correct status
void logStatus(int gasRaw, int gasLevel, int flameRaw, float temp, bool gasHazard, bool flameDetected, bool tempHigh) {
  Serial.println("-------- SENSOR READINGS --------");
  Serial.print("Raw MQ2 Value: ");
  Serial.print(gasRaw);
  Serial.print(" | Gas Concentration: ");
  Serial.print(gasLevel);
  Serial.println("%");
  
  Serial.print("Raw Flame Value: ");
  Serial.print(flameRaw);
  Serial.print(" | Flame Detected: ");
  Serial.println(flameDetected ? "YES" : "NO");
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C | Overheat: ");
  Serial.println(tempHigh ? "YES" : "NO");
  
  Serial.print("Status: ");
  if (!gasHazard && !flameDetected && !tempHigh) {
    Serial.println("ALL NORMAL");
  } else {
    Serial.print("ALERT - ");
    if (gasHazard) Serial.print("GAS ");
    if (flameDetected) Serial.print("FLAME ");
    if (tempHigh) Serial.print("TEMPERATURE ");
    Serial.println();
  }
  Serial.println("----------------------------------\n");
}

void loop() {
  // Read all sensors
  int gasRaw = analogRead(GAS_SENSOR_PIN);
  int gasLevel = computeGasLevel(gasRaw);
  bool gasHazard = evaluateGasHazard(gasLevel);
  
  int flameRaw = analogRead(FLAME_SENSOR_PIN);
  bool flameDetected = interpretFlameSignal(flameRaw);
  
  float temperature = calculateTemperature(TEMP_SENSOR_PIN);
  bool tempHigh = checkOverheat(temperature);
  
  // Update system state
  updateIndicators(gasLevel, gasHazard, flameDetected, tempHigh);
  triggerAlarm(gasHazard, flameDetected, tempHigh);
  logStatus(gasRaw, gasLevel, flameRaw, temperature, gasHazard, flameDetected, tempHigh);
  
  delay(2000); // Reduced delay for more responsive monitoring
}

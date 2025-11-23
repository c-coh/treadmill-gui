/*
 * Simple Serial Test Program for Arduino Mega
 * Blinks LED when serial commands are received
 *
 * Upload this to your Arduino, then send commands from your GUI
 * LED will blink when commands are received
 */

const int LED_PIN = 13; // Built-in LED on Arduino Mega

void setup()
{
    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize serial communication at 115200 baud
    Serial.begin(115200);

    // Wait for serial port to connect
    while (!Serial)
    {
        ; // Wait for serial port to connect (needed for native USB)
    }

    // Blink 3 times to show ready
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }

    Serial.println("INFO,ARDUINO_MEGA_TREADMILL_READY_NO_ESTOP_ENCODER_LIB");
    Serial.println("Arduino ready - LED will blink when commands received");
}

void loop()
{
    // Check if data is available to read
    if (Serial.available() > 0)
    {
        // Read the incoming command (until newline)
        String command = Serial.readStringUntil('\n');

        // Remove any trailing whitespace
        command.trim();

        // Echo back what was received (optional - for debugging)
        Serial.print("RECEIVED: ");
        Serial.println(command);

        // Parse SPD command
        if (command.startsWith("SPD,"))
        {
            // Remove "SPD," prefix
            String params = command.substring(4);

            // Find comma position
            int commaIndex = params.indexOf(',');

            if (commaIndex > 0)
            {
                // Extract left and right speed values
                String leftStr = params.substring(0, commaIndex);
                String rightStr = params.substring(commaIndex + 1);

                int leftSpeed = leftStr.toInt();
                int rightSpeed = rightStr.toInt();

                // Blink LED the number of times equal to leftSpeed value
                // Cap at 20 blinks to avoid excessive delays
                int blinkCount = min(leftSpeed, 20);

                for (int i = 0; i < blinkCount; i++)
                {
                    digitalWrite(LED_PIN, HIGH);
                    delay(100);
                    digitalWrite(LED_PIN, LOW);
                    delay(100);
                }

                Serial.print("  -> SPD command: Left=");
                Serial.print(leftSpeed);
                Serial.print(", Right=");
                Serial.print(rightSpeed);
                Serial.print(" (LED blinked ");
                Serial.print(blinkCount);
                Serial.println(" times)");
            }
        }
    }
}

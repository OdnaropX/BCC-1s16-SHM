#include <Servo.h>
#include <Stepper.h>

#define LED1 4
#define LED2 5
#define SERVO 6

#define LED_TIME 666

/* Blinks a LED */
void blink(int led);

// Game
int player; // Player turn
int incomingByte = 0; // For incoming serial data

// Servo
Servo servo_motor;
int i, pos = 0; // Servo position

// Stepper
const int stepsPerRevolution = 500; 
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11); // Initializes on ports 8 to 11

void setup() {
	// Servo
	servo_motor.attach(SERVO);
	servo_motor.write(0);

	// Stepper
	myStepper.setSpeed(60);

	// LEDs
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);

	// Serial
	Serial.begin(9600);
	Serial.println("IR Receiver Button Decode");
}

void loop() {
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);

    if (Serial.available() > 0) {
        // Read the incoming byte:
        incomingByte = Serial.read();

        Serial.print("First player to go: ");
        Serial.println(incomingByte, DEC);

        // Blinks player turn
        blink(incomingByte == 0x01 ? LED1 : LED2);
        player = incomingByte;
    }

	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);

    if (Serial.available() > 0) {
        // Read the incoming byte:
        incomingByte = Serial.read();

        Serial.print("Other player round: ");
        Serial.println(incomingByte, DEC);

        if (incomingByte == 0x01) {
        	// Switch player blinking LED 
        	blink(1-player);
        }
    }   

	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);

    if (Serial.available() > 0) {
        // Read the incoming byte:
        incomingByte = Serial.read();

        Serial.print("Result: ");
        Serial.println(incomingByte, DEC);

		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);

		if (incomingByte == 0x01) {
			// Roll to player 1
			for (i = 0; i < 90; i++) {
	        	servo_motor.write(pos++);
	        	delay(5);
	      	}
		} else if (incomingByte == 0x02) {
			// Roll to player 2
			for (i = 0; i < 90; i++) {
	        	servo_motor.write(pos--);
	        	delay(5);
	      	}
		}

		for (i = 0; i < 3; i++) 
			myStepper.step(1024);
      	
      	delay(3000);      
		
		// Reset positions
		if (incomingByte == 0x01) {
			// Roll to player 1
			for (i = 0; i < 90; i++) {
	        	servo_motor.write(pos--);
	        	delay(5);
	      	}
		} else if (incomingByte == 0x02) {
			// Roll to player 2
			for (i = 0; i < 90; i++) {
	        	servo_motor.write(pos++);
	        	delay(5);
	      	}
		}

		for (i = 0; i < 3; i++) 
			myStepper.step(-1024);
    }
}

void blink(int led) {
	digitalWrite(led, HIGH);  
	delay(LED_TIME);
	digitalWrite(led, LOW);  
	delay(LED_TIME);

	digitalWrite(led, HIGH);  
	delay(LED_TIME);
	digitalWrite(led, LOW);  
	delay(LED_TIME);

	digitalWrite(led, HIGH);
}

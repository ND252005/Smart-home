
void setup() {
	pinMode(2, OUTPUT);
	Serial.begin(57600);
}
String str = "";
void loop() {
	// if(Serial.available()) {
	//       char ch = Serial.read();
	//       if(ch != '\n') {
	//           str += ch;
	//       } else {
	// 		Serial.println(str);
	// 		if(str.charAt(0) == 'T') digitalWrite(2, 1);
	// 		if(str.charAt(0) == 'F') digitalWrite(2, 0);
	// 		str = "";
	//       }
	// }
	digitalWrite(2, 1);
	delay(500);
	
	digitalWrite(2, 0);
	delay(500);

}

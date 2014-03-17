#include <SPI.h>
#include <WiFly.h>

// Change this to match the adhoc network ssid
char ssid[] = "Adhoc";

WiFlyServer server(80);
boolean current_line_is_blank = false;

void setup() {
	
	Serial.begin(9600);
	Serial.println("WiFly Shield AdHoc Example");

	WiFly.setUart(&Serial);
	WiFly.begin(true);
	
	
	//Comment this to run createAdHocNetwork() in order for the adhoc network to be available for the 1st time
	if(!strcmp(WiFly.ip(), "169.254.1.1")) {
		Serial.println("Already in Ad Hoc mode!");
	}

	else if(!WiFly.createAdHocNetwork(ssid)) {
		Serial.println("Failed to create network, handle error properly?");
		while(1) {
			// Hang on failure.
		}
	}
	
	Serial.println("Network Created");
	Serial.print("IP: ");
	Serial.println(WiFly.ip());
	
	Serial.println("Starting Server");
	server.begin();
}

void loop() {

	WiFlyClient client = server.available();
	char ssid [30];
	char passphrase[30];
	char header[400];
	int index = 0;
	
	if (client) {
		// an http request ends with a blank line

		while (client.connected()) {

			if (client.available()) {
				delay(1);
				char c = client.read();
				header[index++] = c;

// 				if we've got to the end of the line (received a newline
// 				character) and the line is blank, the http request has ended,
// 				so we can send a reply
				if (c == '\n' && current_line_is_blank) {
					header[index] = 0;
					if (strstr(header, "GET") != NULL) {
						send_form(client);
						break;
					}
					else if (strstr(header, "POST") != NULL) {
						get_credentials(client, ssid, passphrase);
						delay(10);
						client.print("SSID:");
						client.println(ssid);
						client.print("PASSPHRASE:");
						client.println(passphrase);
						break;
					}
				}
				if (c == '\n') {
					// we're starting a new line
					current_line_is_blank = true;
				} 
				else if (c != '\r') {
					// we've gotten a character on the current line
					current_line_is_blank = false;
				}
			}
		}
	// give the web browser time to receive the data
	delay(100);

	client.stop();
	}
}

void send_form(WiFlyClient client)
{
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println();

	client.println("<form name=\"input\" action=\"\" method=\"post\">");
	client.println("SSID: <input type=\"text\" name=\"ssid\"> ");
	client.println("Passphrase: <input type=\"password\" name=\"passphrase\">");
	client.println("<input type=\"submit\" value=\"Submit\">");
	client.println("</form>");
}

void get_credentials(WiFlyClient client, char * ssid, char * passphrase)
{
	char buffer[60];

	for (int i = 0; i < 60; i++) {
		if(client.available()) {
			delay(1);
			char s = client.read();
			buffer[i] = s;
		}
		else {
			buffer[i] = 0;
			sscanf(buffer, "%*[^=]%*c%[^&]%*[^=]%*c%s", ssid, passphrase);
			break;
		}
	}
}
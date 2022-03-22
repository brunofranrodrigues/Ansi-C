
char sistema[32] = {0};

char estado[32] = {0};

static byte estadoSTR = 0;

static byte estadoSTR1 = 0;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

boolean newData = false;

void setup() {
  Serial.begin(115200);

}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }

}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


void parseData() {

  // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(receivedChars, ":");     // get the first part - the string
  strcpy(sistema, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ":");     // get the first part - the string
  strcpy(estado, strtokIndx); // copy it to messageFromPC

}

void showParsedData() {
  //Serial.println("sistema");
  //Serial.println(sistema);
  //Serial.println("estado");
  //Serial.println(estado);
  if ((strcmp(sistema, "tank") == 0) && (strcmp(estado, "ON") == 0)) {
    estadoSTR = 1;
    //Serial.println(estadoSTR);
  } else {
    estadoSTR = 2;
    //Serial.println(estadoSTR);
  }

  if ((strcmp(sistema, "irrigation") == 0) && (strcmp(estado, "ON") == 0)) {
    estadoSTR1 = 1;
    //Serial.println(estadoSTR1);
  } else {
    estadoSTR1 = 2;
    //Serial.println(estadoSTR1);
  }


}

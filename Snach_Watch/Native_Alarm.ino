/*Copyright 2015 Giorgio Gross
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
byte NA_ALARM_LAYOUT [20];
int NA_ALARM_LAYOUT_LEN = 20;
byte NA_ALARM_CONTENT [40];
int NA_ALARM_CONTENT_LEN = 40;
byte NA_ALARM_COVER [20];
int NA_ALARM_COVER_LEN = 20;
int ALARM_TIME_HOUR = 0;
int ALARM_TIME_MIN = 0;
boolean ALARM_ENABLED = false;
boolean na_alarm_isHourSelected = true;
boolean na_alarm_isMinuteSelected = false;

void overrideScreenIndex_NA_Alarm(uint8_t newScreenIndex) {
  // Overrides the screen index so that it can be displayed when
  // the native app is called by the phone.
  // Should be reset when connection is lost...

  NA_ALARM_CONTENT[1] = newScreenIndex;
}

void initializeNativeAlarmApp() {
  // Cover:
  NA_ALARM_COVER [0] = 3;
  NA_ALARM_COVER [1] = 1;
  NA_ALARM_COVER [2] = 1;
  NA_ALARM_COVER [3] = 0;
  
  int bCursor = 4;
  char coverTitle[] = "Alarm";
  int titleLen = 5;
  for(int b = 0; b < titleLen; b++){
    NA_ALARM_COVER[bCursor] = coverTitle[b];
    bCursor++;
  }
  NA_ALARM_COVER [bCursor] = 254;
  
  // Layout:
  NA_ALARM_LAYOUT[0] = 2;
  NA_ALARM_LAYOUT [1] = 0;
  NA_ALARM_LAYOUT [2] = 2;
  NA_ALARM_LAYOUT [3] = 0;
  NA_ALARM_LAYOUT [4] = 4;
  NA_ALARM_LAYOUT [5] = 37;
  NA_ALARM_LAYOUT [6] = 0;
  NA_ALARM_LAYOUT [7] = 1;
  NA_ALARM_LAYOUT [8] = 2;
  NA_ALARM_LAYOUT [9] = 6;
  NA_ALARM_LAYOUT [10] = 3;
  NA_ALARM_LAYOUT [11] = 7;
  NA_ALARM_LAYOUT [12] = 0;
  NA_ALARM_LAYOUT [13] = 0;
  NA_ALARM_LAYOUT [14] = 0;
  NA_ALARM_LAYOUT [15] = 0;
  NA_ALARM_LAYOUT [16] = 8;
  NA_ALARM_LAYOUT [17] = 8;
  NA_ALARM_LAYOUT [18] = 8;
  NA_ALARM_LAYOUT [19] = 8;
  
  Serial.println("asigning array for native app. MODE:");
  Serial.println(NA_ALARM_LAYOUT [0]);

// Content:
  updateNA_Alarm_Content(ALARM_TIME_HOUR, ALARM_TIME_MIN);
}

void updateNA_Alarm_Content(int h, int m) {
  NA_ALARM_CONTENT [0] = 253;
  NA_ALARM_CONTENT [1] = 1;
  NA_ALARM_CONTENT [2] = 17;
  int bCursor = 3;
  char HOUR [4];
  memset(&HOUR, 0, 2);
  Serial.println(HOUR);
  Serial.println(ALARM_TIME_HOUR);
  itoa(ALARM_TIME_HOUR, HOUR, 10);
  if(ALARM_TIME_HOUR < 10){
    HOUR[1] = HOUR[0];
    HOUR[0] = '0';
  }
  
  char MINUTE [4];
  memset(&MINUTE, 0, 2);
  Serial.println(MINUTE);
  Serial.println(ALARM_TIME_MIN);
  itoa(ALARM_TIME_MIN, MINUTE, 10);
  if(ALARM_TIME_MIN < 10){
    MINUTE[1] = MINUTE[0];
    MINUTE[0] = '0';
  }
  Serial.println("#############");
  Serial.println(HOUR);
  Serial.println(MINUTE);
  char title [5];
  title[0] = HOUR[0];
  title[1] = HOUR[1];
  title[2] = ':';
  title[3] = MINUTE[0];
  title[4] = MINUTE[1];
  char content [] = "Alarm";
  for (int b = 0; b < sizeof(title) / sizeof(char); b++) {
    NA_ALARM_CONTENT [bCursor] = title [b];
    bCursor++;
  }
  NA_ALARM_CONTENT[bCursor] = 18;
  bCursor++;
  NA_ALARM_CONTENT[bCursor] = 2;
  bCursor++;
  for (int b = 0; b < sizeof(content) / sizeof(char); b++) {
    NA_ALARM_CONTENT [bCursor] = content [b];
    bCursor++;
  }
  NA_ALARM_CONTENT[bCursor] = 3;
  bCursor++;
  NA_ALARM_CONTENT[bCursor] = 254;
}

void activateNA_Alarm() {
  // will call the BLE_Managers methods just as if a new byte buffer was delivered, but inserts the
  // alarm apps data.
  processAssembledData(NA_ALARM_COVER, NA_ALARM_COVER_LEN);
  processAssembledData(NA_ALARM_LAYOUT, NA_ALARM_LAYOUT_LEN);
  processAssembledData(NA_ALARM_CONTENT, NA_ALARM_CONTENT_LEN);
}

void handleNA_AlarmAction(int buttonID) {
  switch(buttonID){
    case 1:
      // top button:
      if(na_alarm_isHourSelected){
        ALARM_TIME_HOUR++;
        if(ALARM_TIME_HOUR > 23){
          ALARM_TIME_HOUR = 0;
        }
      } else if(na_alarm_isMinuteSelected){
        ALARM_TIME_MIN+=5;
        if(ALARM_TIME_MIN > 59){
          ALARM_TIME_MIN = 0;
        }
      }
      updateNA_Alarm_Content(ALARM_TIME_HOUR, ALARM_TIME_MIN);
      processAssembledData(NA_ALARM_LAYOUT, NA_ALARM_LAYOUT_LEN);
      processAssembledData(NA_ALARM_CONTENT, NA_ALARM_CONTENT_LEN);
      break;
    case 2:
      // right button:
      if(na_alarm_isHourSelected){
        na_alarm_isHourSelected = false;
        na_alarm_isMinuteSelected = true;
      } else {
        na_alarm_isHourSelected = true;
        na_alarm_isMinuteSelected = false;
      }
      break;
    case 3:
      // bottom button:
      if(na_alarm_isHourSelected){
        ALARM_TIME_HOUR--;
        if(ALARM_TIME_HOUR < 0){
          ALARM_TIME_HOUR = 23;
        }
      } else if(na_alarm_isMinuteSelected){
        ALARM_TIME_MIN-=5;
        if(ALARM_TIME_MIN < 0){
          ALARM_TIME_MIN = 55;
        }
      }
      updateNA_Alarm_Content(ALARM_TIME_HOUR, ALARM_TIME_MIN);
      processAssembledData(NA_ALARM_LAYOUT, NA_ALARM_LAYOUT_LEN);
      processAssembledData(NA_ALARM_CONTENT, NA_ALARM_CONTENT_LEN);
      break;
    case 4:
      // left button:
      if(ALARM_ENABLED){
        ALARM_ENABLED = false;
      } else {
        ALARM_ENABLED = true;
      }
      break;
    
  }
}

void checkNA_Alarm(){
  Serial.println("Alarm Clock Check (Native) :");
  Serial.println(ALARM_ENABLED);
  Serial.println(HOUR);
  Serial.println(ALARM_TIME_HOUR);
  Serial.println(MINUTE);
  Serial.println(ALARM_TIME_MIN);
  if(ALARM_ENABLED && HOUR == ALARM_TIME_HOUR && MINUTE == ALARM_TIME_MIN){
    // DO ALARM
    setVibPatternCall();
  }
}



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
#define VIB 34
#define VIB_POPUP 1
#define VIB_CALL 2
#define VIB_NOTIF 3

int loopVibCounter = 0;

void initalizeVibration(){
  pinMode(VIB, OUTPUT);
  digitalWrite(VIB, LOW);
}

void resetVibCounter(){
  loopVibCounter = 0;
}

void setVibPatternPopup(){
  CURRENT_VIB_PATTERN = VIB_POPUP;
}

void setVibPatternCall(){
  CURRENT_VIB_PATTERN = VIB_CALL;
}

void setVibPatternNotif(){
  CURRENT_VIB_PATTERN = VIB_NOTIF;
}

void handleVibrationPattern(){
  switch(CURRENT_VIB_PATTERN){
    case VIB_POPUP:
      if((loopVibCounter < 7) || (loopVibCounter > 12 && loopVibCounter < 19)){
        digitalWrite(VIB, HIGH);
      } else {
        digitalWrite(VIB, LOW);
      }
      loopVibCounter++;
      if(loopVibCounter >= 10){
        CURRENT_VIB_PATTERN = 0;
        resetVibCounter();
      }
      break;
    case VIB_CALL:
      digitalWrite(VIB, HIGH);
      break;
    case VIB_NOTIF:
      if((loopVibCounter < 4) || (loopVibCounter > 9 && loopVibCounter < 13)){
        digitalWrite(VIB, HIGH);
      } else {
        digitalWrite(VIB, LOW);
      }
      loopVibCounter++;
      if(loopVibCounter >= 5){// .... 13?
        CURRENT_VIB_PATTERN = 0;
        resetVibCounter();
      }
      break;
  }
}

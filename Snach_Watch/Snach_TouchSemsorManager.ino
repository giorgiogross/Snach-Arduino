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
int TOUCH_TOP = 43;
int TOUCH_MID = 41;
int TOUCH_BOTTOM = 39;

boolean t_top = false;
boolean t_mid = false;
boolean t_bottom = false;

void initializeTouch() {
  pinMode(TOUCH_TOP, INPUT);
  pinMode(TOUCH_MID, INPUT);
  pinMode(TOUCH_BOTTOM, INPUT);
}

void printTouchStatus() {
  Serial.println();
  Serial.print("TOUCH INPUT:   TOP: ");
  Serial.print(digitalRead(TOUCH_TOP));
  Serial.print("    MID: ");
  Serial.print(digitalRead(TOUCH_MID));
  Serial.print("    BOTTOM: ");
  Serial.print(digitalRead(TOUCH_BOTTOM));
  Serial.println();
}

int checkTouchStatus() {
  if (digitalRead(TOUCH_TOP) == HIGH && !t_top) {
    IS_APP_ACTIVE = false;
    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = true;
    t_top = true;
    return 1;
  } else if(digitalRead(TOUCH_TOP) == LOW) {
    t_top = false;
  }
  if (digitalRead(TOUCH_BOTTOM) == HIGH && !t_bottom) {
    IS_APP_ACTIVE = false;
    DO_SCREEN_TRANSITION_UP = true;
    DO_SCREEN_TRANSITION_DOWN = false;
    t_bottom = true;
    return 2;
  } else if(digitalRead(TOUCH_BOTTOM) == LOW){
    t_bottom = false;
  }
  if (digitalRead(TOUCH_MID) == HIGH) {
    IS_APP_ACTIVE = false;
    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
    return 3;
  }

  return 0;
}

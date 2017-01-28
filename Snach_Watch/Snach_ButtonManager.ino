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
int BUTTON_TOP = 28;
int BUTTON_RIGHT = 24;
int BUTTON_BOTTOM = 22;
int BUTTON_LEFT = 26;

boolean isPressedTop = false;
boolean isPressedRight = false;
boolean isPressedBottom = false;
boolean isPressedLeft = false;

void initializeButtons(){
  pinMode(BUTTON_TOP, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_BOTTOM, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
}

int checkSnachButtonState(){
  if(digitalRead(BUTTON_TOP) == HIGH && !isPressedTop){
    isPressedTop = true;
    return 1;
  } else if (digitalRead(BUTTON_TOP) == LOW) {
    isPressedTop = false;
  }
  if(digitalRead(BUTTON_RIGHT) == HIGH && !isPressedRight){
    isPressedRight = true;
    return 2;
  } else if(digitalRead(BUTTON_RIGHT) == LOW) {
    isPressedRight = false;
  }
  if(digitalRead(BUTTON_BOTTOM) == HIGH && !isPressedBottom){
    isPressedBottom = true;
    return 3;
  } else if(digitalRead(BUTTON_BOTTOM) == LOW) {
    isPressedBottom = false;
  }
  if(digitalRead(BUTTON_LEFT) == HIGH && !isPressedLeft){
    isPressedLeft = true;
    return 4;
  } else if(digitalRead(BUTTON_LEFT) == LOW) {
    isPressedLeft = false;
  }
  
  return 0;
}

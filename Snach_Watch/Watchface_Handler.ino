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
void restoreWatchface() {
  // Restores the cached watchface:
  // todo..
}

void setUpWatchFace(byte watchData[], int len_data) {
  
  SNACH_CURRENT_WATCHFACE_TYPE = watchData[2];
  switch (SNACH_CURRENT_WATCHFACE_TYPE) {
    case 1:
      // Watch White
      setUpWatch(COLOR_WHITE, watchData, len_data);
      break;
    case 2:
      // Watch Black
      setUpWatch(COLOR_BLACK, watchData, len_data);
      break;
    case 3:
      // Custom/dot Matrix White
      break;
    case 4:
      // Custom/dot Matrix Black
      break;
  }
}

void setUpWatch(int backgroundColor, byte watchData[], int len_data) {
  int watchCursorPosition = 3; // Positon where WatchDataArray is currently read.
  int watchItemCounter = 0; // Counts the amount of items.
  
  // TODO clear old struct Watchface..
    
  COLOR_WATCH_BACKGROUND = backgroundColor;
  
  boolean error = true;
  while (watchCursorPosition < len_data) {

    if (ITEM_SEPARATOR_RECEIVED_DATA == watchData[watchCursorPosition]) {
      // Next Item
      watchItemCounter++;
      watchCursorPosition++;
    } else if (EOF_RECEIVED_DATA == watchData[watchCursorPosition]) {
      // End reached, print watch
      error = false;
      break;
    } else {
      // Continue reading data
          
      int itemCursorPosition = 0;// Position where object is currently read.
      switch(watchData[watchCursorPosition]){
        case 1:
          // Clockface
          
          // get rid of idetifyer byte:
          watchCursorPosition++;
          CLOCK_FACE newClockFace;
          
          while (watchData[watchCursorPosition] != ITEM_SEPARATOR_RECEIVED_DATA && watchData[watchCursorPosition] != EOF_RECEIVED_DATA) {
            addClockFaceData(&newClockFace, (uint8_t) watchData[watchCursorPosition], itemCursorPosition);
            Serial.println("    Adding new Data to Clock Face: ");
            Serial.print(watchData[watchCursorPosition]);
            Serial.print("      at index: ");
            Serial.print(watchCursorPosition);
            Serial.print("      at item cursor: ");
            Serial.print(itemCursorPosition);
            Serial.print("      as watch item number ");
            Serial.print(watchItemCounter);
  
            watchCursorPosition++;
            itemCursorPosition++;
          }
  
          // If Clock Face was specified:
          if (itemCursorPosition > 1 && mWatchface.clockFace_len < AMOUNT_WATCHFACE_CLOCKFACES) {
            mWatchface.clockFaces[mWatchface.clockFace_len] = newClockFace;
            mWatchface.clockFace_len++;
          }
          
          break;
        case 2:
          // Textface
          
          // get rid of idetifyer byte:
          watchCursorPosition++;
          TEXT_FACE newTextFace;
          
          while(watchData[watchCursorPosition] != ITEM_SEPARATOR_RECEIVED_DATA && watchData[watchCursorPosition] != EOF_RECEIVED_DATA) {
            addTextFaceData(&newTextFace, (uint8_t) watchData[watchCursorPosition], itemCursorPosition);
            Serial.println("      Adding new Data to Watchface Textface: ");
            Serial.print(watchData[watchCursorPosition]);
            Serial.print("      at index: ");
            Serial.print(watchCursorPosition);
            Serial.print("      at item cursor: ");
            Serial.print(itemCursorPosition);
            Serial.print("      as watch item number ");
            Serial.print(watchItemCounter);
  
            watchCursorPosition++;
            itemCursorPosition++;
          }
          
          // If TextFace was specified:
          if (itemCursorPosition > 1 && mWatchface.textFace_len < AMOUNT_WATCHFACE_TEXTS) {
            mWatchface.textFaces[mWatchface.textFace_len] = newTextFace;
            mWatchface.textFace_len++;
          }
          
          break;
        case 3:
          // Object
          
          // get rid of idetifyer byte:
          watchCursorPosition++;
          WATCHFACE_OBJECT newObject;
          
          while (watchData[watchCursorPosition] != ITEM_SEPARATOR_RECEIVED_DATA && watchData[watchCursorPosition] != EOF_RECEIVED_DATA) {
            addWatchObjectData(&newObject, (uint8_t) watchData[watchCursorPosition], itemCursorPosition);
            Serial.println("      Adding new Data to Watchface Object: ");
            Serial.print(watchData[watchCursorPosition]);
            Serial.print("      at index: ");
            Serial.print(watchCursorPosition);
            Serial.print("      at item cursor: ");
            Serial.print(itemCursorPosition);
            Serial.print("      as watch item number ");
            Serial.print(watchItemCounter);
  
            watchCursorPosition++;
            itemCursorPosition++;
          }
  
          // If Object was specified:
          if (itemCursorPosition > 1 && mWatchface.watchObject_len < AMOUNT_WATCHFACE_OBJECTS) {
            mWatchface.watchObjects[mWatchface.watchObject_len] = newObject;
            mWatchface.watchObject_len++;
            Serial.println("object specified");
          }
          
          break;
        default:
          // TODO.. (?)
          return;
      }
    }
  }
  if (!error) {
    // hasDrawnWatchBack = false;
    drawWatface(&mWatchface);
  }
}

void addClockFaceData(struct CLOCK_FACE *pClk, uint8_t data, int itemCursor) {
  switch (itemCursor) {
    case 0:
      pClk->clock_radius = data;
      break;
    case 1:
      pClk->clock_type = data;
      break;
    case 2:
      pClk->clock_maxCount = data;
      break;
    case 3:
      pClk->clock_printNobject = data;
      break;
    case 4:
      pClk->clock_printNobjectStart = data;
      break;
    case 5:
      pClk->clock_midX = data;
      break;
    case 6:
      pClk->clock_midY = data;
      break;
    case 7:
      pClk->clock_color = data;
      break;
    case 8:
      pClk->clock_tokenDirection = data;
      break;
    case 9:
      pClk->clock_tokenWidth = data;
      break;
    case 10:
      pClk->clock_tokenHeight = data;
      break;
    case 11:
      pClk->clock_tokenDisposal = data;
      break;
    case 12:
      pClk->clock_layer = data;
      break;
    case 13:
      pClk->clock_move = data;
      break;
    case 14:
      pClk->clock_moveInterval = data;
      break;
    case 15:{
      uint8_t denom = data & 0x0f;
      uint8_t num = data >> 4;
      pClk->clock_moveDenominator = denom;
      pClk->clock_moveNumerator = num;
      break;
    }
    case 16:
      pClk->clock_moveRelToX = data;
      break;
    case 17:
      pClk->clock_moveRelToY = data;
      break;
    case 18:{
      uint8_t denom = data & 0x0f;
      uint8_t num = data >> 4;
      pClk->clock_moveResetDenominator = denom;
      pClk->clock_moveResetNumerator = num;
      break;
    }
    case 19:
      pClk->clock_movePace = data;
      break;
    case 20:
      pClk->clock_movePaceMultiplyerExponent = data;
      break;
  }
}

void addWatchObjectData(struct WATCHFACE_OBJECT *pObj, uint8_t data, int itemCursor) {
  switch (itemCursor) {
    case 0:
      pObj->object_shape = data;
      break;
    case 1:
      pObj->object_startX = data;
      break;
    case 2:
      pObj->object_startY = data;
      break;
    case 3:
      pObj->object_endX = data;
      break;
    case 4:
      pObj->object_endY = data;
      break;
    case 5:
      pObj->object_radius = data;
      break;
    case 6:
      pObj->object_color = data - 1;
      break;
    case 7:
      pObj->object_layer = data;
      break;
    case 8:
      pObj->object_move = data;
      break;
    case 9:{
      uint8_t denom = data & 0x0f;
      uint8_t num = data >> 4;
      pObj->object_moveDenominator = denom;
      pObj->object_moveNumerator = num;
      break;
    }
    case 10:
      pObj->object_moveInterval = data;
      break;
    case 11:
      pObj->object_moveRelToX = data;
      break;
    case 12:
      pObj->object_moveRelToY = data;
      break;
    case 13:{
      uint8_t denom = data & 0x0f;
      uint8_t num = data >> 4;
      pObj->object_moveResetDenominator = denom;
      pObj->object_moveResetNumerator = num;
      break;
    }
    case 14:
      pObj->object_movePace = data;
      break;
    case 15:
      pObj->object_movePaceMultiplyerExponent = data;
      break;
  }
}

void addTextFaceData(struct TEXT_FACE *pTxt, uint8_t data, int itemCursor) {
    switch(itemCursor){
      case 0:
        pTxt->text_mark = data;
        break;
      case 1:
        pTxt->text_size = data;
        break;
      case 2:
        pTxt->text_format = data;
        break;
      case 3:
        pTxt->text_color = data;
        break;
      case 4:
        pTxt->text_layer = data;
        break;
    }
}


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
#define S_SCK 13
#define S_MOSI 11
#define S_SS 46
#define DSP 7

#define COLOR_BLACK 0
#define COLOR_WHITE 1

Adafruit_SharpMem display(S_SCK, S_MOSI, S_SS);

void initializeDisplay() {
  // pinMode(DSP,OUTPUT);
  // digitalWrite(DSP,HIGH);
  display.begin();
  display.clearDisplay();
}

void refreshDisplay() {
  display.refresh();
}

void printTime(int hour, int minutes) {
  display.fillRect(0, 0, display.width(), display.height(), COLOR_BLACK);
  //display.refresh();

  display.setTextColor(COLOR_WHITE);
  display.setCursor(5, 5);
  display.setTextSize(5);
  display.println(hour);
  //display.setCursor(5,50);
  display.println(minutes);
  display.refresh();
}

void drawWhiteRect() {
  display.fillRect(0, 0, display.width(), display.height(), COLOR_WHITE);
  display.refresh();
}

void drawBlackRect() {
  display.fillRect(0, 0, display.width(), display.height(), COLOR_BLACK);
  display.refresh();
}

void drawWatface(struct WATCHFACE *pW) {
  /**
  This method prints all elements having the same layer the same time on the screen untill it cannot find a new layer.
  Bear in mind that layers must be ongoing and must not be broken. Layer indexes like "1", "2", "4" are invalid,
  instead it should be "1","2","3" !

    Elements of the watch face need to specifiy a layer starting with the lowest layer index.
    E.g. if there are two texts, the first text specified needs to have the lower layer index of the two.
    This must be minded for all elements of the watch face!
  */
  int currentLayer = 1;
  boolean foundMatchForLayer = false;

  while (currentLayer > 0) {
    for (int cf = 0; cf < AMOUNT_WATCHFACE_CLOCKFACES; cf++) {
      uint8_t clockLayer = pW -> clockFaces[cf].clock_layer;
      if (clockLayer == currentLayer) {
        // Draw the clock face:
        drawClockFace(pW -> clockFaces[cf]);
        foundMatchForLayer = true;
      } else if (clockLayer > currentLayer) {
        // Break if element is dedicated to a higher layer index:
        break;
      }
    }

    for (int tf = 0; tf < AMOUNT_WATCHFACE_TEXTS; tf++) {
      uint8_t textLayer = pW -> textFaces[tf].text_layer;
      if (textLayer == currentLayer) {
        // Draw the text:

        // TODO
        foundMatchForLayer = true;
      } else if (textLayer > currentLayer) {
        // Break if element is dedicated to a higher layer index:
        break;
      }
    }

    for (int wo = 0; wo < AMOUNT_WATCHFACE_OBJECTS; wo++) {
      uint8_t objectLayer = pW -> watchObjects[wo].object_layer;
      if (objectLayer == currentLayer) {
        // Draw object:
        drawWatchObject(pW -> watchObjects[wo]);
        foundMatchForLayer = true;
      } else if (objectLayer > currentLayer) {
        // Break if element is dedicated to a higher layer index:
        break;
      }
    }

    if (!foundMatchForLayer) {
      currentLayer = -1;
      // display.refresh(); // -> is called by mainloop
      break;
    } else {
      // Draw all elements created previously:
      currentLayer++;
      foundMatchForLayer = false;
    }

  }
}

void drawClockFace(struct CLOCK_FACE mClockFace) {
  boolean isEOI = false;
  uint8_t RADIUS = (mClockFace.clock_radius &&  mClockFace.clock_radius != 0) ?  mClockFace.clock_radius : 64;
  uint8_t TYPE = (mClockFace.clock_type &&  mClockFace.clock_type != 0) ?  mClockFace.clock_type : 1;
  uint8_t MAX_COUNT = (mClockFace.clock_maxCount &&  mClockFace.clock_maxCount != 0) ?  mClockFace.clock_maxCount : 12;
  uint8_t PRINT_N_OBJ = (mClockFace.clock_printNobject &&  mClockFace.clock_printNobject != 0) ?  mClockFace.clock_printNobject : 3;
  uint8_t PRINT_N_OBJ_START = (mClockFace.clock_printNobjectStart &&  mClockFace.clock_printNobjectStart != 0) ?  mClockFace.clock_printNobjectStart : 1;
  uint8_t MID_X = ((mClockFace.clock_midX &&  mClockFace.clock_midX != 0) ?  mClockFace.clock_midX : 128) - 64;
  uint8_t MID_Y = ((mClockFace.clock_midY &&  mClockFace.clock_midY != 0) ?  mClockFace.clock_midY : 128) - 64;
  uint8_t COLOR = (mClockFace.clock_color &&  mClockFace.clock_color != 0) ?  mClockFace.clock_color - 1 : -1; // "1=BLACK, 2=WHITE"
  if (COLOR < 0) {
    switch (COLOR_WATCH_BACKGROUND) {
      case COLOR_BLACK: COLOR = COLOR_WHITE;
        break;
      case COLOR_WHITE: COLOR = COLOR_BLACK;
        break;
    }
  }
  uint8_t TOKEN_DIR = (mClockFace.clock_tokenDirection &&  mClockFace.clock_tokenDirection != 0) ?  mClockFace.clock_tokenDirection : 1;
  uint8_t TOKEN_WIDTH = (mClockFace.clock_tokenWidth &&  mClockFace.clock_tokenWidth != 0) ?  mClockFace.clock_tokenWidth : 2; // TextSize for numbers
  uint8_t TOKEN_HEIGHT = (mClockFace.clock_tokenHeight &&  mClockFace.clock_tokenHeight != 0) ?  mClockFace.clock_tokenHeight : 2; // Unused for numbers...
  uint8_t TOKEN_DISP = (mClockFace.clock_tokenDisposal &&  mClockFace.clock_tokenDisposal != 0) ?  mClockFace.clock_tokenDisposal : 2;
  // uint8_t LAYER // UNUSED HERE...
  uint8_t MOVE = (mClockFace.clock_move &&  mClockFace.clock_move != 0) ?  mClockFace.clock_move : 0;
  uint8_t MOVE_DENOM = (mClockFace.clock_moveDenominator && mClockFace.clock_moveDenominator != 0) ? mClockFace.clock_moveDenominator : 17;
  uint8_t MOVE_NUM = (mClockFace.clock_moveNumerator && mClockFace.clock_moveNumerator != 0) ? mClockFace.clock_moveNumerator : 17;
  uint8_t MOVE_INTERVAL = (mClockFace.clock_moveInterval &&  mClockFace.clock_moveInterval != 0) ?  mClockFace.clock_moveInterval : 0;
  uint8_t MOVE_REL_X = (mClockFace.clock_moveRelToX &&  mClockFace.clock_moveRelToX != 0) ?  mClockFace.clock_moveRelToX : 0;
  uint8_t MOVE_REL_Y = (mClockFace.clock_moveRelToY &&  mClockFace.clock_moveRelToY != 0) ?  mClockFace.clock_moveRelToY : 0;
  uint8_t MOVE_RESET_DENOM = (mClockFace.clock_moveResetDenominator &&  mClockFace.clock_moveResetDenominator != 0) ?  mClockFace.clock_moveResetDenominator : 0;
  uint8_t MOVE_RESET_NUM = (mClockFace.clock_moveResetNumerator &&  mClockFace.clock_moveResetNumerator != 0) ?  mClockFace.clock_moveResetNumerator : 0;
  uint8_t MOVE_PACE = (mClockFace.clock_movePace &&  mClockFace.clock_movePace != 0) ?  mClockFace.clock_movePace : 0;
  uint8_t multiExponent = (mClockFace.clock_movePaceMultiplyerExponent &&  mClockFace.clock_movePaceMultiplyerExponent != 0) ?  mClockFace.clock_movePaceMultiplyerExponent : 0;
  int MOVE_PACE_MULTI = pow(10, multiExponent);

  display.fillRect(0, 0, display.width(), display.height(), COLOR_WATCH_BACKGROUND);

  // Make clock face fit in screen:
  RADIUS -= TOKEN_WIDTH * 5;
  MID_X -= TOKEN_WIDTH * 5;
  MID_Y -= TOKEN_WIDTH * 5;

  int countDirection = 1;
  if (TOKEN_DIR == 1 || TOKEN_DIR == 3) {
    // Count Right:
    countDirection = 1;
  } else if (TOKEN_DIR == 2 || TOKEN_DIR == 4) {
    // Count Left:
    countDirection = -1;
  }

  int tokenCounter = 0;
  switch (TYPE) {
    case 1:
      // Watch face with numbers:
      for (int token = PRINT_N_OBJ_START; token <= MAX_COUNT; token++) {
        tokenCounter++;
        if (tokenCounter == PRINT_N_OBJ) {
          tokenCounter = 0;

          int Nx = 0;
          int Ny = 0;
          switch (TOKEN_DISP) {
            case 1:
              // Rect disposition:

              break;
            case 2:
              // Circle disposition:
              float alpha = token * (360 / MAX_COUNT);
              Nx = (MID_X + ((sin((alpha * PI) / 180) * RADIUS)) * countDirection);
              Ny = MID_Y + -((cos((alpha * PI) / 180) * RADIUS));


              /*Serial.println("drawing clockface token ");
              Serial.println(token);
              Serial.println("with nx ");
              Serial.println(Nx);
              Serial.println("and ny ");
              Serial.println(Ny);
              Serial.println("with alpha");
              Serial.println(alpha);*/

              break;
          }
          Nx = (int)(((float)Nx / 128.0) * 96.0);
          Ny = (int)(((float)Ny / 128.0) * 96.0);
          display.setTextColor(COLOR);
          display.setTextSize(TOKEN_WIDTH);
          display.setCursor(Nx, Ny);
          display.print(token);
        }

      }

      break;
    case 2:
      // Watch face with objects
      // TODO
      break;
      // more Objects....
      // TODO


      // display.refresh();
  }

  if (MOVE != 0) {
    // move object according to current time:

    // TODO
  }

}

void drawTextFace(struct TEXT_FACE mTextFace) {

}

void drawWatchObject(struct WATCHFACE_OBJECT mWatchObject) {
  uint8_t SHAPE = (mWatchObject.object_shape && mWatchObject.object_shape != 0) ? mWatchObject.object_shape : 4;
  uint8_t START_X = ((mWatchObject.object_startX && mWatchObject.object_startX != 0) ? mWatchObject.object_startX : 128) - 64;
  uint8_t START_Y = ((mWatchObject.object_startY && mWatchObject.object_startY != 0) ? mWatchObject.object_startY : 140) - 64;
  uint8_t END_X = ((mWatchObject.object_endX && mWatchObject.object_endX != 0) ? mWatchObject.object_endX : 128) - 64;
  uint8_t END_Y = ((mWatchObject.object_endY && mWatchObject.object_endY != 0) ? mWatchObject.object_endY : 74) - 64;
  uint8_t RADIUS_DIAMETER = (mWatchObject.object_radius && mWatchObject.object_radius != 0) ? mWatchObject.object_radius : 4; // diameter for Rects, radius for triangles and circles
  uint8_t COLOR = (mWatchObject.object_color && mWatchObject.object_color != 0) ? mWatchObject.object_color : COLOR_BLACK;
  // uint8_t LAYER // UNUSED HERE...
  uint8_t MOVE = (mWatchObject.object_move && mWatchObject.object_move != 0) ? mWatchObject.object_move : 0;
  uint8_t MOVE_DENOM = (mWatchObject.object_moveDenominator && mWatchObject.object_moveDenominator != 0) ? mWatchObject.object_moveDenominator : 17;
  uint8_t MOVE_NUM = (mWatchObject.object_moveNumerator && mWatchObject.object_moveNumerator != 0) ? mWatchObject.object_moveNumerator : 17;
  uint8_t MOVE_INTERVAL = (mWatchObject.object_moveInterval && mWatchObject.object_moveInterval != 0) ? mWatchObject.object_moveInterval : 3;
  uint8_t MOVE_REL_X = ((mWatchObject.object_moveRelToX && mWatchObject.object_moveRelToX != 0) ? mWatchObject.object_moveRelToX : 128) - 64;
  uint8_t MOVE_REL_Y = ((mWatchObject.object_moveRelToY && mWatchObject.object_moveRelToY != 0) ? mWatchObject.object_moveRelToY : 128) - 64;
  uint8_t MOVE_RESET_DENOM = (mWatchObject.object_moveResetDenominator && mWatchObject.object_moveResetDenominator != 0) ? mWatchObject.object_moveResetDenominator : 17;
  uint8_t MOVE_RESET_NUM = (mWatchObject.object_moveResetNumerator && mWatchObject.object_moveResetNumerator != 0) ? mWatchObject.object_moveResetNumerator : 17;
  uint8_t MOVE_PACE = (mWatchObject.object_movePace && mWatchObject.object_movePace != 0) ? mWatchObject.object_movePace : 10;
  uint8_t multiExponent = (mWatchObject.object_movePaceMultiplyerExponent && mWatchObject.object_movePaceMultiplyerExponent != 0) ? mWatchObject.object_movePaceMultiplyerExponent : 1;
  int MOVE_PACE_MULTI = pow(10, multiExponent);
  MOVE_PACE = MOVE_PACE * MOVE_PACE_MULTI;

  if (MOVE_PACE < SMALLEST_MOVE_PACE && MOVE && MOVE != 0) {
    SMALLEST_MOVE_PACE = MOVE_PACE;
  }

  Serial.println("Drawing object");

  switch (SHAPE) {
    // (borders can be replaced with 2 overlapping identical shapes... evtl drop borders.)
    case 1:
      // Rect with borders
      break;
    case 2:
      // Circle with borders
      break;
    case 3:
      // Triangle with borders
      break;
    case 4: {
        // Rect

        /*Serial.println("Drawing object Shape 4. Start x :");
        Serial.println(START_X);
        Serial.println("start y");
        Serial.println(START_Y);
        Serial.println("end x");
        Serial.println(END_X);
        Serial.println("end y");
        Serial.println(END_Y);*/
        int alpha = 0;
        if (MOVE != 0) {
          calculateMovedPoints(&START_X, &START_Y, &END_X, &END_Y, MOVE_REL_X, MOVE_REL_Y, MOVE, MOVE_DENOM, MOVE_NUM, MOVE_INTERVAL, &alpha);
        }

        Serial.println("Start x after calculating: ");
        Serial.println(START_X);
        Serial.println("start y");
        Serial.println(START_Y);
        Serial.println("end x");
        Serial.println(END_X);
        Serial.println("end y");
        Serial.println(END_Y);

        START_X = (int)(((float)START_X / 128.0) * 96.0);
        START_Y = (int)(((float)START_Y / 128.0) * 96.0);
        END_X = (int)(((float)END_X / 128.0) * 96.0);
        END_Y = (int)(((float)END_Y / 128.0) * 96.0);
        display.drawLine(START_X, START_Y, END_X, END_Y, COLOR);

        // koordinaten neben start- und endpunkten verwenden und diese jeweils um die start / end punkte rotieren

        // <-> radius von 0 hochzhlen und um 90 grad r/l rotieren -> ergibt neuer punkt...

        for (int r = 0; r < RADIUS_DIAMETER / 2; r++) {
          /*int startX = START_X + (r) * cos( ((*alpha + 90) * PI) / 180 ) - (*START_Y - MOVE_REL_Y) *sin( ((moveDir* *alpha) * PI) / 180 );
          int startY = START_Y + (r) * sin( ((*alpha + 90) * PI) / 180 ) - (*START_Y - MOVE_REL_Y) *cos( ((moveDir* *alpha) * PI) / 180 );

          int endX = END_X + (*END_X - MOVE_REL_X) * cos( ((*alpha) * PI) / 180 ) - (*END_Y - MOVE_REL_Y) *sin( ((moveDir* *alpha) * PI) / 180 );
          int endY = END_Y + (*END_X - MOVE_REL_X) * sin( ((*alpha) * PI) / 180 ) - (*END_Y - MOVE_REL_Y) *cos( ((moveDir* *alpha) * PI) / 180 );*/

          int startX = START_X + (cos( ((alpha + 90) / 180) * PI) ) * r;
          int startY = START_Y - (sin( ((alpha + 90) / 180) * PI) ) * r;
          int endX = END_X + (cos( ((alpha + 90) / 180) * PI) ) * r;
          int endY = END_Y - (sin( ((alpha + 90) / 180) * PI) ) * r;

          display.drawLine(startX, startY, endX, endY, COLOR);
        }

        for (int r = 1; r <= RADIUS_DIAMETER / 2; r++) {

          int startX = START_X + (cos( ((alpha - 90) / 180) * PI) ) * r;
          int startY = START_Y - (sin( ((alpha - 90) / 180) * PI) ) * r;
          int endX = END_X + (cos( ((alpha - 90) / 180) * PI) ) * r;
          int endY = END_Y - (sin( ((alpha - 90) / 180) * PI) ) * r;

          display.drawLine(startX, startY, endX, endY, COLOR);
        }

        /*
        int dXnorm = END_X - START_X;
        dXnorm = abs(dXnorm);
        int dYnorm = END_Y - START_Y;
        dYnorm = abs(dYnorm);

        float m_norm;
        float m_helper;
        if(dXnorm != 0){
          m_norm = (float)dYnorm / (float)dXnorm;
          if(m_norm != 0){
            m_helper = -1/m_norm;
          } else {
            m_helper = 9999999;
          }
        } else {
          m_helper = 0;
        }

        for(int r = 0; r <= RADIUS_DIAMETER/2; r++){
          int xStart = START_X + r;
          int yStart = START_Y + (int) (-(float)r/m_helper);
          int xEnd = END_X + r;
          int yEnd = END_Y + (int) (-(float)r/m_helper);

          display.drawLine(xStart, yStart, xEnd, yEnd, COLOR);
        }

        for(int r = RADIUS_DIAMETER/2 - 1; r >= 0 ; r--){
          int xStart = START_X - r;
          int yStart = START_Y - (int) (-(float)r/m_helper);
          int xEnd = END_X - r;
          int yEnd = END_Y - (int) (-(float)r/m_helper);

          display.drawLine(xStart, yStart, xEnd, yEnd, COLOR);
        }*/

        break;
      }
    case 5:
      // Circle
      break;
    case 6:
      // Triangle
      break;
  }
}

void calculateMovedPoints(uint8_t *START_X, uint8_t *START_Y, uint8_t *END_X, uint8_t *END_Y, uint8_t MOVE_REL_X,
                          uint8_t MOVE_REL_Y, uint8_t MOVE, uint8_t MOVE_DENOM, uint8_t MOVE_NUM, uint8_t MOVE_INTERVAL, int *alpha) {

  int moveDir = 1;
  int TOTAL_NUMBER = 360; // 128 for display width
  switch (MOVE) {
    case 1:
      // Right
      moveDir = 1;
      TOTAL_NUMBER = display.width();
      break;
    case 2:
      // Left
      moveDir = -1;
      TOTAL_NUMBER = display.width();
      break;
  }
  if (MOVE == 1 || MOVE == 2) {
    // TODO move
  }

  switch (MOVE) {
    case 3:
      // Up
      moveDir = 1;
      TOTAL_NUMBER = display.height();
      break;
    case 4:
      // Down
      moveDir = -1;
      TOTAL_NUMBER = display.height();
      break;
  }
  if (MOVE == 3 || MOVE == 4) {
    // TODO move
  }

  switch (MOVE) {
    case 5:
      // Rotate Left
      moveDir = -1;
      TOTAL_NUMBER = 360;
      break;
    case 6:
      // Rotate Right
      moveDir = 1;
      TOTAL_NUMBER = 360;
      break;
  }
  if (MOVE == 5 || MOVE == 6) {
    // TODO reset after x turns..

    switch (MOVE_INTERVAL) {
      case 1: {
          // Per Day
          float hours_total = (float)(24 * (MOVE_DENOM / MOVE_NUM));//1440 * (MOVE_DENOM / MOVE_NUM));
          float hours_current = (float)((HOUR));// * 60 + MINUTE));

          *alpha = (int)((hours_current / hours_total) * (float) TOTAL_NUMBER);

          int deltaXs = MOVE_REL_X - *START_X;
          int deltaYs = MOVE_REL_Y - *START_Y;
          double deltaStart = sqrt(pow(deltaXs, 2) + pow(deltaYs, 2));

          int deltaXe = MOVE_REL_X - *END_X;
          int deltaYe = MOVE_REL_Y - *END_Y;
          double deltaEnd = sqrt(pow(deltaXe, 2) + pow(deltaYe, 2));

          int START_X_new = MOVE_REL_X + (*START_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*START_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int START_Y_new = MOVE_REL_Y + (*START_X - MOVE_REL_X) * -sin( ((moveDir * *alpha) * PI) / 180 ) + (*START_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          int END_X_new = MOVE_REL_X + (*END_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*END_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int END_Y_new = MOVE_REL_Y + (*END_X - MOVE_REL_X) * -sin( ((moveDir * *alpha) * PI) / 180 ) + (*END_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          *START_X = START_X_new;
          *START_Y = START_Y_new;
          *END_X = END_X_new;
          *END_Y = END_Y_new;

          break;
        }
      case 2: {
          // Per Hour

          float minutes_total = (float)(1440 * (MOVE_DENOM / MOVE_NUM));
          float minutes_current = (float)((HOUR * 60 + MINUTE));

          *alpha = (int)((minutes_current / minutes_total) * (float) TOTAL_NUMBER);

          int deltaXs = MOVE_REL_X - *START_X;
          int deltaYs = MOVE_REL_Y - *START_Y;
          double deltaStart = sqrt(pow(deltaXs, 2) + pow(deltaYs, 2));

          int deltaXe = MOVE_REL_X - *END_X;
          int deltaYe = MOVE_REL_Y - *END_Y;
          double deltaEnd = sqrt(pow(deltaXe, 2) + pow(deltaYe, 2));

          int START_X_new = MOVE_REL_X + (*START_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*START_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int START_Y_new = MOVE_REL_Y - (*START_X - MOVE_REL_X) * sin( ((moveDir * *alpha) * PI) / 180 ) + (*START_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          int END_X_new = MOVE_REL_X + (*END_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*END_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int END_Y_new = MOVE_REL_Y - (*END_X - MOVE_REL_X) * sin( ((moveDir * *alpha) * PI) / 180 ) + (*END_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          *START_X = START_X_new;
          *START_Y = START_Y_new;
          *END_X = END_X_new;
          *END_Y = END_Y_new;

          break;
        }
      case 3: {
          // Per Minute
          float seconds_ms = (float)(60000 * (MOVE_DENOM / MOVE_NUM));
          float currentSeconds_ms = (float)(SECOND * 1000.0 + MILLIS /* * 10.0*/);

          *alpha = (int)((currentSeconds_ms / seconds_ms) * 360.0);

          // double omega = (360.0*(double)movePartA) / ((double)movePartB*seconds); // = Degrees per Second
          int deltaXs = MOVE_REL_X - *START_X;
          //deltaXs = abs(deltaXs);
          int deltaYs = MOVE_REL_Y - *START_Y;
          //deltaYs = abs(deltaYs);
          double deltaStart = sqrt(pow(deltaXs, 2) + pow(deltaYs, 2));

          int deltaXe = MOVE_REL_X - *END_X;
          //deltaXe = abs(deltaXe);
          int deltaYe = MOVE_REL_Y - *END_Y;
          //deltaYe = abs(deltaYe);
          double deltaEnd = sqrt(pow(deltaXe, 2) + pow(deltaYe, 2));

          int START_X_new = MOVE_REL_X + (*START_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*START_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int START_Y_new = MOVE_REL_Y + (*START_X - MOVE_REL_X) * -sin( ((moveDir * *alpha) * PI) / 180 ) + (*START_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          int END_X_new = MOVE_REL_X + (*END_X - MOVE_REL_X) * cos( ((moveDir * *alpha) * PI) / 180 ) - (*END_Y - MOVE_REL_Y) * sin( ((moveDir * *alpha) * PI) / 180 );
          int END_Y_new = MOVE_REL_Y + (*END_X - MOVE_REL_X) * -sin( ((moveDir * *alpha) * PI) / 180 ) + (*END_Y - MOVE_REL_Y) * cos( ((moveDir * *alpha) * PI) / 180 );

          *START_X = START_X_new;
          *START_Y = START_Y_new;
          *END_X = END_X_new;
          *END_Y = END_Y_new;

          /*
          if(deltaXs < 0){
            *START_X = (MOVE_REL_X + ((sin(((*alpha) * PI) / 180) * -deltaStart)) * moveDir);
          } else {
            *START_X = (MOVE_REL_X + ((sin(((*alpha+180) * PI) / 180) * -deltaStart)) * moveDir);
          }

          if(deltaYs < 0){
            *START_Y = MOVE_REL_Y + -(cos(((*alpha) * PI) / 180) * -deltaStart);
          } else {
            *START_Y = MOVE_REL_Y + -(cos(((*alpha) * PI) / 180) * deltaStart);
          }

          if(deltaXe < 0){
            *END_X = (MOVE_REL_X + ((sin((*alpha * PI) / 180) * -deltaEnd)) * moveDir);
          } else {
            *END_X = (MOVE_REL_X + ((sin((*alpha * PI) / 180) * -deltaEnd)) * moveDir);
          }

          if(deltaYe < 0){
            *END_Y = MOVE_REL_Y + -(cos((*alpha * PI) / 180) * -deltaEnd);
          } else {
            *END_Y = MOVE_REL_Y + -(cos((*alpha * PI) / 180) * deltaEnd);
          }*/

          break;
        }
      case 4:
        // Per Second
        break;
    }
  }
}

void dismissCurrentListItem(boolean isScrollDown, boolean isFastScroll) {
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;

  if (isFastScroll) {
    if (app_list -> animateRemoveAsEntry) {
      // remove current item with slide to right
      // todo: check if app starts with this animation and if so dont do it
      for (int t = 1; t <= display.height(); t += t * 4 / 3 + 1) {
        int tt = t;
        drawListBackground(app_list, false, 0, 0);
        drawListItem(app_list, tt, 0);
        refreshDisplay();
      }
    } else if (app_list -> animateScrollAsEntry) {
      // show next Item by vertical scroll:
      for (int t = 1; t <= display.height(); t += t * 4 / 3 + 1) {
        int tt = t;
        if (isScrollDown) {
          tt = -t;
        }
        drawListBackground(app_list, false, 0, 0);
        drawListItem(app_list, 0, tt);
        refreshDisplay();
      }
    } else {
      // show next Item without animation:
      drawListBackground(app_list, false, 0, 0);
      drawListItem(app_list, 0, -DISPLAY_HEIGHT);
      refreshDisplay();
    }
    // Reset old data

    //memset(&app_list -> list_itemContent[app_list -> activeItem].list_itemContents[0], 0, sizeof(app_list -> list_itemContent[app_list -> activeItem].list_itemContents));

  } else {
    // show previous content by vertical scroll
  }
}

void drawListItem(struct LIST_APP* app_list, int xOffset, int yOffset) {

  // todo draw item background in its own method
  switch (app_list -> list_itemTheme.item_theme_ID) {
    case 0:
      // draw item specific layout backgrounds like cards (?) etc...
      break;
      // -- more item themes --
  }
  
  yOffset += app_list -> contentOffsetY;

  // draw title:
  display.setTextColor(app_list -> color_title);
  display.setTextWrap(false);
  display.setTextSize(app_list -> list_fontTheme.titleSize);
  display.setCursor(xOffset + app_list -> list_fontTheme.titleStartX, yOffset + app_list -> list_fontTheme.titleStartY);
  display.println(app_list -> list_itemContent[app_list -> activeItem].list_itemTitle);
  // draw content:
  display.setTextColor(app_list -> color_content);
  display.setTextSize(app_list -> list_fontTheme.contentSize);
  display.setTextWrap(true);
  display.setCursor(xOffset + app_list -> list_fontTheme.contentStartX, yOffset + app_list -> list_fontTheme.contentStartY);
  display.println(app_list -> list_itemContent[app_list -> activeItem].list_itemContents);
  // Serial.println(app_list -> list_itemContent[app_list -> activeItem].list_itemContents);
  // draw button icons:
  if (app_list -> isFirstItem) {
    drawButtonIcon(app_list -> icon_b_top_endstate, app_list -> b_top_x, app_list -> b_top_y, app_list -> color_b_top, app_list -> style_b_top_endstate, app_list -> back_b_top);
  } else {
    drawButtonIcon(app_list -> icon_b_top, app_list -> b_top_x, app_list -> b_top_y, app_list -> color_b_top, app_list -> style_b_top_inscrollmode, app_list -> back_b_top);
  }
  if (app_list -> isLastItem) {
    drawButtonIcon(app_list -> icon_b_bottom_endstate, app_list -> b_bottom_x, app_list -> b_bottom_y, app_list -> color_b_bottom, app_list -> style_b_bottom_endstate, app_list -> back_b_bottom);
  } else {
    drawButtonIcon(app_list -> icon_b_bottom, app_list -> b_bottom_x, app_list -> b_bottom_y, app_list -> color_b_bottom, app_list -> style_b_bottom_inscrollmode, app_list -> back_b_bottom);
  }
  drawButtonIcon(app_list -> icon_b_right, app_list -> b_right_x, app_list -> b_right_y, app_list -> color_b_right, app_list -> style_b_right, app_list -> back_b_right);
  drawButtonIcon(app_list -> icon_b_left, app_list -> b_left_x, app_list -> b_left_y, app_list -> color_b_left, app_list -> style_b_left, app_list -> back_b_left);
  
}

void drawButtonIcon(int icon, int x, int y, int color, int style, int back) {
  // draw style with background color:
  switch (style) {
    case 0:
      // no icon
      break;
    case 1:
      //
      break;
    case 2:
      // transparent style
      break;
    case 3:
      // round style
      break;
      // -- more styles (up to 253 possible!...)
  }

  // draw icon:
  drawBIcon(icon, color, x, y);
}

void drawBIcon(int icon, int color, int x, int y) {
  switch (icon) {
    case 0:
      // no icon
      break;
    case 1:
      //
      break;
    case 2:
      // standard scroll down icon
      display.drawLine(x, y + BUTTON_HEIGHT / 2, x + BUTTON_WIDTH / 2, y, color); //(display.width() / 2 - 8, 10, display.width() / 2, 2, color);
      display.drawLine(x + BUTTON_WIDTH / 2, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT / 2, color); //(display.width() / 2, 2, display.width() / 2 + 8, 10, color);
      break;
    case 3:
      // standard scroll up icon
      display.drawLine(x, y + BUTTON_HEIGHT / 2, x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT, color); //(display.width() / 2 - 8, display.height() - 10, display.width() / 2, display.height() - 2, color);
      display.drawLine(x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT, x + BUTTON_WIDTH, y + BUTTON_HEIGHT / 2, color); //(display.width() / 2, display.height() - 2, display.width() / 2 + 8, display.height() - 10, color);
      break;
      // -- more icons (up to 253 possible!...)
    case 4:
      // Notifications Icon
      display.fillRect(x+1, y, NOTIF_ICON_WIDTH-1, NOTIF_ICON_HEIGHT-3, color);
      display.drawLine(x, y+1, x, y+NOTIF_ICON_HEIGHT-5, color);
      display.drawLine(x+NOTIF_ICON_WIDTH, y+1, x+NOTIF_ICON_WIDTH, y+NOTIF_ICON_HEIGHT-5, color);
      display.drawPixel(x+NOTIF_ICON_WIDTH/2, y+NOTIF_ICON_HEIGHT-3, color);
      display.drawPixel(x+NOTIF_ICON_WIDTH/2, y+NOTIF_ICON_HEIGHT-2, color);
      display.drawPixel(x+NOTIF_ICON_WIDTH/2+1, y+NOTIF_ICON_HEIGHT-3, color);
      break;
    case 5:
      // standard scroll left icon
      display.drawLine(x+BUTTON_WIDTH, y, x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT/2, color);
      display.drawLine(x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT/2, x + BUTTON_WIDTH, y + BUTTON_HEIGHT, color);
      break;
    case 6:
      // standard scroll right icon
      display.drawLine(x, y, x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT/2, color);
      display.drawLine(x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT/2, x, y + BUTTON_HEIGHT, color);
      break;
    case 7:
      // Play icon
      display.fillTriangle(x, y,
                     x, y+BUTTON_HEIGHT,
                     x+BUTTON_WIDTH * 2/3, y+BUTTON_HEIGHT / 2, color);
      break;
    case 8:
      // Pause icon
      display.fillRect(x,y,BUTTON_WIDTH / 2 - 2, BUTTON_HEIGHT, color);
      display.fillRect(x + BUTTON_WIDTH / 2 + 2,y,BUTTON_WIDTH / 2 - 2, BUTTON_HEIGHT, color);
      break;
    case 9:
      // Stop icon
      display.fillRect(x,y,BUTTON_WIDTH, BUTTON_HEIGHT, color);
      break;
    case 10:
      // Next Song icon
      display.fillTriangle(x, y,
                     x, y+BUTTON_HEIGHT,
                     x+BUTTON_WIDTH * 2.0/3.0, y+BUTTON_HEIGHT / 2, color);
      display.fillTriangle(x+BUTTON_WIDTH * 1.0/3.0, y,
                     x+BUTTON_WIDTH * 1.0/3.0, y+BUTTON_HEIGHT,
                     x+BUTTON_WIDTH, y+BUTTON_HEIGHT / 2, color);
      break;
    case 11:
      // Previous Song icon
      display.fillTriangle(x+BUTTON_WIDTH, y,
                     x+BUTTON_WIDTH, y+BUTTON_HEIGHT,
                     x+BUTTON_WIDTH * 1.0/3.0, y+BUTTON_HEIGHT / 2, color);
      display.fillTriangle(x+BUTTON_WIDTH * 2.0/3.0, y,
                     x+BUTTON_WIDTH * 2.0/3.0, y+BUTTON_HEIGHT,
                     x, y+BUTTON_HEIGHT / 2, color);
      break;
    case 12:
      // Rewind left
      display.drawCircle(x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT / 2, BUTTON_WIDTH / 2 - 2, color);
      display.fillTriangle(x, y+BUTTON_HEIGHT / 2, x+5, y+BUTTON_HEIGHT / 2, x+3, y+BUTTON_HEIGHT / 2 + 3, color);
      break;
  }
}

void showListItem(struct LIST_APP* app_list, boolean isFastScroll, boolean isScrollDown) {
  if (isFastScroll) {
    // when button is pressed global variable indicating which transition should be done may be set.
    // check this variable and do the proper transition before showing the content.

    // reset previously slowly scrolled state:
    app_list -> contentOffsetY = 0;
    
    if (DO_SCREEN_TRANSITION_UP) {
      for (int t = display.height(); t >= 0; t -= getScreenTransitionOffset(t) ) {
        drawListBackground(app_list, false, 0, t);
        refreshDisplay();
      }
      for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
        drawListBackground(app_list, false, 0, 0);
        drawListItem(app_list, -t, 0);
        refreshDisplay();
      }

      DO_SCREEN_TRANSITION_UP = false;
      DO_SCREEN_TRANSITION_DOWN = false;
    } else if (DO_SCREEN_TRANSITION_DOWN) {
      for (int t = display.height(); t >= 0; t -= getScreenTransitionOffset(t) ) {
        drawListBackground(app_list, false, 0, -t);
        refreshDisplay();
      }
      for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
        drawListBackground(app_list, false, 0, 0);
        drawListItem(app_list, -t, 0);
        refreshDisplay();
      }

      DO_SCREEN_TRANSITION_UP = false;
      DO_SCREEN_TRANSITION_DOWN = false;
    } else if (app_list -> animateScrollAsEntry) {
      // show next Item by vertical scroll:
      for (int t = display.height(); t >= 0; t -= getScreenTransitionOffset(t) ) {
        int tt = -t;
        if (isScrollDown) {
          tt = t;
        }
        drawListBackground(app_list, false, 0, 0);
        drawListItem(app_list, 0, tt);
        refreshDisplay();
      }

    } else {
      // show next item without animation:
      drawListBackground(app_list, false, 0, 0);
      drawListItem(app_list, 0, 0);
      refreshDisplay();
    }
  } else {
    // show next content by vertical scroll
    int offset = app_list -> list_fontTheme.contentSize * 10; // todo: multiply with lines; correct contentSize to pixelHeight transformation (probably it won't be 10)
    for (int t = 0; t <= offset; t += 3) {
      int tt = t;
      if (isScrollDown) {
        tt = -t;
      }
      drawListBackground(app_list, false, 0, 0);
      drawListItem(app_list, 0, tt);
      refreshDisplay();
    }
    if (isScrollDown) {
      app_list -> contentOffsetY -= offset;
    } else {
      app_list -> contentOffsetY += offset;
    }
  }
  
}

void drawListTitles(struct LIST_APP* app_list){
  display.setTextColor(app_list -> color_listTitles);
  display.setTextWrap(false);
  display.setTextSize(app_list -> list_fontTheme.titleTopSize);
  display.setCursor(app_list -> list_fontTheme.titleTopStartX, app_list -> list_fontTheme.titleTopStartY);
  display.println(app_list -> title_top);
  
  display.setTextColor(app_list -> color_listTitles);
  display.setTextWrap(false);
  display.setTextSize(app_list -> list_fontTheme.titleBottomSize);
  display.setCursor(app_list -> list_fontTheme.titleBottomStartX, app_list -> list_fontTheme.titleBottomStartY);
  display.println(app_list -> title_bottom);
}

void drawListBackground(struct LIST_APP* app_list, boolean doCoverTransition, int xOffset, int yOffset) {
  drawAppBackground(app_list -> list_backTheme.back_theme_ID, app_list -> color_background, xOffset, yOffset);
}

void drawActionBackground(struct ACTION_APP* app_action, boolean doCoverTransition, int xOffset, int yOffset) {
  drawAppBackground(app_action -> action_backTheme.back_theme_ID, app_action -> color_background, xOffset, yOffset);
}

void drawAppBackground(int backID, int colorBack, int xOffset, int yOffset){
  switch (backID) {
    case 0:
      display.fillRect(xOffset + 0, yOffset + 0, xOffset + display.width(), yOffset + display.height(),colorBack);
      break;
      // -- more background themes --
  }
}

void showActionScreen(struct ACTION_APP* app_action) {
  // when button is pressed global variable indicating which transition should be done may be set.
  // check this variable and do the proper transition before showing the content.

  if (DO_SCREEN_TRANSITION_UP) {
    // animateActionTransition(app_action); // <- todo....

    for (int t = display.height(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawActionBackground(app_action, false, 0, t);
      refreshDisplay();
    }
    
    for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawActionBackground(app_action, false, 0, 0);
      drawActionScreen(app_action, -t, 0);
      refreshDisplay();
    }

    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
  } else if (DO_SCREEN_TRANSITION_DOWN) {
    for (int t = display.height(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawActionBackground(app_action, false, 0, -t);
      refreshDisplay();
    }
    for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawActionBackground(app_action, false, 0, 0);
      drawActionScreen(app_action, -t, 0);
      refreshDisplay();
    }

    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
  } else {
    drawActionBackground(app_action, false, 0, 0);
    drawActionScreen(app_action, 0, 0);
    refreshDisplay();
  }
}

int getScreenTransitionOffset(int t) {
  return t * 4 / 7 + 1;
}

void animateActionTransition(struct ACTION_APP* app_action) {
  // todo make this work.. currently not functional and incomplete
  int oldApp = ACTIVE_APPSCREEN + 1;
  if (oldApp > 1) {
    oldApp = 0;
  }
  if (&mAppScreen[oldApp]) {
    int oldScreenMode = mAppScreen[oldApp].APP_SCREEN_MODE;
    switch (oldScreenMode) {
      case BYTE_ID_WATCHFACE:
        break;
      case BYTE_ID_LISTLAYOUT:
        break;
      case BYTE_ID_ACTIONLAYOUT:
        for (int t = 1; t <= display.height(); t += t * 4 / 3 + 1) {
          int tt = -t;
          if (DO_SCREEN_TRANSITION_UP) {
            tt = t;
          }

          drawActionBackground(&mAppScreen[oldApp].app_action, false, 0, tt);
          drawActionScreen(&mAppScreen[oldApp].app_action, tt, 0);


          drawActionBackground(app_action, false, 0, DISPLAY_HEIGHT + tt);

          refreshDisplay();
        }

        // Animate content from left (only when there is no cover?)
        for (int t = display.width(); t >= 0; t -= t * 3 / 4 + 1) {
          int tt = -t;
          drawActionBackground(app_action, false, 0, 0);
          drawActionScreen(app_action, tt, 0);
          refreshDisplay();
        }
        break;
        // -- add missing IDs..  --
    }
  } else {
    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
    showActionScreen(app_action);
  }
}

void drawActionScreen(struct ACTION_APP* app_action, int xOffset, int yOffset) {

  drawMainIcon(app_action -> action_iconTheme);

  // draw title:
  display.setTextColor(app_action -> color_title);
  display.setTextWrap(false);
  display.setTextSize(app_action -> action_fontTheme.titleSize);
  display.setCursor(xOffset + app_action -> action_fontTheme.titleStartX, yOffset + app_action -> action_fontTheme.titleStartY);
  display.println(app_action -> action_content.title);
  Serial.println("        action screen title:");
  Serial.println(app_action -> action_content.title);
  // draw content:
  display.setTextColor(app_action -> color_content);
  display.setTextSize(app_action -> action_fontTheme.contentSize);
  display.setTextWrap(true);
  display.setCursor(xOffset + app_action -> action_fontTheme.contentStartX, yOffset + app_action -> action_fontTheme.contentStartY);
  display.println(app_action -> action_content.subtitle);
  // draw button icons:
  drawButtonIcon(app_action -> icon_b_top, app_action -> b_top_x, app_action -> b_top_y, app_action -> color_b_top, app_action -> style_b_top, app_action -> back_b_top);
  drawButtonIcon(app_action -> icon_b_right, app_action -> b_right_x, app_action -> b_right_y, app_action -> color_b_right, app_action -> style_b_right, app_action -> back_b_right);
  drawButtonIcon(app_action -> icon_b_bottom, app_action -> b_bottom_x, app_action -> b_bottom_y, app_action -> color_b_bottom, app_action -> style_b_bottom, app_action -> back_b_bottom);
  drawButtonIcon(app_action -> icon_b_left, app_action -> b_left_x, app_action -> b_left_y, app_action -> color_b_left, app_action -> style_b_left, app_action -> back_b_left);

}

void drawMainIcon(struct ICON_THEME iconTheme) {
  switch (iconTheme.icon_theme_ID) {
    case 0:
      // transparent background, round
      drawMIcon(iconTheme.mainIcon, iconTheme.mainIconColor);
      break;
    case 1:
      // round background
      break;
      // -- more icon themes --
  }
}

void drawMIcon(int icon, int color) {
  // todo switch icon and draw it.
}

void showCoverLayout(struct COVER_SCREEN *coverScreen) {
  if (DO_SCREEN_TRANSITION_UP) {
    // animateActionTransition(app_action); // <- todo....
    for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawCoverScreen(coverScreen, -t, 0);
      refreshDisplay();
    }

    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
  } else if (DO_SCREEN_TRANSITION_DOWN) {
    for (int t = display.width(); t >= 0; t -= getScreenTransitionOffset(t) ) {
      drawCoverScreen(coverScreen, -t, 0);
      refreshDisplay();
    }

    DO_SCREEN_TRANSITION_UP = false;
    DO_SCREEN_TRANSITION_DOWN = false;
  } else {
    drawCoverScreen(coverScreen, 0, 0);
    refreshDisplay();
  }
}
void showPopup(struct POPUP_SCREEN *popupScreen, boolean* IS_SHOWING_POPUP) {
  if (!*IS_SHOWING_POPUP) {
    *IS_SHOWING_POPUP = true;
    for (int t = 20; t <= (float)display.height() * 2.0 / 3.0; t += getPopupTransitionOffset(t) ) {
      drawPopup(popupScreen, 0, 0, t);
      refreshDisplay();
    }
  } else {
    drawPopup(popupScreen, 0, 0, display.height() * 2.0 / 3.0);
  }
}

void drawPopup(struct POPUP_SCREEN *popupScreen, int xOffset, int yOffset, int heightOffset) {
  int yStart = display.height() / 2 - heightOffset / 2;
  int xStart = 0;
  int yEnd = yStart + heightOffset;
  int xEnd = display.width();

  display.fillRect(xStart, yStart, xEnd, yEnd, popupScreen -> COLOR_BACK);
  display.drawLine(xStart, yStart, xEnd, yStart, COLOR_BLACK);
  display.drawLine(xStart, yStart + 1, xEnd, yStart + 1, COLOR_WHITE);
  display.drawLine(xStart, yStart + 2, xEnd, yStart + 2, COLOR_BLACK);
  display.drawLine(xStart, yEnd, xEnd, yEnd, COLOR_BLACK);
  display.drawLine(xStart, yEnd - 1, xEnd, yEnd - 1, COLOR_WHITE);
  display.drawLine(xStart, yEnd - 2, xEnd, yEnd - 2, COLOR_BLACK);

  display.setTextColor(popupScreen -> COLOR_TEXT);
  display.setTextSize(2);
  display.setCursor(xStart + 10, yStart + 10);
  display.println(popupScreen -> title);

  display.setTextColor(popupScreen -> COLOR_TEXT);
  display.setTextSize(1);
  display.setCursor(xStart + 10, yEnd - 15);
  display.println(popupScreen -> content);

  drawBIcon(popupScreen -> icon_b_left, popupScreen -> COLOR_ICON, xStart + 2, display.height() / 2 - popupScreen -> iconHeight);
  drawBIcon(popupScreen -> icon_b_right, popupScreen -> COLOR_ICON, xEnd - 2 - popupScreen -> iconWidth, display.height() / 2 - popupScreen -> iconHeight);
}

int getPopupTransitionOffset(int t) {
  return 20;
}

void drawCoverScreen(struct COVER_SCREEN *coverScreen, int xOffset, int yOffset) {
  display.fillRect(xOffset + 0, yOffset + 0, xOffset + display.width(), yOffset + display.height(), coverScreen -> COLOR_BACKGROUND);
  display.setTextColor(coverScreen -> COLOR_TEXT);
  display.setTextWrap(false);
  display.setTextSize(coverScreen -> textSize);
  display.setCursor(xOffset + coverScreen -> textX, yOffset + coverScreen -> textY);
  display.println(coverScreen -> app_title);
}

void showNotificationInfo() {
  drawBIcon(4, COLOR_BLACK, 5, 5);
}

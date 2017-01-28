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
#include "I2Cdev.h"
#include "Wire.h"
#include "RTClib.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SPI.h>
#include <EEPROM.h>

#include <boards.h>
#include "services.h"
#include <lib_aci.h>
#include <aci_setup.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

// MPU:
bool dmpReady = false;  // set true if DMP init was successful

//-----------------------------------------------------------------------------------------
// BLE:
boolean IS_SNACH_CONNECTED = false;
int NATIVE_SCREEN_INDEX = 0;
#define MAX_NATIVE_SCREEN_INDEX 1
#define NA_ALARM_INDEX 1

// G_Data:
float MPU_XG;
float MPU_YG;
float MPU_ZG;
VectorInt16 MPU_ACCEL;

/*            DATA PROCESSING          */
// Global System-Data:
uint8_t SNACH_SCREEN = 0;
uint8_t SNACH_SCROLL_STATE = 0;
uint8_t SNACH_CURRENT_WATCHFACE_TYPE = 1;
uint8_t SNACH_SCREEN_MODE = 0;

// Data-Stram and buffer id-bytes:
#define ITEM_SEPARATOR_RECEIVED_DATA 0
#define EOF_RECEIVED_DATA 254

/*          CONTENT                        */
// Data for Wathfaces:
uint8_t HOUR;
uint8_t MINUTE;
uint8_t SECOND;
uint8_t MILLIS; // Multiply with 10
String DAY;
String MONTH;
String YEAR;

/*          LAYOUT AND APPEARANCE          */
// Transitions:
boolean DO_SCREEN_TRANSITION_UP = false;
boolean DO_SCREEN_TRANSITION_DOWN = false;
boolean IS_APP_ACTIVE = false; // used to determine when to do list scroll animations for dismiss etc (true after the app has set up its screen the first time)
// Display size:
#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 96
// Button size:
#define BUTTON_WIDTH 14
#define BUTTON_HEIGHT 14
#define NOTIF_ICON_HEIGHT 9
#define NOTIF_ICON_WIDTH 9

// Layouts for ListApps:
#define AMOUNT_LISTITEMS 2

// Colors
#define COLOR_BLACK 0
#define COLOR_WHITE 1

/*            Layout structs          */
// General
typedef struct BACK_THEME
{
  int back_theme_ID = 0;
  /*
  switch case in draw method will draw the proper background for the saved ID
  */
} BACK_THEME;
typedef struct ICON_THEME
{
  int icon_theme_ID = 0;
  int xStart = 0;
  int yStart = 0;
  const int iconWidth = 30;
  const int iconHeight = 30;
  // caution: Icons will have same, fixed size.
  
  int mainIcon = 0;
  int mainIconColor = 0;
  int mainIconBackground = 0;
  
  /*
  switch case in draw method will draw the proper icon for the saved ID
  */
} ICON_THEME;
typedef struct FONT_THEME
{
  int titleStartX = 0;
  int titleStartY = 0;
  int titleSize = 2;
  
  int contentStartX = 0;
  int contentStartY = 0;
  int contentSize = 1;
  
  int titleTopStartX = 0;
  int titleTopStartY = 0;
  int titleTopSize = 1;
  
  int titleBottomStartX = 0;
  int titleBottomStartY = 0;
  int titleBottomSize = 1;
} FONT_THEME;

// ListLayout structs
typedef struct LIST_ITEM_THEME
{
  int item_theme_ID = 0;
  int linesAmountContent = 0;
  boolean isSubtitle = false;
  /*
  switch case in draw method will draw the proper background for the saved ID
  
  also add options to use the left and right button as a item-button with icon
  */
} LIST_ITEM_THEME;
typedef struct LISTITEM_CONTENT
{
  char list_itemTitle[40];// One line, ~17 tokens
  char list_itemContents[80];// Max 2 lines for subtitles and bold text, for contents are X lines allowed...
  
  int contentBufferCursor = 0; // first free byte behind existing data or 0
  
  // content height in px??
} LISTITEM_CONTENT;
typedef struct LIST_APP 
{
  // Themes
  const int xStartList = 0;
  const int yStartList = 0;
  const int widthList = 128;
  const int heightList = 128;
  BACK_THEME list_backTheme;
  FONT_THEME list_fontTheme;
  LIST_ITEM_THEME list_itemTheme;
  int contentOffsetY = 0; // used for slowly scrolling the content
  
  // Appearance
  int color_background = 0;
  int color_main = 0;
  int color_highlight = 0;
  int color_title = 1;
  int color_content = 1;
    
  boolean showIcon_scrollUp = true;
  boolean showIcon_scrollDown = true;
  
  // Set to true when last or first items are delivered.
  // They will be identified by a special content separator byte
  boolean isLastItem = false;
  boolean isFirstItem = false;
  
  // Animation to apply when the layout is rendered.
  // Remove animation will slide the last item aout of the screen,
  // thus it should not be set when the app pushes the layout first.
  // Snach OS will notice due to SNACH_SCREEN change if this is the case
  // and set the remove animation to false.
  boolean animateRemoveAsEntry = false;
  boolean animateScrollAsEntry = false;
  
  // Buttons
  /*
    Button top icons when in scroll mode:
  */
  int icon_b_top = 0;
  int icon_b_bottom = 0;
  /*
    Button icons left and right for each item:
  */
  int icon_b_right = 0;
  int icon_b_left = 0;
  /*
    Button styles left and right for each listitem:
  */
  int style_b_left = 0;
  int style_b_right = 0;
  /*
    Button icons top and bottom when list is in start/end position:
  */
  int icon_b_bottom_endstate = 0;
  int icon_b_top_endstate = 0;
  /*
    Button styles top and bottom when list is in start/end position
    (Use default styles when in scroll mode):
  */
  const int style_b_top_inscrollmode = 0;
  const int style_b_bottom_inscrollmode = 0;
  int style_b_top_endstate = 0;
  int style_b_bottom_endstate = 0;
  /*
    Colors for each button position
    (Top and bottom button colors are same for in scroll mode and in endpositions):
  */
  int color_b_top = 0;
  int color_b_right = 0;
  int color_b_bottom = 0;
  int color_b_left = 0;
  int back_b_top = 0;
  int back_b_right = 0;
  int back_b_bottom = 0;
  int back_b_left = 0;
  const int b_top_x = DISPLAY_WIDTH / 2 - BUTTON_WIDTH / 2;
  const int b_top_y = BUTTON_HEIGHT / 2;
  const int b_right_x = DISPLAY_WIDTH - BUTTON_WIDTH * 1.5;
  const int b_right_y = DISPLAY_WIDTH / 2 - BUTTON_HEIGHT / 2;
  const int b_bottom_x = DISPLAY_WIDTH / 2 - BUTTON_WIDTH / 2;
  const int b_bottom_y = DISPLAY_HEIGHT - BUTTON_HEIGHT * 1.5;
  const int b_left_x = BUTTON_WIDTH / 2;
  const int b_left_y = DISPLAY_WIDTH / 2 - BUTTON_HEIGHT / 2;
  
  // Content
  LISTITEM_CONTENT list_itemContent[AMOUNT_LISTITEMS];
  int activeItem = 1;
  int color_listTitles = 0;//todo: set black; if background black then set white
  char title_top[20];
  char title_bottom[20];
  
} LIST_APP;

// ActionLayout structs
typedef struct ACTION_CONTENT
{
  char title [20];
  char subtitle [80];
} ACTION_CONTENT;
typedef struct ACTION_APP
{
  // Themes
  const int xStartAction = 0;
  const int yStartAction = 0;
  const int widthAction = 128;
  const int heightAction = 128;
  BACK_THEME action_backTheme;
  ICON_THEME action_iconTheme;
  FONT_THEME action_fontTheme;
  
  // Appearance
  int color_background = 0;
  int color_main = 0;
  int color_highlight = 0;
  int color_title = 1;
  int color_content = 1;
  
  // todo Buttons in eigenes struct!
  // Buttons
  int icon_b_top = 0;
  int icon_b_right = 0;
  int icon_b_bottom = 0;
  int icon_b_left = 0;
  int style_b_top = 0;
  int style_b_right = 0;
  int style_b_bottom = 0;
  int style_b_left = 0;
  int color_b_top = 0;
  int color_b_right = 0;
  int color_b_bottom = 0;
  int color_b_left = 0;
  int back_b_top = 0;
  int back_b_right = 0;
  int back_b_bottom = 0;
  int back_b_left = 0;
  const int b_top_x = DISPLAY_WIDTH / 2 - BUTTON_WIDTH / 2;
  const int b_top_y = BUTTON_HEIGHT / 2;
  const int b_right_x = DISPLAY_WIDTH - BUTTON_WIDTH * 1.5;
  const int b_right_y = DISPLAY_WIDTH / 2 - BUTTON_HEIGHT / 2;
  const int b_bottom_x = DISPLAY_WIDTH / 2 - BUTTON_WIDTH / 2;
  const int b_bottom_y = DISPLAY_HEIGHT - BUTTON_HEIGHT * 1.5;
  const int b_left_x = BUTTON_WIDTH / 2;
  const int b_left_y = DISPLAY_WIDTH / 2 - BUTTON_HEIGHT / 2;
  
  // Content
  ACTION_CONTENT action_content;
} ACTION_APP;

// App structs:
typedef struct APP_SCREEN
{
  LIST_APP app_list;
  ACTION_APP app_action;
  
  int APP_SCREEN_MODE = 0; 
  // Used for transitions. will equal BYTE_ID_XXX and helps to select either app_list, 
  // app_action or another mode for transition animation
} APP_SCREEN;

typedef struct POPUP_SCREEN
{
  char title[20];
  char content[20];
  int popup_theme_ID = 0;
  int icon_b_left = 0;
  int icon_b_right = 0;
  int popup_level = 0; // highest level = 0; will be displayed on the very top
  
  // set by switch-case of theme_ID:
  int COLOR_BACK = 0;
  int COLOR_TEXT = 0;
  int COLOR_ICON = 0;
  int style_b_left;
  int style_b_right;
  
  const int iconHeight = 15;
  const int iconWidth = 15;
}POPUP_SCREEN;

typedef struct COVER_SCREEN 
{
  char app_title[20];
  int COLOR_BACKGROUND = COLOR_BLACK;
  int COLOR_TEXT = COLOR_WHITE;
  int cover_theme_ID = 0;
  
  int textX = 10;
  int textY = 80;
  int textSize = 1;
  
} COVER_SCREEN;


// Watchface data:
#define AMOUNT_WATCHFACE_OBJECTS 8
#define AMOUNT_WATCHFACE_CLOCKFACES 3
#define AMOUNT_WATCHFACE_TEXTS 15

#define TIME_LOOP_DELAY 100
#define DEFAULT_MOVE_PACE_LARGE 10000000000000
int SMALLEST_MOVE_PACE = DEFAULT_MOVE_PACE_LARGE;
int TIME_COUNTER = 0;
uint8_t COLOR_WATCH_BACKGROUND = 1;
// boolean hasDrawnWatchBack = false;

// Watchface structs:
typedef struct CLOCK_FACE 
{
  /* uint8_t active; 
  // -> determines whether this clock face was specified. 1 if som 0 if not.
  // necessary as there is created an array of clock faces which should contain
  // all specified clock faces. So, 3 dummy clock faces are provided as global 
  // variales and are added to the array if they are specified. Thus, the array 
  // size can always match the amount of clock faces  */
  uint8_t clock_radius;
  uint8_t clock_type;
  uint8_t clock_maxCount;
  uint8_t clock_printNobject;
  uint8_t clock_printNobjectStart;
  uint8_t clock_midX;
  uint8_t clock_midY;
  uint8_t clock_color;
  uint8_t clock_tokenDirection;
  uint8_t clock_tokenWidth;
  uint8_t clock_tokenHeight;
  uint8_t clock_tokenDisposal;
  uint8_t clock_layer;
  uint8_t clock_move;
  uint8_t clock_moveDenominator;
  uint8_t clock_moveNumerator;
  uint8_t clock_moveInterval;
  uint8_t clock_moveRelToX;
  uint8_t clock_moveRelToY;
  uint8_t clock_moveResetDenominator;
  uint8_t clock_moveResetNumerator;
  uint8_t clock_movePace; // animate each data*10^multiplyerExponent ms, default is animate per second.
  uint8_t clock_movePaceMultiplyerExponent;
} CLOCK_FACE;
typedef struct TEXT_FACE
{
  // Text:
  uint8_t text_mark;
  uint8_t text_size;
  uint8_t text_format;
  uint8_t text_color;
  uint8_t text_layer;
} TEXT_FACE;
typedef struct WATCHFACE_OBJECT
{
  // Object:
  uint8_t object_shape;
  uint8_t object_startX;
  uint8_t object_startY;
  uint8_t object_endX;
  uint8_t object_endY;
  uint8_t object_radius;
  // TODO: border-width
  uint8_t object_color;
  uint8_t object_layer;
  uint8_t object_move;
  uint8_t object_moveDenominator;
  uint8_t object_moveNumerator; // X/Y turns per Z 4 bits for denominator, 4 bits for numerator
  uint8_t object_moveInterval;//             L> Z; 
  uint8_t object_moveRelToX;
  uint8_t object_moveRelToY;
  uint8_t object_moveResetDenominator; // reset afet X/Y turns. 4 bits for denominator, 4 bits for numerator
  uint8_t object_moveResetNumerator;
  uint8_t object_movePace; // animate each data*10^multiplyerExponent ms, default is animate per second.
  uint8_t object_movePaceMultiplyerExponent;
} WATCHFACE_OBJECT;
typedef struct WATCHFACE
{
  CLOCK_FACE clockFaces [AMOUNT_WATCHFACE_CLOCKFACES];
  TEXT_FACE textFaces [AMOUNT_WATCHFACE_TEXTS];
  WATCHFACE_OBJECT watchObjects [AMOUNT_WATCHFACE_OBJECTS];
  
  int clockFace_len = 0;
  int textFace_len = 0;
  int watchObject_len = 0;
  
} WATCHFACE;


struct WATCHFACE mWatchface;
struct APP_SCREEN mAppScreen[2];
struct COVER_SCREEN mCoverScreen[2];
struct POPUP_SCREEN mPopUp;
int ACTIVE_APPSCREEN = 1; // ID of the array for APP_SCREEN
int ACTIVE_COVERSCREEN = 1; // ID of the array for COVER_SCREEN
int CURRENT_VIB_PATTERN = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  
  initializeBLE();
  
  initializeMPU();
  initializeRTC();
    
  initializeTouch();
  initializeButtons();
  initalizeVibration();
  
  initializeDisplay();
  // drawBlackRect();
  // printTime(22,35);
  
  initializeNativeAlarmApp();
  
  mWatchface.clockFaces[0].clock_layer = 1;
  mWatchface.clockFaces[0].clock_color = 1;
  mWatchface.watchObjects[0].object_layer = 2;
  mWatchface.watchObjects[0].object_color = 0;
  mWatchface.watchObjects[0].object_move = 6;
  mWatchface.watchObjects[0].object_moveDenominator = 1; // = once
  mWatchface.watchObjects[0].object_moveNumerator = 1; // = once
  mWatchface.watchObjects[0].object_moveInterval = 3; // per minute
  mWatchface.watchObjects[0].object_moveRelToX = 128;
  mWatchface.watchObjects[0].object_moveRelToY = 128;
  mWatchface.watchObjects[0].object_moveResetDenominator = 1;
  mWatchface.watchObjects[0].object_moveResetNumerator = 1;
  mWatchface.watchObjects[0].object_movePace = 10;
  mWatchface.watchObjects[0].object_movePaceMultiplyerExponent = 1;
  
  // mWatchface.watchObjects[1].object_layer = 3;
  // mWatchface.watchObjects[1].object_color = 1;
  drawWatface(&mWatchface);
}

void loop() {
  // printTouchStatus();
  // printTime(1,1);
  
  aci_loop();
  //sendMPUdataViaBLE(0,0,0,0,0,0,0,0);
  
  SECOND++;
  // Only draw watch face if on second changed (or to animate sliding objects..):
  TIME_COUNTER += TIME_LOOP_DELAY; // TODO change so that received time data is compared and counted!
  if((IS_SNACH_CONNECTED && SNACH_SCREEN == 0) || (!IS_SNACH_CONNECTED && NATIVE_SCREEN_INDEX == 0)){
    if(TIME_COUNTER >= SMALLEST_MOVE_PACE){
      updateTime();
      drawWatface(&mWatchface);
      TIME_COUNTER = 0;
    }
  }// else {
  handleDisplayDrawRequests();
  //}
  handlePendingPopups();
  handleNotificationInfos();
  checkNA_Alarm();
  refreshDisplay();
  
  handleVibrationPattern();
  
  Serial.println("SNACH SCREEN: ");
  Serial.println(SNACH_SCREEN);
  Serial.println("NATIVE SNACH SCREEN: ");
  Serial.println(NATIVE_SCREEN_INDEX);
  
  delay(25);
  resetMPU(); // Necessary as otherwise FIFO overflow occours.
  delay(25);
  
  if(!dmpReady){
    // do default action..
    // initializeMPU(); 
    return;
  }
  manageSensorData();
  
  int SNACH_SCREEN_CHANGE = checkTouchStatus();
  int SNACH_BUTTON_CLICKED = checkSnachButtonState();
  if(IS_SNACH_CONNECTED){
    sendSnachDataViaBLE(MPU_XG,MPU_YG,MPU_ZG,MPU_ACCEL.x,MPU_ACCEL.y,MPU_ACCEL.z, SNACH_SCREEN_CHANGE, SNACH_BUTTON_CLICKED, SNACH_SCROLL_STATE);
  } else {
    handleNativeActions(SNACH_SCREEN_CHANGE, SNACH_BUTTON_CLICKED);
  }
  
}

void handleNativeActions(int screenChange, int buttonClicked){
  if(screenChange != 0){
    switch (screenChange){
      case 1:
        NATIVE_SCREEN_INDEX++;
        break;
      case 2:
        NATIVE_SCREEN_INDEX--;
        break;
      case 3:
        NATIVE_SCREEN_INDEX = 0;
        break;
        
    }
    
    if(NATIVE_SCREEN_INDEX > MAX_NATIVE_SCREEN_INDEX){
      NATIVE_SCREEN_INDEX = 0;
    }
    if(NATIVE_SCREEN_INDEX < 0){
      NATIVE_SCREEN_INDEX = MAX_NATIVE_SCREEN_INDEX;
    }
    activateNativeApp(NATIVE_SCREEN_INDEX);
    
  } else {
    switch(NATIVE_SCREEN_INDEX){
      case 0:
        // Watchface
        break;
      case NA_ALARM_INDEX:
        handleNA_AlarmAction(buttonClicked);
        break;
    }
  }
}

void activateNativeApp(int index){
  switch(index){
    case 0:
      // Watchface
      break;
    case NA_ALARM_INDEX:
      activateNA_Alarm();
      break;
  }
  
}


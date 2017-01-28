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
#define SNACH_ACTION_TITLE_START_BYTE 17
#define SNACH_ACTION_TITLE_END_BYTE 18
#define SNACH_ACTION_CONTENT_START_BYTE 2
#define SNACH_ACTION_CONTENT_END_BYTE 3

void setUpActionLayout(byte watchData[], int len_data){
  // sets up a list layout and presets its themes
  ACTIVE_APPSCREEN++;
  if(ACTIVE_APPSCREEN>1){
    ACTIVE_APPSCREEN = 0;
  }
  
  struct ACTION_APP* appAction = &mAppScreen[ACTIVE_APPSCREEN].app_action;
  
  // Back Theme:
  appAction -> action_backTheme.back_theme_ID = watchData[1];
  
  // Icon Theme:
  int iconTheme = watchData[2];
  appAction -> action_iconTheme.icon_theme_ID = iconTheme;
  switch(iconTheme){
    case 0:
      // mid icon
      appAction -> action_iconTheme.xStart = DISPLAY_WIDTH/2 - appAction -> action_iconTheme.iconHeight/2;
      appAction -> action_iconTheme.yStart = DISPLAY_HEIGHT/2 - appAction -> action_iconTheme.iconHeight/2;
      break;
    // -- more icon themes --
  }
  
  // Font Theme:
  int fontTheme = watchData[3];
  switch(fontTheme){
    case 0:
      // title - subtitle ; left aligned ; 10px paddingLeft
      appAction -> action_fontTheme.titleSize = 2;
      appAction -> action_fontTheme.contentSize = 1;
      appAction -> action_fontTheme.titleStartX = 30;
      appAction -> action_fontTheme.titleStartY = 40;
      appAction -> action_fontTheme.contentStartX = 30;
      appAction -> action_fontTheme.contentStartY = 60;
      break;
    case 1:
      // title - subtitle ; left aligned ; 10px paddingLeft
      appAction -> action_fontTheme.titleSize = 2;
      appAction -> action_fontTheme.contentSize = 1;
      appAction -> action_fontTheme.titleStartX = 10;
      appAction -> action_fontTheme.titleStartY = 10;
      appAction -> action_fontTheme.contentStartX = 10;
      appAction -> action_fontTheme.contentStartY = 35;
      break;
    case 2:
      // title - subtitle ; left aligned ; 10px paddingLeft
      appAction -> action_fontTheme.titleSize = 2;
      appAction -> action_fontTheme.contentSize = 1;
      appAction -> action_fontTheme.titleStartX = 20;
      appAction -> action_fontTheme.titleStartY = 40;
      appAction -> action_fontTheme.contentStartX = 10;
      appAction -> action_fontTheme.contentStartY = 10;
      break;
    // -- more themes --
  }
  
  int COLOR_ASSEMB_1 = watchData[4];
  int COLOR_ASSEMB_2 = watchData[5];
  int buffer_color_highlight = 0;
  appAction -> color_background = COLOR_ASSEMB_1 >> 5;
  appAction -> color_main = (COLOR_ASSEMB_1 >> 2) & 7;
  buffer_color_highlight = COLOR_ASSEMB_1 & 3;
  appAction -> color_highlight = ((COLOR_ASSEMB_2 & 3) << 2) | buffer_color_highlight;
  appAction -> color_title = COLOR_ASSEMB_2 >> 5;
  appAction -> color_content = (COLOR_ASSEMB_2 >> 2) & 7;
  
  appAction -> action_iconTheme.mainIcon = watchData[6];
  int mainIconColors = watchData[7];
  appAction -> action_iconTheme.mainIconColor = mainIconColors >> 3;
  appAction -> action_iconTheme.mainIconBackground = mainIconColors & 3;
  
  // Button Icons
  appAction -> icon_b_top = watchData[8];
  appAction -> icon_b_right = watchData[9];
  appAction -> icon_b_bottom = watchData[10];
  appAction -> icon_b_left = watchData[11];
  appAction -> style_b_top = watchData[12];
  appAction -> style_b_right = watchData[13];
  appAction -> style_b_bottom = watchData[14];
  appAction -> style_b_left = watchData[15];
  
  appAction -> color_b_top = watchData[16] >> 3;
  appAction -> color_b_right = watchData[17] >> 3;
  appAction -> color_b_bottom = watchData[18] >> 3;
  appAction -> color_b_left = watchData[19] >> 3;
  appAction -> back_b_top = watchData[16] & 7;
  appAction -> back_b_right = watchData[17] & 7;
  appAction -> back_b_bottom = watchData[18] & 7;
  appAction -> back_b_left = watchData[19] & 7;
}

void setUpActionScreen(byte watchData[], int len_data) {
  struct ACTION_APP* app_action = &mAppScreen[ACTIVE_APPSCREEN].app_action;
  
  // reset previous values:
  memset(&app_action -> action_content.title[0], 0, sizeof(app_action -> action_content.title));
  memset(&app_action -> action_content.subtitle[0], 0, sizeof(app_action -> action_content.subtitle));
  
  Serial.println("setting up action screen");
  
  int dataCursorPosition = 3;
  int bufferCursor = 0;
  for(int c = dataCursorPosition; c < len_data; c++){
    if(watchData[c] == SNACH_ACTION_TITLE_END_BYTE){
      dataCursorPosition+=2;
      break;
    }
    app_action -> action_content.title[bufferCursor] = watchData[c];
    bufferCursor++;
    dataCursorPosition++;
  }
  
  bufferCursor = 0;
  for(int c = dataCursorPosition; c < len_data; c++){
    if(watchData[c] == SNACH_ACTION_CONTENT_END_BYTE){
      bufferCursor+=2;
      break;
    }
    app_action -> action_content.subtitle[bufferCursor] = watchData[c];
    bufferCursor++;
  }
  
  Serial.println("content:");
  Serial.println(app_action -> action_content.subtitle);
  
  showActionScreen(app_action);
}

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
#define THEME_POPUP_MESSAGE 0

void setUpPopupLayout(byte watchData[], int len_data){
  mPopUp.popup_theme_ID = watchData[1];
  mPopUp.icon_b_right = watchData[2];
  mPopUp.icon_b_left = watchData[3];
  mPopUp.popup_level = watchData[4];
  
  switch(mPopUp.popup_theme_ID){
    case THEME_POPUP_MESSAGE:
      mPopUp.style_b_right = 0,
      mPopUp.style_b_right = 0;
      mPopUp.COLOR_BACK = COLOR_WHITE;
      mPopUp.COLOR_TEXT = COLOR_BLACK;
      mPopUp.COLOR_ICON = COLOR_BLACK;
      break;
      // -- more styles --
    // actions of buttons are processed in snach app
  }
  
  int bufferCursor = 0;
  int dataCursor = 5;
  for (dataCursor; dataCursor < len_data; dataCursor++){
    if(watchData[dataCursor] == EOF_RECEIVED_DATA){
      break;
    }
    if(watchData[dataCursor] == ITEM_SEPARATOR_RECEIVED_DATA){
      dataCursor++;
      break;
    }
    mPopUp.title[bufferCursor] = watchData[dataCursor]; 
    bufferCursor++;
  }
  
  bufferCursor = 0;
  for (dataCursor; dataCursor < len_data; dataCursor++){
    if(watchData[dataCursor] == EOF_RECEIVED_DATA){
      break;
    }
    mPopUp.content[bufferCursor] = watchData[dataCursor];
    bufferCursor++;
  }
}

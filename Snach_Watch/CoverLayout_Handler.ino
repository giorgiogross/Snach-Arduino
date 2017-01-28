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
#define THEME_COVER_STANDARD 0

void setUpCoverLayout(byte watchData[], int len_data){
  ACTIVE_COVERSCREEN++;
  if(ACTIVE_COVERSCREEN > 1){
    ACTIVE_COVERSCREEN = 0;
  }
  
  int colors = watchData[2];
  mCoverScreen[ACTIVE_COVERSCREEN].COLOR_BACKGROUND = colors >> 3;
  mCoverScreen[ACTIVE_COVERSCREEN].COLOR_TEXT = colors & 7;
  
  int coverTheme = watchData[3];
  mCoverScreen[ACTIVE_COVERSCREEN].cover_theme_ID = coverTheme;
  switch(coverTheme){
    case THEME_COVER_STANDARD:
      mCoverScreen[ACTIVE_COVERSCREEN].textX = 5;
      mCoverScreen[ACTIVE_COVERSCREEN].textY = 76;
      mCoverScreen[ACTIVE_COVERSCREEN].textSize = 2;
      break;
      // -- more themes --
  }
  
  int bufferCursor = 0;
  for(int c = 4; c < len_data; c++){
    if(watchData[c] == EOF_RECEIVED_DATA){
      break;
    }
    mCoverScreen[ACTIVE_COVERSCREEN].app_title[bufferCursor] = (char) watchData[c];
    bufferCursor++;
  }
  
  showCoverLayout(&mCoverScreen[ACTIVE_COVERSCREEN]);
}

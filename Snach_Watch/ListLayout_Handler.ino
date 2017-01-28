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
#define SNACH_LISTITEM_TITLE_START_BYTE 17
#define SNACH_LISTITEM_TITLE_END_BYTE 18
#define SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM 1
#define SNACH_LISTITEM_CONTENT_START_BYTE 2
#define SNACH_LISTITEM_CONTENT_END_BYTE 3
#define SNACH_LISTITEM_CONTENT_END_BYTE_LASTITEM 4
#define BYTE_ID_SCROLL_UP 0
#define BYTE_ID_SCROLL_DOWN 1
#define BYTE_ID_LISTTITLE_TOP 14
#define BYTE_ID_LISTTITLE_BOTTOM 15

void setUpListItem(byte watchData[], int len_data, boolean isAttachData) {
  // sets up a listitem content and fills and shows the preset theme
  boolean isFastScroll = true;

  boolean isScrollDown = false;
  int newScrollState = watchData[2];
  if (newScrollState == BYTE_ID_SCROLL_DOWN) {
    // SCROLL DOWN
    isScrollDown = true;
  } else if (newScrollState == BYTE_ID_SCROLL_UP) {
    // SCROLL UP
    isScrollDown = false;
  }
  SNACH_SCROLL_STATE = newScrollState;

  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;

  if (isAttachData) {
    // prepare ItemContent and BufferCursor of ItemContent,
    // so that received data is inserted on the right place
    if (IS_APP_ACTIVE) {
      //scroll slowly to next content
      isFastScroll = false;
      dismissCurrentListItem(isScrollDown, isFastScroll);
    } else {
      isFastScroll = true;// this part will never be called...
    }
    if (!isScrollDown) {
      // Shift content from index 0 to the index which equals the size of the received content array
      int dst = 0;
      int contentCursor = 3;
      if (watchData[contentCursor] == SNACH_LISTITEM_TITLE_START_BYTE || watchData[contentCursor] == SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM){
        Serial.println("contatins title ...");
        for (int c = 3; c < len_data; c++) {
          if(watchData[c] == SNACH_LISTITEM_TITLE_END_BYTE){
            contentCursor = c+2;
            Serial.println("content cursor:  ");
            Serial.println(contentCursor);
            break;
          }
        }
      }
      
      for (int c = contentCursor; c < len_data; c++) {
        if (watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE || watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE_LASTITEM) {
          break;
        }
        if (watchData[c] == EOF_RECEIVED_DATA) {
          break;
        }
        Serial.println("Destination memmove: ");
        Serial.println(dst);
        dst++;
      }
      
      Serial.println("Destination memmove: ");
      Serial.println(dst);
      int activeItem = app_list -> activeItem;
      memmove(&app_list -> list_itemContent[activeItem].list_itemContents[dst], &app_list -> list_itemContent[activeItem].list_itemContents[0], app_list -> list_itemContent[activeItem].contentBufferCursor+1);
      app_list -> list_itemContent[activeItem].contentBufferCursor = 0;
    }
  } else {
    // Clear ItemContent and reset BufferCursor of ItemContent to 0
    if (IS_APP_ACTIVE) {
      // scroll fast to next item
      isFastScroll = true;
      dismissCurrentListItem(isScrollDown, isFastScroll);
    } else {
      isFastScroll = true;
    }
    clearListItem(app_list -> activeItem);
    clearListTitles(); // titles will always be resent with each new item
    // changeActiveListItem(app_list);
  }
  loadListItem(watchData, len_data, isScrollDown, isFastScroll);

  /*if (isScrollDown) {
    if (HAS_RECEIVED_ITEM_END) {
      // fast scroll (dismiss current) to next item
      if (IS_APP_ACTIVE) {
        dismissCurrentListItem(isScrollDown, true);
      }
      loadNewListItem(watchData, len_data, isScrollDown);
    }
    else {
      // add data to existing item and scoll down slowly
      extendListItem(watchData, len_data, isScrollDown);
    }
  } else {
    if (HAS_RECEIVED_ITEM_START) {
      // fast scroll (dismiss current) to previous item
      if (IS_APP_ACTIVE) {
        dismissCurrentListItem(isScrollDown, true);
      }
      loadNewListItem(watchData, len_data, isScrollDown);
    } else {
      // add new data to existing item and scroll up slowly
      extendListItem(watchData, len_data, isScrollDown);
    }
  }*/

  IS_APP_ACTIVE = true;
}

void setUpListLayout(byte watchData[], int len_data, boolean isRemoveAnimEntry, boolean isScrollAnimEntry) {
  // sets up a list layout and presets its themes
  ACTIVE_APPSCREEN++;
  if (ACTIVE_APPSCREEN > 1) {
    ACTIVE_APPSCREEN = 0;
  }

  mAppScreen[ACTIVE_APPSCREEN].APP_SCREEN_MODE = watchData[0];

  struct LIST_APP* appList = &mAppScreen[ACTIVE_APPSCREEN].app_list;
  int activeItem = appList -> activeItem;

  appList -> animateRemoveAsEntry = isRemoveAnimEntry;
  appList -> animateScrollAsEntry  = isScrollAnimEntry;

  // Back Theme:
  appList -> list_backTheme.back_theme_ID = watchData[1];

  // Font Theme:
  int fontTheme = watchData[2];
  switch (fontTheme) {
    case 0:
      // title - subtitle ; left aligned ; 10px paddingLeft
      appList -> list_fontTheme.titleSize = 2;
      appList -> list_fontTheme.contentSize = 1;
      appList -> list_fontTheme.titleTopSize = 1;
      appList -> list_fontTheme.titleBottomSize = 1;
      appList -> list_fontTheme.titleStartX = 10;
      appList -> list_fontTheme.titleStartY = 30;
      appList -> list_fontTheme.contentStartX = 10;
      appList -> list_fontTheme.contentStartY = 50;
      appList -> list_fontTheme.titleTopStartX = 0;
      appList -> list_fontTheme.titleTopStartY = 0;
      appList -> list_fontTheme.titleBottomStartX = 0;
      appList -> list_fontTheme.titleBottomStartY = DISPLAY_HEIGHT-10;
      break;
      // -- more themes --
  }

  // Item Theme
  int itemTheme = watchData[5];
  appList -> list_itemTheme.item_theme_ID = itemTheme;
  switch (itemTheme) {
    case 0:
      // one line
      appList -> list_itemTheme.linesAmountContent = 1;
      appList -> list_itemTheme.isSubtitle = true;
      break;
      // -- more themes --
      /*
      e.g. Subtitle:
      Print content in bold.
      */
  }

  int COLOR_ASSEMB_1 = watchData[3];
  int COLOR_ASSEMB_2 = watchData[4];
  int buffer_color_highlight = 0;
  appList -> color_background = COLOR_ASSEMB_1 >> 5;
  appList -> color_main = (COLOR_ASSEMB_1 >> 2) & 7;
  buffer_color_highlight = COLOR_ASSEMB_1 & 3;
  appList -> color_highlight = ((COLOR_ASSEMB_2 & 3) << 2) | buffer_color_highlight;
  appList -> color_title = COLOR_ASSEMB_2 >> 5;
  appList -> color_content = (COLOR_ASSEMB_2 >> 2) & 7;

  // Button Icons
  appList -> icon_b_top = watchData[6];
  appList -> icon_b_right = watchData[7];
  appList -> icon_b_bottom = watchData[8];
  appList -> icon_b_left = watchData[9];
  appList -> icon_b_top_endstate = watchData[10];
  appList -> icon_b_bottom_endstate = watchData[11];

  appList -> style_b_top_endstate = watchData[12];
  appList -> style_b_right = watchData[13];
  appList -> style_b_bottom_endstate = watchData[14];
  appList -> style_b_left = watchData[15];

  appList -> color_b_top = watchData[16] >> 3;
  appList -> color_b_right = watchData[17] >> 3;
  appList -> color_b_bottom = watchData[18] >> 3;
  appList -> color_b_left = watchData[19] >> 3;
  appList -> back_b_top = watchData[16] & 7;
  appList -> back_b_right = watchData[17] & 7;
  appList -> back_b_bottom = watchData[18] & 7;
  appList -> back_b_left = watchData[19] & 7;

}

/*
void extendListItem(byte watchData[], int len_data, boolean isScrollDown) {
  int dataCursorPosition = 3;
  boolean isFastScroll = false;
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;
  boolean isOverflow = false;

  HAS_RECEIVED_ITEM_START = false;
  HAS_RECEIVED_ITEM_END = false;

  if (isScrollDown) {
    int bufferCursor = app_list -> list_itemContent[app_list -> activeItem].contentBufferCursor;

    for (int c = dataCursorPosition; c < len_data; c++) {
      if (watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE) {
        HAS_RECEIVED_ITEM_END = true;
        break;
      }
      if (watchData[c] == SNACH_LISTITEM_CONTENT_START_BYTE) {
        HAS_RECEIVED_ITEM_START = true;
        break;
      }
      if (watchData[c] == EOF_RECEIVED_DATA) {
        HAS_RECEIVED_ITEM_END = false;
        isOverflow = true;
        break;
      }
      app_list -> list_itemContent[app_list -> activeItem].list_itemContents [bufferCursor] = (char) watchData[c];
      bufferCursor++;
    }
    showListItem(app_list, isFastScroll, isScrollDown);

    int activeItem = app_list -> activeItem;
    int srcCursor = app_list -> list_itemContent[activeItem].contentBufferCursor;
    int len = bufferCursor - app_list -> list_itemContent[app_list -> activeItem].contentBufferCursor;
    memmove(&app_list -> list_itemContent[activeItem].list_itemContents[0], &app_list -> list_itemContent[activeItem].list_itemContents[srcCursor], len);
    app_list -> list_itemContent[activeItem].contentBufferCursor = len;
  } else {
    int activeItem = app_list -> activeItem;
    // Determine length of new data:
    /*
    int dst = 0;
    for (int c = dataCursorPosition; c < len_data; c++) {
      if (watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE) {
        dst = c;
        break;
      }
      if (watchData[c] == EOF_RECEIVED_DATA) {
        dst = c;
        break;
      }
    }
    memmove(&app_list -> list_itemContent[activeItem].list_itemContents[dst], &app_list -> list_itemContent[activeItem].list_itemContents[0], app_list -> list_itemContent[activeItem].contentBufferCursor);
    //memset(&app_list -> list_itemContent[activeItem].list_itemContents[0], 0, dst*sizeof(char));
    */

/*
int bufferCursor = 0;
for (int c = dataCursorPosition; c < len_data; c++) {
  if (watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE) {
    HAS_RECEIVED_ITEM_END = true;
    break;
  }
  if (watchData[c] == SNACH_LISTITEM_CONTENT_START_BYTE) {
    HAS_RECEIVED_ITEM_START = true;
    break;
  }
  if (watchData[c] == EOF_RECEIVED_DATA) {
    HAS_RECEIVED_ITEM_END = false;
    isOverflow = true;
    break;
  }
  app_list -> list_itemContent[app_list -> activeItem].list_itemContents [bufferCursor] = (char) watchData[c];
  bufferCursor++;
}
app_list -> list_itemContent[app_list -> activeItem].contentBufferCursor = bufferCursor;
showListItem(app_list, isFastScroll, isScrollDown);

}


}

void loadNewListItem(byte watchData[], int len_data, boolean isScrollDown) {
int dataCursorPosition = 3;
boolean isFastScroll = true;

struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;
app_list -> activeItem++;
if (app_list -> activeItem > 1) {
app_list -> activeItem = 0;
}

while (dataCursorPosition < len_data) {
if (watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE || watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM) {
  app_list -> isFirstItem = watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM;

  int bufferCursor = 0;
  dataCursorPosition++;

  for (int c = dataCursorPosition; c < len_data; c++) {
    if (watchData[c] == SNACH_LISTITEM_TITLE_END_BYTE || watchData[c] == SNACH_LISTITEM_TITLE_END_BYTE_LASTITEM) {
      app_list -> isLastItem = watchData[c] == SNACH_LISTITEM_TITLE_END_BYTE_LASTITEM;
      break;
    }
    app_list -> list_itemContent[app_list -> activeItem].list_itemTitle [bufferCursor] = (char) watchData[c];
    bufferCursor++;
  }
  dataCursorPosition += bufferCursor + 1;

  Serial.println("TITLE read:");
  Serial.println(app_list -> list_itemContent[app_list -> activeItem].list_itemTitle);
  //for(int c = 0; c < bufferCursor; c++){
  //  Serial.print(app_list -> list_itemContent[app_list -> activeItem].list_itemTitle[c]);
  //}

} else if (watchData[dataCursorPosition] == SNACH_LISTITEM_CONTENT_START_BYTE) {
  /*
  Content:
  Fill Array according to received data.
  If there already is data saved, extend data instead of reinitializing the array.

  -> Caution: When scrolling the content, Snach App will send only the new contents,
  so for scrolling the new content should be well aligned right behind/in front of the old one.
  */

/*

HAS_RECEIVED_ITEM_START = true;

int charbuffer_len = 40;
int bufferCursor = 0;
app_list -> list_itemContent[app_list -> activeItem].contentBufferCursor = 0;
dataCursorPosition++;
boolean isOverflow = false;

for (int c = dataCursorPosition; c < len_data; c++) {
  if (watchData[c] == SNACH_LISTITEM_CONTENT_END_BYTE) {
    HAS_RECEIVED_ITEM_END = true;
    break;
  }
  if (watchData[c] == EOF_RECEIVED_DATA) {
    HAS_RECEIVED_ITEM_END = false;
    isOverflow = true;
    break;
  }
  app_list -> list_itemContent[app_list -> activeItem].list_itemContents [bufferCursor] = (char) watchData[c];
  bufferCursor++;

  // dataCursorPosition++;
}
app_list -> list_itemContent[app_list -> activeItem].contentBufferCursor = bufferCursor;
dataCursorPosition += bufferCursor + 1;

if (isOverflow) {
  app_list -> list_itemContent[app_list -> activeItem].list_itemContents[charbuffer_len - 1] = '.';
  app_list -> list_itemContent[app_list -> activeItem].list_itemContents[charbuffer_len - 2] = '.';
  app_list -> list_itemContent[app_list -> activeItem].list_itemContents[charbuffer_len - 3] = '.';
}

Serial.println("SUBTITLE display:");
Serial.println(app_list -> list_itemContent[app_list -> activeItem].list_itemContents);
//for(int c = 0; c < bufferCursor; c++){
//  Serial.print(app_list -> list_itemContent[app_list -> activeItem].list_itemSubtitles[c]);
//}

} else if (watchData[dataCursorPosition] == EOF_RECEIVED_DATA) {
break;
} else {
dataCursorPosition++;
}
}

showListItem(app_list, isFastScroll, isScrollDown);
}*/

void loadListItem(byte watchData[], int len_data, boolean isScrollDown, boolean isFastScroll) {
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;
  int activeItem = app_list -> activeItem;

  int dataCursorPosition = 3;
  int contentBufferCursor = app_list -> list_itemContent[activeItem].contentBufferCursor;
  int contentBufferRemember = contentBufferCursor;

  while (dataCursorPosition < len_data) {
    if (watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE || watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM) {
      app_list -> isFirstItem = watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_START_BYTE_FIRSTITEM;
      dataCursorPosition++;
      int titleBufferCursor = 0;
      for (int c = dataCursorPosition; c < len_data; c++) {
        if (watchData[dataCursorPosition] == SNACH_LISTITEM_TITLE_END_BYTE || watchData[dataCursorPosition] == EOF_RECEIVED_DATA) {
          dataCursorPosition++;
          break;
        }
        app_list -> list_itemContent[activeItem].list_itemTitle[titleBufferCursor] = (char) watchData[c];
        titleBufferCursor++;
        dataCursorPosition++;
      }
    } else if (watchData[dataCursorPosition] == SNACH_LISTITEM_CONTENT_START_BYTE) {
      dataCursorPosition++;
      for (int c = dataCursorPosition; c < len_data; c++) {
        if (watchData[dataCursorPosition] == SNACH_LISTITEM_CONTENT_END_BYTE || watchData[dataCursorPosition] == SNACH_LISTITEM_CONTENT_END_BYTE_LASTITEM || watchData[dataCursorPosition] == EOF_RECEIVED_DATA) {
          app_list -> isLastItem = (uint8_t)watchData[dataCursorPosition] == SNACH_LISTITEM_CONTENT_END_BYTE_LASTITEM;
          dataCursorPosition++;
          break;
        }
        app_list -> list_itemContent[activeItem].list_itemContents[contentBufferCursor] = (char) watchData[c];
        contentBufferCursor++;
        dataCursorPosition++;
      }
    } else if (watchData[dataCursorPosition] == EOF_RECEIVED_DATA) {
      break;
    } else {
      dataCursorPosition++;
    }
  }

  app_list -> list_itemContent[activeItem].contentBufferCursor = contentBufferCursor;
  Serial.println("showing list item with fastScroll:");
  Serial.print(isFastScroll);
  Serial.print(" and  is contentBufferCursor: ");
  Serial.println(contentBufferCursor);

  //isFastScroll = true;
  showListItem(app_list, isFastScroll, isScrollDown);

  if (contentBufferRemember != 0) {
    int srcCursor = contentBufferRemember;
    int len = app_list -> list_itemContent[activeItem].contentBufferCursor - contentBufferRemember;
    memmove(&app_list -> list_itemContent[activeItem].list_itemContents[0], &app_list -> list_itemContent[activeItem].list_itemContents[srcCursor], len);
    memset(&app_list -> list_itemContent[activeItem].list_itemContents[len], 0, sizeof(app_list -> list_itemContent[activeItem].list_itemContents)-len);
    app_list -> list_itemContent[activeItem].contentBufferCursor = len;
  }
}

void changeActiveListItem(struct LIST_APP* app_list) {
  app_list -> activeItem++;
  if (app_list -> activeItem > 1) {
    app_list -> activeItem = 0;
  }
}

void clearListItem(int item) {
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;

  memset(&app_list -> list_itemContent[item].list_itemContents[0], 0, sizeof(app_list -> list_itemContent[item].list_itemContents));
  memset(&app_list -> list_itemContent[item].list_itemTitle[0], 0, sizeof(app_list -> list_itemContent[item].list_itemTitle));
  app_list -> list_itemContent[item].contentBufferCursor = 0;

}

void clearListTitles(){
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;
  memset(&app_list -> title_top[0], 0, sizeof(app_list -> title_top));
  memset(&app_list -> title_bottom[0], 0, sizeof(app_list -> title_bottom));
}

void setUpListTitles(byte watchData[], int len_data){
  struct LIST_APP* app_list = &mAppScreen[ACTIVE_APPSCREEN].app_list;
  
  int bufferCursor = 3;
  while(bufferCursor < len_data){
    if(watchData[bufferCursor] == BYTE_ID_LISTTITLE_TOP){
      memset(&app_list -> title_top[0], 0, sizeof(app_list -> title_top));
      bufferCursor++;
      for(int c = 0; c < len_data; c++){
        if(watchData[bufferCursor] == BYTE_ID_LISTTITLE_TOP || watchData[bufferCursor] == EOF_RECEIVED_DATA){
          bufferCursor++;
          break;
        }
        app_list -> title_top[c] = watchData[bufferCursor];
      }
    } else if(watchData[bufferCursor] == BYTE_ID_LISTTITLE_BOTTOM){
      memset(&app_list -> title_bottom[0], 0, sizeof(app_list -> title_bottom));
      bufferCursor++;
      for(int c = 0; c < len_data; c++){
        if(watchData[bufferCursor] == BYTE_ID_LISTTITLE_BOTTOM || watchData[bufferCursor] == EOF_RECEIVED_DATA){
          bufferCursor++;
          break;
        }
        app_list -> title_bottom[c] = watchData[bufferCursor];
      }
    } else {
      bufferCursor++;
    }
  }
  //  drawListTitles(app_list);
}

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
RTC_DS1307 rtc;

void initializeRTC() {
  rtc.begin();
}

int getHour() {
  return rtc.now().hour();
}

int getMinute() {
  return rtc.now().minute();
}

int getSecond() {
  return rtc.now().minute();
}

void setRTCdata(int year, int month, int day, int hour, int minute, int second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

void updateTime() {
  DateTime now = rtc.now();
  HOUR = now.hour();
  MINUTE = now.minute();
  SECOND = now.second();
  MILLIS = 0;
}

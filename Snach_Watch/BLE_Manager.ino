/* Copyright (c) 2014, Nordic Semiconductor ASA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// Modifications were done by Giorgio Gross

// Data-Stream id-bytes:
#define BYTE_ID_WATCHFACE 0
#define BYTE_ID_LISTLAYOUT 1
#define BYTE_ID_ACTIONLAYOUT 2
#define BYTE_ID_CONTENT 253
#define BYTE_ID_ATTACH_CONTENT_TO_ITEM 252
#define BYTE_ID_UPDATE_LISTTITLES 251
#define BYTE_ID_COVER 3
#define BYTE_ID_POPUP 4
#define BYTE_ID_DISMISS_POPUP 5
#define BYTE_ID_NOTIFICATION_INFO 6
#define BYTE_ID_LISTLAYOUT_REMOVEANIM 7
#define BYTE_ID_LISTLAYOUT_SCROLLANIM 8
#define BYTE_ID_LISTLAYOUT_REMOVE_AND_SCROLLANIM 9
#define BYTE_ID_NATIVE_ALARM 250
#define BYTE_ID_SHARED_FIELD_ALARM 249

boolean IS_SCREENDATA_READY = false;
boolean IS_SHOW_POPUP = false;
boolean IS_SHOWING_POPUP = false;
boolean IS_SHOW_NOTIFICATION_INFO = false;
boolean IS_LL_ATTACH_CONTENT_TO_ITEM = false;
int SCREEN_POST_DELAY_COUNTER = 0;
byte DATA_ARRAY[100];
//byte POPUP_ARRAY[40];
int LEN_DATA = 0;
//int LEN_POPUP = 0;

#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
static services_pipe_type_mapping_t
services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
#define NUMBER_OF_PIPES 0
static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static const hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;

// aci_struct that will contain
// total initial credits
// current credit
// current state of the aci (setup/standby/active/sleep)
// open remote pipe pending
// close remote pipe pending
// Current pipe available bitmap
// Current pipe closed bitmap
// Current connection interval, slave latency and link supervision timeout
// Current State of the the GATT client (Service Discovery)
// Status of the bond (R) Peer address
static struct aci_state_t aci_state;

/*
Temporary buffers for sending ACI commands
*/
static hal_aci_evt_t  aci_data;
static hal_aci_data_t aci_cmd;

/*
Timing change state variable
*/
static bool timing_change_done = false;// NEEDS TO BE SPECIFIED FOR EVERY SINGLE PIPE

/*
Initialize the radio_ack. This is the ack received for every transmitted packet.
*/ /*
static bool radio_ack_pending_xAccel = false;// NEEDS TO BE SPECIFIED FOR EVERY SINGLE PIPE
static bool radio_ack_pending_yAccel = false;
static bool radio_ack_pending_zAccel = false;
static bool radio_ack_pending_xGyro = false;
static bool radio_ack_pending_yGyro = false;
static bool radio_ack_pending_zGyro = false;
*/

/* Define how assert should function in the BLE library */
void __ble_assert(const char *file, uint16_t line)
{
  Serial.print("ERROR ");
  Serial.print(file);
  Serial.print(": ");
  Serial.print(line);
  Serial.print("\n");
  while (1);
}

void initializeBLE() {
  Serial.println(F("Arduino setup"));
  Serial.println(F("Set line ending to newline to send data from the serial monitor"));

  /**
  Point ACI data structures to the the setup data that the nRFgo studio generated for the nRF8001
  */
  if (NULL != services_pipe_type_mapping)
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
  }
  else
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
    Serial.println("services_pipe_type_mapping is null..");
  }
  aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
  aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*)setup_msgs;
  aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

  /*
  Tell the ACI library, the MCU to nRF8001 pin connections.
  The Active pin is optional and can be marked UNUSED
  */
  aci_state.aci_pins.board_name = BOARD_DEFAULT; //See board.h for details REDBEARLAB_SHIELD_V1_1 or BOARD_DEFAULT
  aci_state.aci_pins.reqn_pin   = 9; //SS for Nordic board, 9 for REDBEARLAB_SHIELD_V1_1
  aci_state.aci_pins.rdyn_pin   = 8; //3 for Nordic board, 8 for REDBEARLAB_SHIELD_V1_1
  aci_state.aci_pins.mosi_pin   = MOSI;
  aci_state.aci_pins.miso_pin   = MISO;
  aci_state.aci_pins.sck_pin    = SCK;

  aci_state.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV32;//SPI_CLOCK_DIV8  = 2MHz SPI speed
  //SPI_CLOCK_DIV16 = 1MHz SPI speed

  aci_state.aci_pins.reset_pin              = UNUSED; //4 for Nordic board, UNUSED for REDBEARLAB_SHIELD_V1_1
  aci_state.aci_pins.active_pin             = UNUSED;
  aci_state.aci_pins.optional_chip_sel_pin  = UNUSED;

  aci_state.aci_pins.interface_is_interrupt = false; //Interrupts still not available in Chipkit
  aci_state.aci_pins.interrupt_number       = 1;

  //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
  //If the RESET line is not available we call the ACI Radio Reset to soft reset the nRF8001
  //then we initialize the data structures required to setup the nRF8001
  //The second parameter is for turning debug printing on for the ACI Commands and Events so they be printed on the Serial
  lib_aci_init(&aci_state, true);

  Serial.println(F("Set up done"));

}

void sendSnachDataViaBLE(float sG_x, float sG_y, float sG_z, int sA_x, int sA_y, int sA_z, int currentScreen, int buttonClicked, int scrollState) {
  aci_loop();

  int x_accelLevel = (int)(sA_x / 100) + 128;
  int y_accelLevel = (int)(sA_y / 100) + 128;
  int z_accelLevel = (int)(sA_z / 100) + 128;
  if (x_accelLevel >= 255) {
    x_accelLevel = 254;
  }
  if (y_accelLevel >= 255) {
    y_accelLevel = 254;
  }
  if (z_accelLevel >= 255) {
    z_accelLevel = 254;
  }

  int gX1 = 0;
  int gX2 = 0;
  sG_x += 180;
  if (sG_x > 254) {
    gX1 = 254;
    gX2 = sG_x - 254;
  } else {
    gX1 = sG_x;
  }

  int gY1 = 0;
  int gY2 = 0;
  sG_y += 180;
  if (sG_y > 254) {
    gY1 = 254;
    gY2 = sG_y - 254;
  } else {
    gY1 = sG_y;
  }

  int gZ1 = 0;
  int gZ2 = 0;
  sG_z += 180;
  if (sG_z > 254) {
    gZ1 = 254;
    gZ2 = sG_z - 254;
  } else {
    gZ1 = sG_z;
  }

  uint8_t dataBuffer[12];
  dataBuffer[0] = (uint8_t)gX1;
  dataBuffer[1] = (uint8_t)gX2;
  dataBuffer[2] = (uint8_t)gY1;
  dataBuffer[3] = (uint8_t)gY2;
  dataBuffer[4] = (uint8_t)gZ1;
  dataBuffer[5] = (uint8_t)gZ2;
  dataBuffer[6] = (uint8_t)x_accelLevel;
  dataBuffer[7] = (uint8_t)y_accelLevel;
  dataBuffer[8] = (uint8_t)z_accelLevel;
  dataBuffer[9] = (uint8_t)currentScreen;
  dataBuffer[10] = (uint8_t)buttonClicked;
  dataBuffer[11] = (uint8_t)scrollState;

  Serial.print("data to send:");
  Serial.print(dataBuffer[0]); Serial.print(" "); Serial.print(dataBuffer[1]); Serial.print(" "); Serial.print(dataBuffer[2]); Serial.print(" "); Serial.print(dataBuffer[3]); Serial.print(" ");
  Serial.print(dataBuffer[4]); Serial.print(" "); Serial.print(dataBuffer[5]); Serial.print(" "); Serial.print(dataBuffer[6]); Serial.print(" "); Serial.print(dataBuffer[7]);
  Serial.print(dataBuffer[8]); Serial.print(" "); Serial.print(dataBuffer[9]); Serial.print(" "); Serial.print(dataBuffer[10]); Serial.print(" "); Serial.print(dataBuffer[11]);
  Serial.println("");

  sendBufferedData(dataBuffer);

  //aci_loop();
}

void sendBufferedData(uint8_t data[]) {
  if (lib_aci_is_pipe_available(&aci_state, PIPE_MPU_UART_UART_TX_TX)
      /*&& (false == radio_ack_pending_xAccel)*/
      && (true == timing_change_done))
  {
    if (BLE_send_data(data, PIPE_MPU_UART_UART_TX_TX, 12))
    {
      aci_state.data_credit_available--;
      //Serial.print(F("HRM sent: "));
      //Serial.println(data);
      //radio_ack_pending_xAccel = true;
    } else {
      Serial.println(F("Failed sending x accel data."));
    }
  } else {
    Serial.println(F("PIPE is not available"));
  }
}

boolean BLE_send_data(uint8_t data[], uint8_t pipe, uint8_t bufferLength) {

  if (lib_aci_send_data(pipe, &data[0], bufferLength)) {
    aci_state.data_credit_available--;
    //Serial.print(F("HRM sent: "));
    //Serial.println(b);
    return true;
  }
  return false;
}

void aci_loop()
{
  static bool setup_required = false;

  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&aci_state, &aci_data))
  {
    aci_evt_t * aci_evt;
    aci_evt = &aci_data.evt;

    Serial.println("Event Code: ");
    Serial.println(aci_evt->evt_opcode);

    switch (aci_evt->evt_opcode)
    {
      /**
      As soon as you reset the nRF8001 you will get an ACI Device Started Event
      */
      case ACI_EVT_DEVICE_STARTED:
        {
          aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
          switch (aci_evt->params.device_started.device_mode)
          {
            case ACI_DEVICE_SETUP:
              /**
              When the device is in the setup mode
              */
              Serial.println(F("Evt Device Started: Setup"));
              IS_SNACH_CONNECTED = false;
              resetNativeAppScreenIndex();
              setup_required = true;
              break;

            case ACI_DEVICE_STANDBY:
              Serial.println(F("Evt Device Started: Standby"));
              //Looking for an iPhone by sending radio advertisements
              //When an iPhone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
              if (aci_evt->params.device_started.hw_error)
              {
                delay(20); //Handle the HW error event correctly.
              }
              else
              {
                lib_aci_connect(0/* in seconds : 0 means forever */, 0x0050 /* advertising interval 50ms*/);
                Serial.println(F("Advertising started"));
              }

              break;
          }
        }
        break; //ACI Device Started Event

      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status)
        {
          //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
          Serial.print(F("ACI Command "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
          Serial.print(F("Evt Cmd respone: Status "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_status, HEX);
        }
        if (ACI_CMD_GET_DEVICE_VERSION == aci_evt->params.cmd_rsp.cmd_opcode)
        {
          //Store the version and configuration information of the nRF8001 in the Hardware Revision String Characteristic
          // lib_aci_set_local_data
        }
        break;

      case ACI_EVT_CONNECTED:
        // radio_ack_pending_xAccel  = false;
        timing_change_done = false;
        aci_state.data_credit_available = aci_state.data_credit_total;
        // DO SOME UART INITIALISATION OR UART INIT HERE...
        Serial.println(F("Evt Connected"));
        IS_SNACH_CONNECTED = true;
        break;

      case ACI_EVT_PIPE_STATUS:
        Serial.println(F("Evt Pipe Status"));
        if (lib_aci_is_pipe_available(&aci_state, PIPE_MPU_UART_UART_TX_TX)
            && (false == timing_change_done) )
        {
          /*
          Request a change to the link timing as set in the GAP -> Preferred Peripheral Connection Parameters
          Change the setting in nRFgo studio -> nRF8001 configuration -> GAP Settings and recompile the xml file.
          */
          lib_aci_change_timing_GAP_PPCP();
          timing_change_done = true;
        }

        break;

      case ACI_EVT_TIMING:
        Serial.println(F("Evt link connection interval changed"));
        //        lib_aci_set_local_data...
        break;

      case ACI_EVT_DISCONNECTED:
        Serial.println(F("Evt Disconnected/Advertising timed out"));
        lib_aci_connect(0/* in seconds  : 0 means forever */, 0x0050 /* advertising interval 50ms*/);
        Serial.println(F("Advertising started. "));

        // TODO ADD METHOD TO RESET ALL TEMPORARY BUFFERS
        IS_SNACH_CONNECTED = false;
        resetNativeAppScreenIndex();
        break;

      case ACI_EVT_DATA_RECEIVED:
        Serial.print(F("Received Data at Pipe Number: "));
        Serial.println(aci_evt->params.data_received.rx_data.pipe_number, DEC);
        //delay(20);
        if (aci_evt->len > 0) {
          //uint8_t receivedBuffer[aci_evt->len];
          //receivedBuffer = aci_evt->params.data_received.rx_data.aci_data;

          for (int i = 0; i < aci_evt->len; i++) {
            //receivedBuffer[i] = aci_evt->params.data_received.rx_data.aci_data[i];
            Serial.println(aci_evt->params.data_received.rx_data.aci_data[i]);
          }


          processReceivedData(aci_evt->params.data_received.rx_data.aci_data, aci_evt->len - 2);
        }

        //OUTPUT RECEIVED DATA OF SPECIFIC PIPE*/
        break;

      case ACI_EVT_DATA_CREDIT:
        aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
        /**
        Bluetooth Radio ack received from the peer radio for the data packet sent.
        This also signals that the buffer used by the nRF8001 for the data packet is available again.
        */
        //radio_ack_pending_xAccel = false;
        break;

      case ACI_EVT_PIPE_ERROR:
        /**
        Send data failed. ACI_EVT_DATA_CREDIT will not come.
        This can happen if the pipe becomes unavailable by the peer unsubscribing to the Heart Rate
        Measurement characteristic.
        This can also happen when the link is disconnected after the data packet has been sent.
        */
        // radio_ack_pending_xAccel = false;


        //See the appendix in the nRF8001 Product Specication for details on the error codes
        Serial.print(F("ACI Evt Pipe Error: Pipe #:"));
        Serial.print(aci_evt->params.pipe_error.pipe_number, DEC);
        Serial.print(F("  Pipe Error Code: 0x"));
        Serial.println(aci_evt->params.pipe_error.error_code, HEX);

        //Increment the credit available as the data packet was not sent.
        //The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted
        //for the credit.
        if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aci_evt->params.pipe_error.error_code)
        {
          aci_state.data_credit_available++;
        }
        break;

      case ACI_EVT_HW_ERROR:
        Serial.print(F("HW error: "));
        Serial.println(aci_evt->params.hw_error.line_num, DEC);

        for (uint8_t counter = 0; counter <= (aci_evt->len - 3); counter++)
        {
          Serial.write(aci_evt->params.hw_error.file_name[counter]); //uint8_t file_name[20];
        }
        Serial.println();
        lib_aci_connect(0/* in seconds, 0 means forever */, 0x0050 /* advertising interval 50ms*/);
        Serial.println(F("Advertising started"));
        break;

    }
  }
  else
  {
    Serial.println("No ACI Events available");
    // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
    // Arduino can go to sleep now
    // Wakeup from sleep from the RDYN line
  }

  /* setup_required is set to true when the device starts up and enters setup mode.
   * It indicates that do_aci_setup() should be called. The flag should be cleared if
   * do_aci_setup() returns ACI_STATUS_TRANSACTION_COMPLETE.
   */
  if (setup_required)
  {
    if (SETUP_SUCCESS == do_aci_setup(&aci_state))
    {
      setup_required = false;
    }
  }
}

/**########################################################################################################################################################
                
                                                    Following methods were added by Giorgio Gross
                
########################################################################################################################################################*/

uint8_t receivedDataArray [100];
int currentRecCursorPosition = 0;
// Can receive max. 5 Packets, 100Bytes! -> Prove bevore sending in Snach App if the data to transmit exceeds this maximum..
void processReceivedData(byte *receivedBuffer, int len_buffer) {

  Serial.println("   Size of received data array: ");
  Serial.print(len_buffer);

  for (int i = 0; i < len_buffer; i++) {
    receivedDataArray[currentRecCursorPosition] = receivedBuffer[i];
    Serial.println("received Data Array value:");
    Serial.println(receivedDataArray[currentRecCursorPosition]);

    if (EOF_RECEIVED_DATA == receivedDataArray[currentRecCursorPosition]) {
      // If end byte received (= received data complete) then process assembled arrays:
      processAssembledData(receivedDataArray, currentRecCursorPosition + 1);
      break;
    }

    currentRecCursorPosition++;
  }

  if (isLayoutMode(receivedDataArray[0])) {
    // if its a layout save it, as it will consist of only one package:
    processAssembledData(receivedDataArray, currentRecCursorPosition + 1);
  }
}

boolean isLayoutMode(int newMode){
  return (newMode != BYTE_ID_CONTENT && 
  newMode != BYTE_ID_POPUP && 
  newMode != BYTE_ID_DISMISS_POPUP && 
  newMode != BYTE_ID_NOTIFICATION_INFO && 
  newMode != BYTE_ID_WATCHFACE && 
  newMode != BYTE_ID_ATTACH_CONTENT_TO_ITEM && 
  newMode != BYTE_ID_UPDATE_LISTTITLES); 
}

void processAssembledData(byte dataArray[], int len_data) {
  Serial.println("   Size of assembled data array: ");
  Serial.println(len_data);

  uint8_t dataMode = dataArray[0];
  if (isLayoutMode(dataMode)){
    // if its a layout save the mode:
    SNACH_SCREEN_MODE = dataMode;
  }
  
  Serial.println("  checking dataMode:");
  Serial.println(dataMode);
  Serial.println("  and snach screen mode A:");
  Serial.println(SNACH_SCREEN_MODE);

  switch (dataMode) {
    case BYTE_ID_WATCHFACE:
      //Watchface
      SMALLEST_MOVE_PACE = DEFAULT_MOVE_PACE_LARGE;
      SNACH_SCREEN = (uint8_t) dataArray[1];
      setUpWatchFace(dataArray, len_data);
      SNACH_SCREEN_MODE = BYTE_ID_WATCHFACE;
      break;
    case BYTE_ID_LISTLAYOUT:
      // List Layout
      setUpListLayout(dataArray, len_data, false, false);
      break;
    case BYTE_ID_ACTIONLAYOUT:
      // Action Layout
      setUpActionLayout(dataArray, len_data);
      break;
    case BYTE_ID_COVER:
      // Cover Layout, can be accessed always, e.g. to interrupt pending transitions
      // A cover layout is mandatory for each screen except for watchfaces
      SNACH_SCREEN = (uint8_t) dataArray[1];
      
      setUpCoverLayout(dataArray, len_data);
      SCREEN_POST_DELAY_COUNTER = 8;
      break;
    case BYTE_ID_POPUP:
      // Pop up
      IS_SHOWING_POPUP = false;
      setUpPopupLayout(dataArray, len_data);
      
      // LEN_POPUP = len_data;
      // memcpy(&POPUP_ARRAY[0], &dataArray[0], len_data);
      IS_SHOW_POPUP = true;
      setVibPatternPopup();
      break;
    case BYTE_ID_DISMISS_POPUP:
      // Dismiss pop up:
      IS_SHOW_POPUP = false;
    case BYTE_ID_NOTIFICATION_INFO:
      // Show notification info
      IS_SHOW_NOTIFICATION_INFO = true;
      setVibPatternNotif();
      break;
    case BYTE_ID_LISTLAYOUT_SCROLLANIM:
      setUpListLayout(dataArray, len_data, false, true);
      break;
    case BYTE_ID_LISTLAYOUT_REMOVEANIM:
      setUpListLayout(dataArray, len_data, true, false);
      break;
    case BYTE_ID_LISTLAYOUT_REMOVE_AND_SCROLLANIM:
      setUpListLayout(dataArray, len_data, true, true);
      break;
    case BYTE_ID_UPDATE_LISTTITLES:
      // setUpListTitles(dataArray, len_data);
      break;
    case BYTE_ID_NATIVE_ALARM:
      // Correct the screen index and let the native alarm app call this method with its own data.
      overrideScreenIndex_NA_Alarm((uint8_t) dataArray[1]);
      activateNA_Alarm();
      break;
    case BYTE_ID_ATTACH_CONTENT_TO_ITEM:
      // Attach received content to existing list item
      IS_LL_ATTACH_CONTENT_TO_ITEM = true;
      // Content Data, handle in main thread
      SNACH_SCREEN = (uint8_t) dataArray[1];
      IS_SCREENDATA_READY = true;
      LEN_DATA = len_data;
      memcpy(&DATA_ARRAY[0], &dataArray[0], len_data);
      break;
    case BYTE_ID_CONTENT:
      // Content Data, handle in main thread
      IS_LL_ATTACH_CONTENT_TO_ITEM = false;
      
      Serial.println("  checking dataArray[1]:");
      Serial.println((uint8_t) dataArray[1]);
      
      SNACH_SCREEN = (uint8_t) dataArray[1];
      IS_SCREENDATA_READY = true;
      LEN_DATA = len_data;
      memcpy(&DATA_ARRAY[0], &dataArray[0], len_data);
      break;

      // further cases for watch-data, app data, etc.
  }
  resetReceivedBuffers();
  
  Serial.println("  checking Screen Mode:");
  Serial.println(SNACH_SCREEN_MODE);
  Serial.println("  and snach screen:");
  Serial.println(SNACH_SCREEN);
}

void handleDisplayDrawRequests() {
  if (IS_SCREENDATA_READY && SCREEN_POST_DELAY_COUNTER <= 0) {
    
    SCREEN_POST_DELAY_COUNTER = 0;
    switch (SNACH_SCREEN_MODE) {
      case BYTE_ID_WATCHFACE:
        // watch-data
        HOUR = DATA_ARRAY[2];
        MINUTE = DATA_ARRAY[3];
        SECOND = DATA_ARRAY[4];
        MILLIS = DATA_ARRAY[5];// alternatively: count millis as 100ms in loop and reset when 1000ms are reached
        setRTCdata(2015, 6, 3, DATA_ARRAY[2], DATA_ARRAY[4], DATA_ARRAY[5]);
        break;
      case BYTE_ID_LISTLAYOUT:
        // update list layout
        setUpListItem(DATA_ARRAY, LEN_DATA, IS_LL_ATTACH_CONTENT_TO_ITEM);
        break;
      case BYTE_ID_LISTLAYOUT_SCROLLANIM:
        setUpListItem(DATA_ARRAY, LEN_DATA, IS_LL_ATTACH_CONTENT_TO_ITEM);
        break;
      case BYTE_ID_LISTLAYOUT_REMOVEANIM:
        setUpListItem(DATA_ARRAY, LEN_DATA, IS_LL_ATTACH_CONTENT_TO_ITEM);
        break;
      case BYTE_ID_LISTLAYOUT_REMOVE_AND_SCROLLANIM:
        setUpListItem(DATA_ARRAY, LEN_DATA, IS_LL_ATTACH_CONTENT_TO_ITEM);
        break;
      case BYTE_ID_ACTIONLAYOUT:
        // update action layut
        setUpActionScreen(DATA_ARRAY, LEN_DATA);
        break;
    }
    
    IS_SCREENDATA_READY = false;
  } else {
    SCREEN_POST_DELAY_COUNTER --;
  }
  // refreshDisplay();
}

void handlePendingPopups() {
  if(IS_SHOW_POPUP){
    // IS_SHOW_POPUP = false; // -> set to false when popup is dismissed or used.
    showPopup(&mPopUp, &IS_SHOWING_POPUP);
    // refreshDisplay();
  }
}

void handleNotificationInfos(){
  if(IS_SHOW_NOTIFICATION_INFO){
    showNotificationInfo();
  }
  if(SNACH_SCREEN == 1){
    IS_SHOW_NOTIFICATION_INFO = false;
  }
}

void resetReceivedBuffers() {
  currentRecCursorPosition = 0;
  memset(&receivedDataArray[0], 0, sizeof(receivedDataArray));
}

void resetNativeAppScreenIndex(){
  NATIVE_SCREEN_INDEX = 0;
  overrideScreenIndex_NA_Alarm(1);
}

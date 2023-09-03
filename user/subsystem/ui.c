#include "ui.h"
#include "gpio.h"
#include "keyboard.h"
#include "oled.h"
#include "timer.h"
#include <string.h>
#include "log.h"

// forward declare
bool_t ui_tick(void);

OLEDPort ui_oled = {
    .en = {GPIOC, GPIO_PINS_SOURCE13},
    .i2c = {.data = {GPIOA, GPIO_PINS_SOURCE12},
            .clk = {GPIOB, GPIO_PINS_SOURCE7},
            .address = 0x78},
};

KeyboardPort ui_enter = {
  .port={GPIOC, GPIO_PINS_SOURCE14},
  .capture=KEY_ACTIVE_LOW
};

KeyboardPort ui_select = {
  .port={GPIOC, GPIO_PINS_SOURCE15},
  .capture=KEY_ACTIVE_LOW
};

/* menu */
/* 1. status */
/*    show system status: */
/*  station id, board type, key info */
/* 2. setup */
/* 2.1 set canstation id */
/* 2.1.1  select to change value, longpress accelerate; click enter to quit, longpress enter to confirm,    */
/* 3. screen off */

/* state machine
 * 0-> init status            go->1
 * 1-> show system status     longpress enter-> 2, timeout -> 6
 * 2-> show menu.status       select ->3, longpress enter ->1
 * 3-> show menu.setup        select ->4, longpress enter ->7
 * 4-> show menu.screen off   select ->5, longpress enter ->5
 * 5-> show menu.screen timeout    select ->2, longpress enter -> 10
 * 6-> screen off             go->7 after screen off
 * 7-> waiting for wakeup     select ->2, enter -> 1
 * 8-> setting canstation id  select -> increase station id, longpress select->
 *                            accelerate increae station id, enter -> 1, longpress
 *                            enter ->9
 * 9-> write station id       go ->1
 * 10-> set screen timeout    select -> increase timeout, longpress
 *                            accelerate increaing timeout, enter -> 1, longpress
 *                            enter -> 1
 */

enum UIState{
  UI_INIT,
  UI_DO_SHOW_STATUS,
  UI_SHOW_STATUS,
  UI_MENU_STATUS,
  UI_MENU_SETUP,
  UI_MENU_SCREENOFF,
  UI_MENU_SCREENOFF_TIMEOUT,
  UI_DO_SCREENOFF,
  UI_WAITING_FOR_WAKEUP,
  UI_SETTING_STATION_ID,
  UI_DO_WRITE_STATION_ID,
  UI_SETTING_SCREEN_TIMEOUT,
};

enum UIState ui_state = UI_INIT, last_ui_state = UI_INIT;
uint8_t station_id=0;
//uint8_t station_id_stored=1;
enum SCREENTIMEOUT{
  SECOND_5,
  SECOND_10,
  SECOND_30,
  MINUTE_1,
  MINUTE_2,
  MINUTE_5,
  MINUTE_10,
  MINUTE_15,
  MINUTE_30,
  SCREENTIMEOUT_COUNT
};

enum SCREENTIMEOUT screen_timeout = SECOND_5;
//enum SCREENTIMEOUT screen_timeout_stored = SECOND_5;
uint16_t screen_timeout_current = 0;
uint16_t screen_timeout_current_ms_counter = 0;

store_id_cb func_store_id_cb = NULL;
get_id_cb func_get_id_cb = NULL;
store_timeout_cb func_store_timeout_cb = NULL;
get_timeout_cb func_get_timeout_cb = NULL;

uint16_t timeout_to_seconds(enum SCREENTIMEOUT timeout){
  return screen_timeout == SECOND_5 ? 5 :		\
    screen_timeout == SECOND_10 ? 10 :			\
    screen_timeout == SECOND_30 ? 30 :			\
    screen_timeout == MINUTE_1 ? 60 :			\
    screen_timeout == MINUTE_2 ? 120 :			\
    screen_timeout == MINUTE_5 ? 300 :			\
    screen_timeout == MINUTE_10 ? 600 :			\
    screen_timeout == MINUTE_15 ? 900 :			\
    screen_timeout == MINUTE_30 ? 1800 : 1800;
}

void ui_tick_wrap(){
  while(ui_tick());
}

void init_ui(store_id_cb f1, get_id_cb f2, store_timeout_cb f3, get_timeout_cb f4){

  func_store_id_cb = f1;
  func_get_id_cb = f2;
  func_store_timeout_cb = f3;
  func_get_timeout_cb = f4;
  
  init_keyboard(&ui_enter);
  init_keyboard(&ui_select);
  init_oled(&ui_oled);
  ui_state = UI_INIT;
  last_ui_state = UI_INIT;
  timer_add_tick(ui_tick_wrap);
}

void dump_keystate(KeyboardPort * port, enum KEYSTATUS status){
  log_printf("State:%s, Key:%s  evt:%s\n",
             ui_state == UI_INIT                     ? "init"
             : ui_state == UI_DO_SHOW_STATUS         ? "prepare to show status"
             : ui_state == UI_SHOW_STATUS            ? "show status"
             : ui_state == UI_MENU_STATUS            ? "menu status"
             : ui_state == UI_MENU_SETUP             ? "menu setup"
             : ui_state == UI_MENU_SCREENOFF         ? "menu screenoff"
             : ui_state == UI_MENU_SCREENOFF_TIMEOUT ? "screen timeout"
             : ui_state == UI_DO_SCREENOFF           ? "do screenoff"
             : ui_state == UI_WAITING_FOR_WAKEUP     ? "waiting for wakeup"
             : ui_state == UI_SETTING_STATION_ID     ? "setting station id"
             : ui_state == UI_DO_WRITE_STATION_ID    ? "do write station id"
             : ui_state == UI_SETTING_SCREEN_TIMEOUT ? "setting timeout"
                                                     : "unknown",

             port == &ui_enter ? "enter" : "select",
             status == KEY_PRESS       ? "PRESS"
             : status == KEY_RELEASE   ? "RELEASE"
             : status == KEY_CLICKED   ? "CLICK"
             : status == KEY_LONGPRESS ? "LONGPRESS"
                                       : "DUMMY");
}

void ui_update(void){
  char buf[20];
  switch (ui_state){
  case UI_INIT:
    break;
  case UI_SHOW_STATUS:
    oled_showstring(&ui_oled, 0, 0, "Status", 0, 8, 1);
    sprintf(buf, "StationID:");//, station_id_stored);
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    sprintf(buf, "%d", func_get_id_cb != NULL ? (*func_get_id_cb)() : 0);
    oled_showstring(&ui_oled, 0, 20, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_DO_SHOW_STATUS:
    oled_refresh(&ui_oled);
    break;
  case UI_MENU_STATUS:
    oled_showstring(&ui_oled, 0, 0, "Menu", 0, 8, 1);
    sprintf(buf, "Status");
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_MENU_SETUP:
    oled_showstring(&ui_oled, 0, 0, "Menu", 0, 8, 1);
    sprintf(buf, "Setup");
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_MENU_SCREENOFF:
    oled_showstring(&ui_oled, 0, 0, "Menu", 0, 8, 1);
    sprintf(buf, "ScreenOff");
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_MENU_SCREENOFF_TIMEOUT:
    oled_showstring(&ui_oled, 0, 0, "Menu", 0, 8, 1);
    sprintf(buf, "Setup screen");
    //    timeout");
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    sprintf(buf, "timeout");
    oled_showstring(&ui_oled, 0, 20, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_DO_SCREENOFF:
    break;
  case UI_WAITING_FOR_WAKEUP:
    break;
  case UI_SETTING_STATION_ID:
    oled_showstring(&ui_oled, 0, 0, "Set stationID", 0, 8, 1);
    sprintf(buf, "ID:%d", station_id);
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break;
  case UI_DO_WRITE_STATION_ID:
    
    break;
  case UI_SETTING_SCREEN_TIMEOUT:
    oled_showstring(&ui_oled, 0, 0, "Set screen", 0, 8, 1);
    sprintf(buf, "timeout:");
    oled_showstring(&ui_oled, 0, 10, buf, strlen(buf), 8, 1);
    sprintf(buf, "%s",
	     screen_timeout == SECOND_5 ? "5 sec" :	\
	     screen_timeout == SECOND_10 ? "10 sec" :	\
	     screen_timeout == SECOND_30 ? "30 sec" :	\
	     screen_timeout == MINUTE_1 ? "1 min" :	\
	     screen_timeout == MINUTE_2 ? "2 min" :	\
	     screen_timeout == MINUTE_5 ? "5 min" :	\
	     screen_timeout == MINUTE_10 ? "10 min" :	\
	     screen_timeout == MINUTE_15 ? "15 min" :		\
	     screen_timeout == MINUTE_30 ? "30 min" : "Unknown"
	     );
    oled_showstring(&ui_oled, 0, 20, buf, strlen(buf), 8, 1);
    oled_refresh(&ui_oled);
    break; 
  }
}

void keyboard_triggered_state_transfer(KeyboardPort * port, enum KEYSTATUS status){
  switch (ui_state) {
  case UI_DO_SHOW_STATUS:
    break;
  case UI_SHOW_STATUS:
    if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      ui_state = UI_MENU_STATUS;
      keyboard_suppress_longpress(&ui_enter);
    }
    break;
  case UI_MENU_STATUS:
    if ((port == &ui_select) && (status == KEY_CLICKED))
      ui_state = UI_MENU_SETUP;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      ui_state = UI_DO_SHOW_STATUS;
      keyboard_suppress_longpress(&ui_enter);
    }
    break;
  case UI_MENU_SETUP:
    if ((port == &ui_select) && (status == KEY_CLICKED))
      ui_state = UI_MENU_SCREENOFF;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      ui_state = UI_SETTING_STATION_ID;
      station_id = func_get_id_cb != NULL ? (*func_get_id_cb)() : 0; //station_id_stored;
      keyboard_suppress_longpress(&ui_enter);
    }
    break;
  case UI_MENU_SCREENOFF:
    if ((port == &ui_select) && (status == KEY_CLICKED))
      ui_state = UI_MENU_SCREENOFF_TIMEOUT;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      keyboard_suppress_longpress(&ui_enter);
      ui_state = UI_DO_SCREENOFF;
    }
    break;
  case UI_MENU_SCREENOFF_TIMEOUT:
    if ((port == &ui_select) && (status == KEY_CLICKED))
      ui_state = UI_MENU_STATUS;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      keyboard_suppress_longpress(&ui_enter);
      ui_state = UI_SETTING_SCREEN_TIMEOUT;
      screen_timeout = func_get_timeout_cb != NULL ? (*func_get_timeout_cb)() : 0;//screen_timeout_stored;
    }
    break;
  case UI_DO_SCREENOFF:
    oled_display_off(&ui_oled);
    ui_state = UI_WAITING_FOR_WAKEUP;
    break;
  case UI_WAITING_FOR_WAKEUP:
    if ((port == &ui_select) && (status == KEY_CLICKED)){
      ui_state = UI_MENU_STATUS;
      oled_display_on(&ui_oled);
    }
    else if ((port == &ui_enter) && (status == KEY_CLICKED))
      ui_state = UI_DO_SHOW_STATUS;
    break;
  case UI_SETTING_STATION_ID:
    if ((port == &ui_select) &&
        (status == KEY_CLICKED || status == KEY_LONGPRESS)) {
      /* increase station id */
      station_id = (station_id + 1) % 256;
      last_ui_state = UI_INIT; /* force refresh */
    } else if ((port == &ui_enter) && (status == KEY_CLICKED))
      ui_state = UI_DO_SHOW_STATUS;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      ui_state = UI_DO_WRITE_STATION_ID;
      keyboard_suppress_longpress(&ui_enter);
    }
    break;
  case UI_SETTING_SCREEN_TIMEOUT:
    if ((port == &ui_select) &&
        (status == KEY_CLICKED || status == KEY_LONGPRESS)) {
      /* increase timeout */
      screen_timeout = (screen_timeout + 1) % SCREENTIMEOUT_COUNT;
      last_ui_state = UI_INIT; /* force refresh */
    } else if ((port == &ui_enter) && (status == KEY_CLICKED))
      ui_state = UI_DO_SHOW_STATUS;
    else if ((port == &ui_enter) && (status == KEY_LONGPRESS)) {
      ui_state = UI_DO_SHOW_STATUS;
      func_store_timeout_cb != NULL ? (*func_store_timeout_cb)(screen_timeout) : 0;
      keyboard_suppress_longpress(&ui_enter);
    }
    break;
  default:
  }
}

void tick_triggered_state_transfer(){
  switch (ui_state){
  case UI_INIT:
    ui_state = UI_DO_SHOW_STATUS;
    //printf("UI_INIT -> UI_DO_SHOW_STATUS\n");
    break;
  case UI_DO_WRITE_STATION_ID:
    ui_state = UI_DO_SHOW_STATUS;
    func_store_id_cb != NULL ? (*func_store_id_cb)(station_id) : 0;
    break;
  case UI_DO_SHOW_STATUS:
    screen_timeout_current_ms_counter = 1000;
    screen_timeout_current = timeout_to_seconds(					       func_get_timeout_cb != NULL ? func_get_timeout_cb() : 0); // screen_timeout_stored);
    ui_state = UI_SHOW_STATUS;
    oled_display_on(&ui_oled);
    //printf("UI_DO_SHOW_STATUS -> UI_SHOW_STATUS, %d\n", screen_timeout_current);
    break;
  case UI_SHOW_STATUS:
    if (screen_timeout_current_ms_counter == 0){
      screen_timeout_current_ms_counter = 1000;
      if(screen_timeout_current == 0){ /* triggered */
	ui_state = UI_DO_SCREENOFF;
      }
      //printf("screen timout:%d\n", screen_timeout_current);
      screen_timeout_current -= 1;
    } else screen_timeout_current_ms_counter -= 1;
    break;
  case UI_DO_SCREENOFF:
    oled_display_off(&ui_oled);
    ui_state = UI_WAITING_FOR_WAKEUP;
    break;
  default:
    break;
  }
}

bool_t ui_tick(void){		/* per 1ms */
  KeyboardPort * port;
  enum KEYSTATUS status;
  
  if(keyboard_get_event(&port, &status)) { /* state transfer */
    //dump_keystate(port, status);
    keyboard_triggered_state_transfer(port, status);
    return 1;
  }
  tick_triggered_state_transfer();
  
  if (last_ui_state != ui_state) { /* update ui */
    oled_clear(&ui_oled);
    ui_update();
  }
  last_ui_state = ui_state;

  return 0;
}

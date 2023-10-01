#include "keyboard.h"
#include "gpio.h"
#include "timer.h"
#include <stdint.h>
#include "log.h"

KeyboardPort * monitor[KEYBOARD_MAX_KEYS];
uint8_t keyboard_monitor_num=0;
bool_t queue_inited=0;
/* enum KEYSTATUS event_queue[KEYBOARD_MAX_KEYS]; */
/* uint8_t event_queue_num; */

/* simple FIFO */
#define QUEUE_SIZE KEYBOARD_MAX_KEYS

typedef struct {
  enum KEYSTATUS items[QUEUE_SIZE];
  KeyboardPort * item_ports[QUEUE_SIZE];
  int front;
  int rear;
  int count;
} CircularQueue;

void init_queue(CircularQueue *queue) {
  queue->front = 0;
  queue->rear = -1;
  queue->count = 0;
}

// 向队列中添加一个元素
bool_t queue_put(CircularQueue *queue, enum KEYSTATUS item, KeyboardPort * item_port) {
  if(queue->count >= QUEUE_SIZE) {
    //printf("Queue is full\n");
    return 0;
  }
  queue->rear = (queue->rear + 1) % QUEUE_SIZE;
  queue->items[queue->rear] = item;
  queue->item_ports[queue->rear] = item_port;
  queue->count++;
  return 1;
}

// 从队列中获取一个元素
bool_t queue_get(CircularQueue *queue, enum KEYSTATUS *item, KeyboardPort ** item_port) {
  if(queue->count <= 0) {
    //printf("Queue is empty\n");
    return 0;
  }
  *item = queue->items[queue->front];
  *item_port = queue->item_ports[queue->front];
  queue->front = (queue->front + 1) % QUEUE_SIZE;
  queue->count--;
  return 1;
}

// 获取队列中的元素个数
uint16_t queue_size(CircularQueue *queue) {
  return queue->count;
}

CircularQueue keyboard_event_queue;


void keyboard_tick(){		/* called every 1ms */
  uint8_t i;
  uint8_t key_val;
  enum KEYSTATUS status = KEY_DUMMY;
  KeyboardPort * triggered_port = NULL;

  for (i=0; i<keyboard_monitor_num; i++){
    KeyboardPort *port = monitor[i];
    key_val = gpio_read(&port->port);
    if (port->key_last_val != key_val) { /* key status changed */
      /* printf("key_val:%d  last_val: %d  countdown:%d  restart:%d\n", */
      /* 	     key_val, port->key_last_val,port->count_down, port->restart); */

      /* restart:1 => waiting for key press down */
      if (((port->capture == KEY_ACTIVE_HIGH) && (key_val)) ||		\
	   ((port->capture == KEY_ACTIVE_LOW) && (!key_val))) { /* something triggered */
	if (port->count_down > 1) port->count_down -=1;	  /* soft schmitt trigger */
	else if (port->count_down == 1){		  /* triggered! */
	  port->key_last_val = key_val;
	  triggered_port = port;
	  status = KEY_PRESS;
	  port->restart = 0;
	  port->count_down = REPEATIVE_TRIGGER_CYCLE;
	} else {		/* init schmitt trigger */
	  port->count_down = SCHMITT_TRIGGER_PRESS - 1;
	}
      } else if (((port->capture == KEY_ACTIVE_HIGH) && (!key_val)) ||	\
		  ((port->capture == KEY_ACTIVE_LOW) && (key_val))) { /* something released */
	if (port->count_down > 2) port->count_down -=1;	  /* soft schmitt trigger */
	else if (port->count_down == 2){		  /* released detect, send clicked */
	  port->count_down--;
	  if (!port->restart){
	    triggered_port = port;
	    status = KEY_CLICKED;
	  }
	} else if (port->count_down == 1){ /* released detect again, send release */
	  port->count_down = 0;
	  port->key_last_val = key_val;
          port->restart = 1;
          if (!port->restart){
	    triggered_port = port;
	    status = KEY_RELEASE;
	  }
	} else {		/* init schmitt trigger */
	  port->count_down = SCHMITT_TRIGGER_PRESS;
	}
      }
    } else if (!(port->restart) &&					\
	       (((port->capture == KEY_ACTIVE_HIGH) && (key_val)) ||	\
		((port->capture == KEY_ACTIVE_LOW) && (!key_val)))) { /* repeative? */
      if (port->count_down == 1) {			       /* need to send repeative click */
	triggered_port = port;
	status = KEY_LONGPRESS;
	port->count_down = REPEATIVE_TRIGGER_CYCLE;
      } else if(port->count_down > 1) port->count_down--;
    }
  }

  if (queue_inited && (status != KEY_DUMMY)) {
    queue_put(&keyboard_event_queue, status, triggered_port);
  }
}

/* keyboard handling */
void init_keyboard(KeyboardPort *devport) {
  
  init_gpio_input(&devport->port,
		  devport->capture == KEY_ACTIVE_LOW ? GPIO_PULL_UP : GPIO_PULL_DOWN,
		  GPIO_DRIVE_STRENGTH_STRONGER);
  devport->key_last_val = KEY_DUMMY;
  devport->restart = 1;
  if (!queue_inited){
    init_queue(&keyboard_event_queue);
    queue_inited = 1;
  }
  timer_pause();
  monitor[keyboard_monitor_num++] = devport;
  if (keyboard_monitor_num == KEYBOARD_MAX_KEYS){
    log_printf("Too many monitored keys\n");
    keyboard_monitor_num = 0;
  }
  timer_add_tick(keyboard_tick, 0);
  timer_resume();
}

void keyboard_suppress_longpress(KeyboardPort *port){
  port->count_down = 0;
  port->key_last_val = gpio_read(&port->port);
  port->restart = 1;
}

bool_t keyboard_get_event(KeyboardPort **triggered_port, enum KEYSTATUS * status){
  if(queue_inited && queue_get(&keyboard_event_queue, status, triggered_port))
    return 1;

  *status = KEY_DUMMY;
  return 0;
}

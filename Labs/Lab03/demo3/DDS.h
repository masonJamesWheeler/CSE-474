#define BIT0 1<<0
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3

#define TIMER_REG_A TCCR4A
#define TIMER_REG_B TCCR4B
#define TIMER_COUNTER TCNT4H
#define PRESCALER OCR4A
#define LED_DDR DDRL
#define LED_PORT PORTL
#define SPEAKER_DDR DDRH
#define SPEAKER_PORT PORTH

#define FLASH_DURATION 250
#define NFLASH 3
#define PLAY_DURATION 200
#define ADURATION 2000
#define CDURATION 10000
#define PAUSE_DURATION 4000
#define PICKUP_TIME (NMELODY * PLAY_DURATION) + PAUSE_DURATION

#define NOTE_C  262
#define NOTE_CS 277
#define NOTE_D  294
#define NOTE_DS 311
#define NOTE_E  330
#define NOTE_F  349
#define NOTE_FS 370
#define NOTE_G  392
#define NOTE_GS 415
#define NOTE_A  440
#define NOTE_AS 466
#define NOTE_B  494
#define NOTE_C5 523

#define NMELODY 72
int melody[] = {
  NOTE_E, NOTE_E, 0, NOTE_E, 0, NOTE_C, NOTE_E, 0,
  NOTE_G, 0, 0,  0, NOTE_G, 0, 0, 0,
  NOTE_C5, 0, 0, NOTE_G, 0, 0, NOTE_E, 0,
  0, NOTE_A, 0, NOTE_B, 0, NOTE_AS, NOTE_A, 0,
  NOTE_G, NOTE_E, NOTE_G, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5,
  0, 0, NOTE_G, 0, 0, NOTE_E, 0, 0,
  NOTE_G, 0, 0, NOTE_A, 0, NOTE_F, NOTE_G, 0,
  NOTE_E, 0, NOTE_C, NOTE_D, NOTE_B, 0, 0, NOTE_C5
};

#define NTASKS 10

typedef enum {READY, RUNNING, SLEEPING, DEAD} STATE;
typedef enum flagState {PENDING, DONE} FLAG;

typedef struct TCB {
  char name[20];
  void (*fn_ptr)();
  int id;
  int nTimes;
  int timeSleep;
  int time;
  STATE state;
} tcb;

void sleep_474(long t);

void schedule_sync();

void copy_tcb(tcb * dst, tcb * src);

void task_self_quit();

void task_start(tcb * task);

void task_load(void (*fn_ptr)(), const char * name);

tcb * find_dead_task(const char * name);

void setOC4AFreq(uint32_t freq);

void task1();

void task2();

void speakerSetup();

void ledSetup();

/***************************************************************************

  2610intf.c

  The YM2610 emulator supports up to 2 chips.
  Each chip has the following connections:
  - Status Read / Control Write A
  - Port Read / Data Write A
  - Control Write B
  - Data Write B

***************************************************************************/


#include <stdio.h>
#include "ay8910.h"
#include "fm.h"
#include "2610intf.h"
#include "timer.h"
#include "sound.h"

#include "neocd.h"

#if BUILD_YM2610


static timer_struct *Timer[2];

/*------------------------- TM2610 -------------------------------*/
/* IRQ Handler */
static void IRQHandler(int n, int irq)
{
    neogeo_sound_irq(irq);
}

/* Timer overflow callback from timer.c */
void timer_callback_2610(int param)
{
    Timer[param] = 0;
    YM2610TimerOver(param);
}

/* TimerHandler from fm.c */
static void TimerHandler(int c, int count, float stepTime)
{

    if (count == 0) 
    {	/* Reset FM Timer */
	if (Timer[c]) {
	    del_timer(Timer[c]);   Timer[c] = 0;
	}
    } 
    else 
    {	/* Start FM Timer */
	float timeSec = (float) count * stepTime;

	if (Timer[c] == 0) {
	    Timer[c] =	(timer_struct *) insert_timer(timeSec, c,   timer_callback_2610);
	}
    }
}

void FMTimerInit(void)
{
    Timer[0] = Timer[1] = 0;
    free_all_timer();
}

int YM2610_sh_start(int rate)
{

  if (AY8910_sh_start(rate))
	return 1;

  FMTimerInit();
     
  if (YM2610Init(8000000, 
  		 rate,
		 (void *) neogeo_pcm_memory, 
		 0x100000, 
		 NULL, 
		 0,
		 TimerHandler, IRQHandler) == 0)
   return 0;

   printf("error\n");
   return 1;
}


/************************************************/
/* Sound Hardware Stop				*/
/************************************************/
void YM2610_sh_stop(void)
{
    YM2610Shutdown();
}

/* reset */
void YM2610_sh_reset(void)
{
    YM2610ResetChip();
}

/************************************************/
/* Status Read for YM2610 - Chip 0		*/
/************************************************/
inline uint32 YM2610_status_port_0_A_r(uint32 offset)
{
    return YM2610Read(0);
}

inline uint32 YM2610_status_port_0_B_r(uint32 offset)
{
    return YM2610Read(2);
}

/************************************************/
/* Port Read for YM2610 - Chip 0		*/
/************************************************/
inline uint32 YM2610_read_port_0_r(uint32 offset)
{
    return YM2610Read(1);
}


/************************************************/
/* Control Write for YM2610 - Chip 0		*/
/* Consists of 2 addresses			*/
/************************************************/
inline void YM2610_control_port_0_A_w(uint32 offset, uint32 data)
{
    YM2610Write(0, data);
}

inline void YM2610_control_port_0_B_w(uint32 offset, uint32 data)
{
    YM2610Write(2, data);
}

/************************************************/
/* Data Write for YM2610 - Chip 0		*/
/* Consists of 2 addresses			*/
/************************************************/
inline void YM2610_data_port_0_A_w(uint32 offset, uint32 data)
{
    YM2610Write(1, data);
}

inline void YM2610_data_port_0_B_w(uint32 offset, uint32 data)
{
    YM2610Write(3, data);
}

#endif

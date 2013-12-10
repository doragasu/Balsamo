/// \todo USE __delay_ms & __delay_us from libpic30.h

#include "common.h"
#include <p30f6014.h>

/************************************************************************//**
 * \brief Software delay
 *
 * \param[in] nCycles Number of cycles to delay.
 ****************************************************************************/
void SwDelayNCyc(long nCycles)
{
	long i;
	for (i = 0; i < nCycles; i++) ClrWdt();
}

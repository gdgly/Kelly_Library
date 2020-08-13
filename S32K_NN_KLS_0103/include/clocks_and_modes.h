#ifndef CLOCKS_AND_MODES_H_
#define CLOCKS_AND_MODES_H_

void disable_WDOG (void);

void Init_WDOG (void);
void WDOG_Feed (void);
void SOSC_init_8MHz (void);
void HSRUNmode_112MHz (void);
void NormalRUNmode_80MHz (void);
void HSRUN_to_RUN(void);


#endif /* CLOCKS_AND_MODES_H_ */


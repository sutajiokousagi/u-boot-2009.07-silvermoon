#ifndef _BU_RTC_H_
#define	_BU_RTC_H_

#define	RTC_PRIORITY	15

extern void RTC_init(void);
extern void RTC_set_raw_time(BU_U32);
extern BU_U32 RTC_get_raw_time(void);
extern void RTC_enable_1hz_int(void);
extern void RTC_enable_alarm_int(void);
extern void RTC_disable_1hz_int(void);
extern void RTC_disable_alarm_int(void);
extern void RTC_set_alarm(BU_U32);
extern void rtc_alarm_to_pmu(int);

#endif

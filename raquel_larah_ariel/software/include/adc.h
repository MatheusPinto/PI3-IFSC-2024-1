#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

#define ADC_MIN 0
#define ADC_MAX ((1 << 10) - 1)
#define ADC_MAP_MIN -120
#define ADC_MAP_MAX 120

void adcInit(void);
void adcEnable(void);
void adcDisable(void);
int16_t adcValue(void);
const char *adcToString();

#endif /* ADC_H_ */
#ifndef I2C_H_
#define I2C_H_


void I2C_Init(unsigned char slave_add);

void I2C_Write(unsigned char address, unsigned char numbyte, unsigned char *poisend);

void I2C_Read(unsigned char address, unsigned char numbyte, unsigned char *poirev);

unsigned char Dec2BCD(unsigned char Dec);
unsigned char BCD2Dec(unsigned char BCD);


#endif

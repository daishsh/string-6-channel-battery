/* 
 * File:   iic.h
 * Author: zwq
 *
 * Created on 2017年8月25日, 上午11:26
 */

#ifndef _IIC_H_
#define	_IIC_H_

void I2C_Delay10us(unsigned char us);
void start(void);
void stop(void);
int r_ack(void);
int wait_r_ack(void);
void s_ack(void);
void sn_ack(void);
void write_byte(unsigned char w_data);
unsigned char read_byte(void);
int cw_read(unsigned char point_reg,unsigned char *r_pdata);
int cw_read_word(unsigned char point_reg,unsigned char *r_pdata, unsigned int length);
int cw_write(unsigned char point_reg,unsigned char *w_pdata);



#endif	/* _IIC_H_ */


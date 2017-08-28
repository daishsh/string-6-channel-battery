#include "mcc_generated_files/mcc.h"

//****************************Micro*******************************/
#define	READ_CW2015				0xc5
#define	WRITE_CW2015			0xc4

#define I2C_DELAY		5

#define SCL0 IO_RC0_SetLow()
#define SCL1 IO_RC0_SetHigh()

#define SDA0 IO_RC1_SetLow()
#define SDA1 IO_RC1_SetHigh()

#define SDA_READ IO_RC1_GetValue()
#define SDA IO_RC1_LAT

#define SDA_IN IO_RC1_SetDigitalInput()
#define SDA_OUT IO_RC1_SetDigitalOutput()

//***************************function*******************************/
                                                                
//==========================�����Ӻ���===============================
void I2C_Delay10us(unsigned char us)
{
	unsigned char a, b;
	unsigned char i;
	for(i = 0; i < us; i++)
    {
        for(b = 1;b > 0;b --)
        {
            for(a = 7;a > 0;a --) ;
        }
	}
}
#if 0
//-------------------------------------------------------------------
void iic_init(void)			//IIC���߳�ʼ������
{
	SDA = 1;				//�ͷ�IIC���ߵ������ߡ�
	SCL = 1;				//�ͷ�IIC���ߵ�ʱ���ߡ�
}
//-------------------------------------------------------------------
#endif
void start(void)			//IIC���߲�����ʼ�źź���  
{
    SDA_OUT;
	SDA1;				//����������
	SCL1;				//����ʱ����
	I2C_Delay10us(I2C_DELAY);
	SDA0;				//��ʱ����Ϊ�ߵ�ƽʱ�����������ߣ�������ʼ�źš�
	I2C_Delay10us(I2C_DELAY);
	SCL0;				//����ʱ����
}
//-------------------------------------------------------------------
void stop(void)				//IIC���߲���ֹͣ�źź���
{
    SDA_OUT;
	SDA0;				//����������
	I2C_Delay10us(I2C_DELAY);
	SCL1;				//����ʱ���ߡ�
	I2C_Delay10us(I2C_DELAY);
	SDA1;				//ʱ��ʱ��Ϊ�ߵ�ƽʱ�����������ߣ�����ֹͣ�źš�
	I2C_Delay10us(I2C_DELAY);
	//	SCL = 0;
	I2C_Delay10us(I2C_DELAY);
}
//-------------------------------------------------------------------
int r_ack(void)				//����Ӧ���źź���
{
    int ack;				//����һ��λ���������ݴ�Ӧ��״̬��
    SDA_IN;
	SCL1;				//����ʱ���ߡ�
	I2C_Delay10us(I2C_DELAY);
	ack = SDA_READ;				//��ȡӦ���źŵ�״̬��
	I2C_Delay10us(I2C_DELAY);
	SCL0;				//����ʱ���ߡ�
	I2C_Delay10us(I2C_DELAY);
	return ack;				//����Ӧ���źŵ�״̬��0��ʾӦ��1��ʾ��Ӧ��
}

int wait_r_ack(void)
{
	int ack;
	unsigned int i;	
	ack = 1;
	for(i = 0; i < 100; i++)
	{
		ack = r_ack();
		if(!ack)
		{
			break;
		}
		if(i == 99)
		{
			stop();
			return 1;
		}
	}
	return 0;
}
//---------------------------------------`----------------------------
void s_ack(void)				//����Ӧ���źź���
{
    SDA_OUT;
	SDA1;				//�ͷ��������ߣ�׼������Ӧ���źš�
	I2C_Delay10us(I2C_DELAY);
	SCL1;				//����ʱ���ߡ�
	I2C_Delay10us(I2C_DELAY);
	SCL0;				//����ʱ���ߡ�
	I2C_Delay10us(I2C_DELAY);

}

void sn_ack(void)				//Master����׼������ָ��
{
    SDA_OUT;
	SDA0;
	I2C_Delay10us(I2C_DELAY);
	SCL1;
	I2C_Delay10us(I2C_DELAY);
	SCL0;
	SDA1;
	I2C_Delay10us(I2C_DELAY);
}

//-------------------------------------------------------------------
void write_byte(unsigned char w_data)		//��IIC����д��һ���ֽڵ����ݺ��� 
{
	unsigned char i;
	SDA_OUT;
    for(i = 0; i < 8; i++)				//��8λ����
	{
		SDA = w_data & 0x80;				//д���λ������
		I2C_Delay10us(I2C_DELAY);
		SCL1;						//����ʱ���ߣ�����д�뵽�豸�С�
		I2C_Delay10us(I2C_DELAY);
		SCL0;						//����ʱ���ߣ�����ı������ߵ�״̬
		I2C_Delay10us(I2C_DELAY);
		w_data = w_data << 1;				//��������һλ���Ѵθ�λ�������λ,Ϊд��θ�λ��׼��
    }
}
//-------------------------------------------------------------------
unsigned char read_byte(void)			//��IIC���߶�ȡһ���ֽڵ����ݺ���
{
	unsigned char i;
	unsigned char r_data;					//����һ������Ĵ�����
	SDA_IN;
    for(i = 0; i < 8; i++)				//��8λ����
	{
		SCL1;						//����ʱ���ߣ�Ϊ��ȡ��һλ������׼����
		I2C_Delay10us(I2C_DELAY);
		r_data = r_data << 1;				//�������ֽڵ���������һλ��׼����ȡ���ݡ�
		I2C_Delay10us(I2C_DELAY);
		if(SDA_READ)							//���������Ϊ��ƽ��ƽ��
		{
			r_data = r_data | 0x1;			//��������ֽڵ����λд1��
		}
		SCL0;						//����ʱ���ߣ�Ϊ��ȡ��һλ������׼����
		I2C_Delay10us(I2C_DELAY);
	}	
	return r_data;						//���ض�ȡ��һ���ֽ����ݡ�
}

//------------------------IIC��д����--------------------------------
int cw_read(unsigned char point_reg,unsigned char *r_pdata)
{
	start();
	write_byte(WRITE_CW2015);
	if(wait_r_ack())
	{
		return 1;
	}
	write_byte(point_reg);
	if(wait_r_ack())
	{
		return 1;
	}
	start();
	write_byte(READ_CW2015);
	if(wait_r_ack())
	{
		return 1;
	}
	*r_pdata = read_byte();
	s_ack();
	stop();
	return 0;
}

int cw_read_word(unsigned char point_reg,unsigned char *r_pdata, unsigned int length)
{
	unsigned char count;

	start();
	write_byte(WRITE_CW2015);
	if(wait_r_ack())
	{
		return 1;
	}
	write_byte(point_reg);
	if(wait_r_ack())
	{
		return 1;
	}
	start();
	write_byte(READ_CW2015);
	if(wait_r_ack())
	{
		return 1;
	}
	for(count = 0; count < length; count++ ){
		*r_pdata = read_byte();
		if(count + 1 < length){
			r_pdata++;
			sn_ack();
		}
	}
	s_ack();
	stop();
	return 0;
}

int cw_write(unsigned char point_reg,unsigned char *w_pdata)
{
	start();
	write_byte(WRITE_CW2015);
	if(wait_r_ack())
	{
		return 1;
	}
	write_byte(point_reg);
	if(wait_r_ack())
	{
		return 1;
	}
	write_byte(*w_pdata);
	if(wait_r_ack())
	{
		return 1;
	}
	stop();
	return 0;
}
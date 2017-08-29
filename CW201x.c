#include "CW201x.h"
#include "iic.h"

unsigned char CHARGE = 0; //�Ƿ�ӳ������־λ��1Ϊ�ӳ������0Ϊ�γ������
//int no_charger_full_jump =0;
//unsigned int allow_no_charger_full =0;
//unsigned int allow_charger_always_zero =0;
//unsigned char if_quickstart =0;
unsigned char reset_loop =0;

/*����һ��ȫ�ֱ������ⲿ�ļ�Ҫʹ��ʱ����include Cellwise CW201x Driver for MCU.h�ļ�������extern����cw_bat*/
STRUCT_CW_BATTERY   cw_bat;

////////////////////////////////////////////////////////////////////////////////////
////global function: 'cw_bat_work()'  and  'cw_bat_init()'                      ////
////'cw_bat_work()'need be called by main.c in every second                     ////
////'cw_bat_init()'need be called by main.c in system-init after power on reset ////
////////////////////////////////////////////////////////////////////////////////////

//void delay_us(unsigned char us);	

/**********************************************************************************/
/***************CW_Delay10ms() �� CW_Delay10us() �������Ҹ���51��******************/
/***************Ƭ������д���ӳٺ�����������Լ���ƽ̨�����滻*********************/
/**********************************************************************************/
void CW_Delay10ms(unsigned int c) 
{
    unsigned char a, b, d;
    for (;c>0;c--)
	{
        for(b=95;b>0;b--)
        {
            for(a=209;a>0;a--) ;
        }
	}       
}

void CW_Delay100us()
{
	unsigned char a, b;
	unsigned char i;
	for(i = 0; i < 10; i++)
    {
        for(b = 1;b > 0;b --)
        {
            for(a = 7;a > 0;a --);
        }
	}
}

/*��������������Ǹ���ic�ڵĵ��profile��Ϣ��һ��ֻ����ic VDD��������ϵ�ʱ��ִ�� 
return 1 : i2c��д�� return 2 : оƬ����sleepģʽ return 3 : д���profile��Ϣ������������еĲ�һ��*/
unsigned char cw_update_config_info(void)
{
	int ret = 0;
	unsigned char i;
	unsigned char reset_val;
	unsigned char reg_val;
    const unsigned char cw_bat_config_info[SIZE_BATINFO] = {
    0x14,0xA6,0x70,0x6C,0x6C,0x67,0x64,
    0x62,0x60,0x5C,0x59,0x4E,0x50,0x42,
    0x2E,0x26,0x25,0x24,0x27,0x2E,0x41,
    0x5A,0x6D,0x4C,0x39,0xD7,0x06,0x66,
    0x00,0x01,0x05,0x34,0x61,0x76,0x79,
    0x7D,0x42,0x1E,0xA4,0x00,0x27,0x5A,
    0x52,0x87,0x8F,0x91,0x94,0x52,0x82,
    0x8C,0x92,0x96,0x2B,0x59,0x6E,0xCB,
    0x2F,0x7D,0x72,0xA5,0xB5,0xC1,0x94,
    0x99
    };
    
	/* make sure no in sleep mode */
	ret = cw_read(REG_MODE, &reg_val);
	if(ret)
	{
		return 1;
	}
	if((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP)
	{
		return 2;
	}
	/* update new battery info */
	for(i = 0; i < SIZE_BATINFO; i++)
	{
		reg_val = cw_bat_config_info[i];
		ret = cw_write(REG_BATINFO+i, &reg_val);
		if(ret)
		{
			return 1;
		}
	}

	/* readback & check */
	for(i = 0; i < SIZE_BATINFO; i++)
	{
		ret = cw_read(REG_BATINFO+i, &reg_val);
		if(ret)
		{
			return 1;
		}
		if(reg_val != cw_bat_config_info[i])
		{
			return 3;
		}
	}
	/* set cw2015/cw2013 to use new battery info */
	ret = cw_read(REG_CONFIG, &reg_val);
	if(ret)
	{
		return 1;
	}
	reg_val |= CONFIG_UPDATE_FLG;   /* set UPDATE_FLAG */
	reg_val &= 0x07;                /* clear ATHD */
	reg_val |= ATHD;                /* set ATHD */
	ret = cw_write(REG_CONFIG, &reg_val);
	if(ret)
	{
		return 1;
	}
	/* reset */
	reset_val = MODE_NORMAL;
	reg_val = MODE_RESTART;
	ret = cw_write(REG_MODE, &reg_val);
	if(ret)
	{
		return 1;
	}
	CW_Delay100us();  //delay 100us      
	ret = cw_write(REG_MODE, &reset_val);
	if(ret)
	{
		return 1;
	}   
	return 0;
}

/*�����Ƴ�ʼ������ ÿ�ο�����Ҫִ��
return 1 : i2c��д�� return 2 : оƬ����sleepģʽ return 3 : д���profile��Ϣ������������еĲ�һ�� return 4 : оƬ������30s�ڶ�����ֵһֱ�쳣*/
unsigned char cw_init(void)
{
	unsigned ret;
	unsigned char i;
	unsigned char reg_val = MODE_NORMAL;
    const unsigned char cw_bat_config_info[SIZE_BATINFO] = {
    0x14,0xA6,0x70,0x6C,0x6C,0x67,0x64,
    0x62,0x60,0x5C,0x59,0x4E,0x50,0x42,
    0x2E,0x26,0x25,0x24,0x27,0x2E,0x41,
    0x5A,0x6D,0x4C,0x39,0xD7,0x06,0x66,
    0x00,0x01,0x05,0x34,0x61,0x76,0x79,
    0x7D,0x42,0x1E,0xA4,0x00,0x27,0x5A,
    0x52,0x87,0x8F,0x91,0x94,0x52,0x82,
    0x8C,0x92,0x96,0x2B,0x59,0x6E,0xCB,
    0x2F,0x7D,0x72,0xA5,0xB5,0xC1,0x94,
    0x99
    };

	/* wake up cw2015/13 from sleep mode */
	ret = cw_write(REG_MODE, &reg_val);
	if(ret)
	{
		return 1;
	}

	/* check ATHD if not right */
	ret = cw_read(REG_CONFIG, &reg_val);
	if(ret)
	{
		return 1;
	}
	if((reg_val & 0xf8) != ATHD)
	{
		//"the new ATHD need set"
		reg_val &= 0x07;    /* clear ATHD */
		reg_val |= ATHD;    /* set ATHD */
		ret = cw_write(REG_CONFIG, &reg_val);
		if(ret)
		{
			return 1;
		}
	}
	
	/* check config_update_flag if not right */
	ret = cw_read(REG_CONFIG, &reg_val);
	if(ret)
	{
		return 1;
	}
	if(!(reg_val & CONFIG_UPDATE_FLG))
	{
		//"update flag for new battery info need set"
		ret = cw_update_config_info();
		if(ret)
		{
			return ret;
		}
	}
	else
	{
		for(i = 0; i < SIZE_BATINFO; i++)
		{ 
			ret = cw_read(REG_BATINFO +i, &reg_val);
			if(ret)
			{
				return 1;
			}
			if(cw_bat_config_info[i] != reg_val)
			{
				break;
			}
		}
		if(i != SIZE_BATINFO)
		{
			//"update flag for new battery info need set"
			ret = cw_update_config_info();
			if(ret)
			{
				return ret;
			}
		}
	}
	/* check SOC if not eqaul 255 */
	for (i = 0; i < 30; i++) {
		CW_Delay10ms(10);//delay 100ms
		ret = cw_read(REG_SOC, &reg_val);
		if (ret)
			return 1;
		else if (reg_val <= 100) 
			break;		
    }
	
    if (i >=30){
        reg_val = MODE_SLEEP;
        ret = cw_write(REG_MODE, &reg_val);
        // "cw2015/cw2013 input unvalid power error_2\n";
        return 4;
    } 
	return 0;
}

#ifdef BAT_LOW_INTERRUPT
/*��һ��alrt �¼�����ʱ��cw2015 ic������arlt pin�����жϣ���ʱ��Ҫ��06�Ĵ��������bitλ��0��������cw2015 ic�ͷ�alrt pin ���溯�����������ͷ�alrt pin*/
unsigned char cw_release_alrt_pin(void)
{
        signed char ret = 0;
        unsigned int reg_val;
        unsigned char alrt;
        
		
        ret = cw_read(REG_RRT_ALERT, &reg_val);
        if (ret) {
            return -1;
		}
        alrt = reg_val & 0x80;
        
        reg_val = reg_val & 0x7f;
        ret = cw_write(REG_RRT_ALERT, &reg_val);
        if(ret) {
            return -1;
        }
        
        return alrt;
}

/*����ĺ�������д��һ�����ӣ����Ӻ����������Ǹ����µĵ͵�澯ֵΪ�ϴε� -1�� �������ǵĴ��뿪ʼ��ʱ���趨�ĵ͵�澯ֵ��10���ǵ���������10��*/
/*���ش������жϺ��Ұ��µĵ͵�澯ֵ9д���˶�Ӧ�ļĴ����С� ATHD��08�Ĵ�����ǰ5��bit*/
bit cw_update_athd(){
	bit ret = 0;
	unsigned char reg_val;
	char new_athd = 0;
	
	ret = cw_read(REG_CONFIG, &reg_val);
	if(ret)
	{
		return -1;
	}
	new_athd = (reg_val >> 3) - 1;
	if(new_athd <= 0){
		new_athd = 0;
	}
	new_athd = new_athd << 3;

	//"the new ATHD need set"
	reg_val &= 0x07;    /* clear ATHD */
	reg_val |= new_athd;    /* set new ATHD */
	ret = cw_write(REG_CONFIG, &reg_val);
	if(ret)
	{
			return -1;
	}
	return 0;
}

static void ALRT_ISR()interrupt 
{
    /*User can do something when alrt */
	/*�ͻ�������������뵱�жϵ���ʱ�����Ĳ���*/
    cw_release_alrt_pin();
	cw_update_athd();
    /*User can write new alrt to CONFIG resiger*/
}
#endif

int cw_por(void)
{
	int ret = 0;
	unsigned char reset_val = 0;
	reset_val = MODE_SLEEP;             
	ret = cw_write(REG_MODE, &reset_val);
	if (ret)
		return -1;
	CW_Delay100us(); //delay 100us
	
	reset_val = MODE_NORMAL;
	ret = cw_write(REG_MODE, &reset_val);
	if (ret)
		return -1;
	CW_Delay100us(); //delay 100us
	
	ret = cw_init();
	if (ret) 
		return ret;
	return 0;
}

int cw_get_capacity(void)
{
	int ret = 0;
	unsigned char allow_capacity;
	unsigned char reg_val;
	//unsigned char reset_val;
	unsigned char cw_capacity;
	//int charge_time;

	ret = cw_read(REG_SOC, &reg_val);
	if(ret)
	{
		return -1;
	}
        
	cw_capacity = reg_val;
	/*����ic�������⣬��ȡ�������ں���ֵ��Χ��5�Σ�����ic������м������ȷ��ֵ����ô5�εļ�������0����ȷ��ʾ*/
	if ((cw_capacity < 0) || (cw_capacity > 100)) {
                // "get cw_capacity error; cw_capacity = %d\n"
        reset_loop++;
		if (reset_loop >5) { 
			ret = cw_por(); //por ic
			if(ret)
				return -1;
			reset_loop =0;               
		}                   
        return cw_bat.capacity;
    }else {
        reset_loop =0;
    }
	
/*
	if(((cw_bat.usb_online == 1) && (cw_capacity == (cw_bat.capacity - 1)))
			|| ((cw_bat.usb_online == 0) && (cw_capacity == (cw_bat.capacity + 1))))
	{
		// modify battery level swing
		if(!((cw_capacity == 0 && cw_bat.capacity <= 2)||(cw_capacity == 100 && cw_bat.capacity == 99)))
		{			
			cw_capacity = cw_bat.capacity;
		}
	}

	if((cw_bat.usb_online == 1) && (cw_capacity >= 95) && (cw_capacity <= cw_bat.capacity) )
	{     
		// avoid not charge full
		allow_no_charger_full++;
		if(allow_no_charger_full >= BATTERY_UP_MAX_CHANGE)
		{
			allow_capacity = cw_bat.capacity + 1; 
			cw_capacity = (allow_capacity <= 100) ? allow_capacity : 100;
			no_charger_full_jump =1;
			allow_no_charger_full =0;
		}
		else if(cw_capacity <= cw_bat.capacity)
		{
			cw_capacity = cw_bat.capacity; 
		}
	}
	
    else if((cw_bat.usb_online == 0) && (cw_capacity <= cw_bat.capacity ) && (cw_capacity >= 90) && (no_charger_full_jump == 1))
	{
		// avoid battery level jump to CW_BAT
		if(cw_bat.usb_online == 0) 
		   allow_no_charger_full++;
		if(allow_no_charger_full >= BATTERY_DOWN_MIN_CHANGE)
		{
			allow_capacity = cw_bat.capacity - 1;
			allow_no_charger_full =0; 
			if (cw_capacity >= allow_capacity)
			{
				no_charger_full_jump =0;
			}
			else
			{
				cw_capacity = (allow_capacity > 0) ? allow_capacity : 0;
			}
		}
		else if(cw_capacity <= cw_bat.capacity)
		{
			cw_capacity = cw_bat.capacity;
		}
	}
	else
    {
  		allow_no_charger_full =0;
    }
	
	
	if((cw_bat.usb_online > 0) && (cw_capacity == 0))
	{		  
		allow_charger_always_zero++;
		if((allow_charger_always_zero >= BATTERY_DOWN_MIN_CHANGE_SLEEP) && (if_quickstart == 0))
		{
            ret = cw_por(); //por ic
			if(ret){
				return -1;
			}
			if_quickstart = 1;
			allow_charger_always_zero =0;
		}
	}
	else if((if_quickstart == 1)&&(cw_bat.usb_online == 0))
	{
		if_quickstart = 0;
	}
*/
	return(cw_capacity);
}

unsigned int cw_get_vol(void)
{
	int ret = 0;
	unsigned char get_ad_times = 0;
	unsigned char reg_val[2] = {0 , 0};
	unsigned long ad_value = 0;
	unsigned int ad_buff = 0;
	unsigned int ad_value_min = 0;
	unsigned int ad_value_max = 0;

	for(get_ad_times = 0; get_ad_times < 3; get_ad_times++)
	{
		ret = cw_read_word(REG_VCELL, &reg_val[0],2);
		if(ret)
		{
			return 1;
		}
		ad_buff = (reg_val[0] << 8) + reg_val[1];

		if(get_ad_times == 0)
		{
			ad_value_min = ad_buff;
			ad_value_max = ad_buff;
		}
		if(ad_buff < ad_value_min)
		{
			ad_value_min = ad_buff;
		}
		if(ad_buff > ad_value_max)
		{
			ad_value_max = ad_buff;
		}
		ad_value += ad_buff;
	}
	ad_value -= ad_value_min;
	ad_value -= ad_value_max;
	ad_value = ad_value  * 305 / 100000;
	return(ad_value);       //14λADCת��ֵ
}

#ifdef CW2015_GET_RRT
int cw_get_time_to_empty(void)
{
        signed char ret;
        unsigned char reg_val;
        unsigned int value16;

        ret = cw_read(REG_RRT_ALERT, &reg_val);
        if (ret)
                return -1;

        value16 = (unsigned int)reg_val;

        ret = cw_read(REG_RRT_ALERT + 1, &reg_val);
        if (ret)
                return -1;

        value16 = ((value16 << 8) + reg_val) & 0x1fff;
       
        return value16;
}
#endif

void update_capacity(void)
{
	int cw_capacity;
	cw_capacity = cw_get_capacity();
	if((cw_capacity >= 0) && (cw_capacity <= 100) && (cw_bat.capacity != cw_capacity))
	{       
		cw_bat.capacity = cw_capacity;
	}
}


void update_vol(void)
{
	unsigned int cw_voltage;
	cw_voltage = cw_get_vol();
	if(cw_voltage == 1){
		//read voltage error
		cw_bat.voltage = cw_bat.voltage;
	}else if(cw_bat.voltage != cw_voltage)
	{
		cw_bat.voltage = cw_voltage;
	}
}

#ifdef CW2015_GET_RRT
static void update_time_to_empty(void)
{
	unsigned int rrt;
	rrt = (unsigned int)cw_get_time_to_empty();
	if((rrt >= 0) && (cw_bat.time_to_empty != rrt))
	{
		cw_bat.time_to_empty = rrt;
	}
}
#endif
/*
static void update_alt(void)
{
	signed int alt;
	alt = cw_get_alt();
	if ((rrt >= 0) && (cw_bat.alt != alt))
	{
		cw_bat.alt = (unsigned int)alt;
	}       
}
*/

void update_usb_online(void)
{
	if(CHARGE == 1) 
	{
		cw_bat.usb_online = 1;
	}else{
		cw_bat.usb_online = 0;
	}
}

////////////////////////////////////////MCUһ�����һ��//////////////////////////////////////////
void cw_bat_work(void)
{
	update_usb_online();
	update_capacity();
	update_vol();
#ifdef CW2015_GET_RRT
	update_time_to_empty();
#endif
}

/*
static void cw_bat_gpio_init(void)
{
     
     usb_det_pin -- init
     alt_pin  -- init
 
     return 0;
}
*/

///////////////////////////////////////MCU������ʼ��ʱ����.//////////////////////////////////////
unsigned char cw_bat_init(void)
{
	unsigned char ret;
	unsigned char loop = 0;
	//cw_bat_gpio_init();
	
	ret = cw_init();
	while((loop++ < 200) && (ret != 0))
	{
		ret = cw_init();
	}
	
	cw_bat.usb_online = 0;
	cw_bat.capacity = 2;
	cw_bat.voltage = 0;
#ifdef CW2015_GET_RRT
	cw_bat.time_to_empty = 0;
#endif
	cw_bat.alt = 0;
	
	return ret;	
}





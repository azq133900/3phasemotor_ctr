#include "sys.h"
#include "delay.h"
#include "usart.h"	  
#include "pwm.h"

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���         

#define IDt 'A'
#define IDr a
u8 cnum=0;
u8 dectr[5];
u16 speed=0;
u16 power=500;
u16 out_speed=0;
int StSpe=700;
//u8 ID='A';
u16 time2run=100;

struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 

void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}
//void validspeed(u8 dir,u16 speed)
//{
//	if(!dir)
//	{
//		power=(u16)(0.2546f*speed+631.4f);
//	  setspeed(power);
//	}
//	else if(dir==1)
//	{
//		if(speed<550)
//				{
//					power=(u16)(231.9f-0.2573f*speed);
//					setspeed(power);
//				}
//				else
//				{
//					power=(u16)(122.5f-0.05978f*speed);
//					setspeed(power);
//				}
//	}
//}
void USART1_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
		{
		Res =USART_ReceiveData(USART1);	
		if ( Res==IDt)
		{
			cnum=1;
			dectr[0]=Res;
		}
		else if(cnum>0)
		{
			if ( ( (Res>='0')&& (Res<='9') )  )
			{dectr[cnum]=Res;
			cnum++;
			}
			else
			cnum=0;
		}
		
		if(cnum==5)
		{
			cnum=0;
			if(dectr[1]=='0'||(dectr[1]=='1'||dectr[1]=='2'))
			{
				speed=(u16)((dectr[2]-'0')*100+(dectr[3]-'0')*10+(dectr[4]-'0'));
				setspeed(dectr[1]-'0',speed);
				//set_stepmotor(dectr[1]-'0');
				if(dectr[1]=='2')
				{
				set_stepmotor(2);
				}
				else
				{
					set_stepmotor(Dir_of_Stepmptor);
				}
				
				
			}
			else if(dectr[1]=='3'||dectr[1]=='4')
			{
				time2run=(u16)((dectr[3]-'0')*10+(dectr[4]-'0'));
				StSpe=(dectr[2]-'0')*100;
				while(StSpe>0)
				{
					setspeed(dectr[1]-'3',StSpe);
					delay_ms(time2run);
					StSpe=StSpe-20;
				}
				setspeed(2,1);
			}
			else if(dectr[1]=='4')
			{
				out_speed=Get_Adc_Average();
				printf("a%d\r\n",out_speed);
			}
		}

		}
		
} 



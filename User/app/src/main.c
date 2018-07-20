/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ���̵�������SEGGER��RTT�����������ڵ���
*	�޸ļ�¼ : 
*		�汾��    ����        ����     ˵��
*		V1.0    2018-04-12   armfly    �׷�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* �ײ�Ӳ������ */
#include "bsp_rtt.h"



/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint32_t i = 0;
	int GetKey;

	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ� startup_stm32f4xx.s ����� system_stm32f4xx.c �е� void SystemInit(void)��
		SystemInit()����������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����

		������STM32-V5��������������25MHz, �ڲ�PLL��Ƶ��168MHz�������Ҫ������Ƶ�������޸�������ļ���
		\User\bsp_stm32f4xx\system_stm32f4xx.c
		�ļ���ͷ�ļ�������PLL��Ƶ�������޸���Щ��Ϳ����޸���Ƶ���������Ӳ����
	*/

	bsp_Init();		/* Ӳ����ʼ�� */

	bsp_StartAutoTimer(0, 100);	/* ����1��100ms���Զ���װ�Ķ�ʱ�� */
	
	bsp_RTT_Init();

	/* ����������ѭ���� */
	while (1)
	{
		bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */

		/* �ж϶�ʱ����ʱʱ�� */
		if (bsp_CheckTimer(0))	
		{
			bsp_LedToggle(4);	
			SEGGER_RTT_printf(0, "test rtt_printf");
		}
		
		/* ��һ���򵥵Ļػ����� */
		if (SEGGER_RTT_HasKey()) 
		{
			GetKey = SEGGER_RTT_GetKey();
			SEGGER_RTT_SetTerminal(0);
			SEGGER_RTT_printf(0, "SEGGER_RTT_GetKey = %c\r\n", GetKey);
		}

		/* �����˲��ͼ���ɺ�̨systick�жϷ������ʵ�֣�����ֻ��Ҫ����bsp_GetKey��ȡ��ֵ���ɡ� */
		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:			/* K1������ */
					SEGGER_RTT_SetTerminal(1);
					SEGGER_RTT_WriteString(0,
										   RTT_CTRL_RESET"Red: " \
										   RTT_CTRL_TEXT_RED"This text is red. " \
										   RTT_CTRL_TEXT_BLACK"" \
										   RTT_CTRL_BG_BRIGHT_GREEN"This background is green. " \
										   RTT_CTRL_RESET"Normal text again.\r\n");
				
					break;

				case KEY_DOWN_K2:			/* K2������ */
					SEGGER_RTT_SetTerminal(2);
					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN"KEY_DOWN_K2, ArmFly Real-Time-Terminal Sample\r\n");
					break;

				case KEY_DOWN_K3:			/* K3������ */
					SEGGER_RTT_SetTerminal(3);
					SEGGER_RTT_printf(0, "KEY_DOWN_K3, i = %d\r\n", i++);
					break;

				default:
					/* �����ļ�ֵ������ */
					break;
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

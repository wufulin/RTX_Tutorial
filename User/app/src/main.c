/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 工程调试利器SEGGER的RTT组件，替代串口调试
*	修改记录 : 
*		版本号    日期        作者     说明
*		V1.0    2018-04-12   armfly    首发
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* 底层硬件驱动 */
#include "bsp_rtt.h"



/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;		/* 按键代码 */
	uint32_t i = 0;
	int GetKey;

	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件 startup_stm32f4xx.s 会调用 system_stm32f4xx.c 中的 void SystemInit(void)。
		SystemInit()函数配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		安富莱STM32-V5开发板主晶振是25MHz, 内部PLL倍频到168MHz。如果需要更改主频，可以修改下面的文件：
		\User\bsp_stm32f4xx\system_stm32f4xx.c
		文件开头的几个宏是PLL倍频参数，修改这些宏就可以修改主频，无需更改硬件。
	*/

	bsp_Init();		/* 硬件初始化 */

	bsp_StartAutoTimer(0, 100);	/* 启动1个100ms的自动重装的定时器 */
	
	bsp_RTT_Init();

	/* 进入主程序循环体 */
	while (1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */

		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			bsp_LedToggle(4);	
			SEGGER_RTT_printf(0, "test rtt_printf");
		}
		
		/* 做一个简单的回环功能 */
		if (SEGGER_RTT_HasKey()) 
		{
			GetKey = SEGGER_RTT_GetKey();
			SEGGER_RTT_SetTerminal(0);
			SEGGER_RTT_printf(0, "SEGGER_RTT_GetKey = %c\r\n", GetKey);
		}

		/* 按键滤波和检测由后台systick中断服务程序实现，我们只需要调用bsp_GetKey读取键值即可。 */
		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:			/* K1键按下 */
					SEGGER_RTT_SetTerminal(1);
					SEGGER_RTT_WriteString(0,
										   RTT_CTRL_RESET"Red: " \
										   RTT_CTRL_TEXT_RED"This text is red. " \
										   RTT_CTRL_TEXT_BLACK"" \
										   RTT_CTRL_BG_BRIGHT_GREEN"This background is green. " \
										   RTT_CTRL_RESET"Normal text again.\r\n");
				
					break;

				case KEY_DOWN_K2:			/* K2键按下 */
					SEGGER_RTT_SetTerminal(2);
					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN"KEY_DOWN_K2, ArmFly Real-Time-Terminal Sample\r\n");
					break;

				case KEY_DOWN_K3:			/* K3键按下 */
					SEGGER_RTT_SetTerminal(3);
					SEGGER_RTT_printf(0, "KEY_DOWN_K3, i = %d\r\n", i++);
					break;

				default:
					/* 其它的键值不处理 */
					break;
			}
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

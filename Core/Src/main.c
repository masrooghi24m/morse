/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "..\..\..\required\display.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t InTime = 0,OutTime = 0, Time = 0, now = 0;
uint8_t pressed = 0, i = 0, flag = 0, correct= 0;
char Value[5] = {0,0,0,0,0};
char alphabet;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef struct {
    char* morse;
    char letter;
} MorseStruct;

const MorseStruct Dictionary[] = {
    { ".-", 'A' }, { "-...", 'B' }, { "-.-.", 'C' }, { "-..", 'D' },
    { ".", 'E' }, { "..-.", 'F' }, { "--.", 'G' }, { "....", 'H' },
    { "..", 'I' }, { ".---", 'J' }, { "-.-", 'K' }, { ".-..", 'L' },
    { "--", 'M' }, { "-.", 'N' }, { "---", 'O' }, { ".--.", 'P' },
    { "--.-", 'Q' }, { ".-.", 'R' }, { "...", 'S' }, { "-", 'T' },
    { "..-", 'U' }, { "...-", 'V' }, { ".--", 'W' }, { "-..-", 'X' },
    { "-.--", 'Y' }, { "--..", 'Z' },
    { "-----", '0' }, { ".----", '1' }, { "..---", '2' }, { "...--", '3' },
    { "....-", '4' }, { ".....", '5' }, { "-....", '6' }, { "--...", '7' },
    { "---..", '8' }, { "----.", '9' },
};

char Translate(char* Code) {
    for (int j = 0; j < sizeof(Dictionary) / sizeof(MorseStruct); j++) 
	  {
			if (strcmp(Code , Dictionary[j].morse) == 0)
				{
					  HAL_GPIO_WritePin(ld5_GPIO_Port, ld5_Pin, GPIO_PIN_SET);
            return Dictionary[j].letter;
						correct = 1;
        }
    }
		if(correct != 1) {return '?';		correct = 0; }
}
/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_SYSTICK_Callback(void)		{DisplayRun();}
void EndLetter (void)
{
	i = 0;
	alphabet = Translate(Value);
	uint8_t alphabet_char = (uint8_t) alphabet;
	HAL_UART_Transmit(&huart2, &alphabet_char , 1 , 100);
	HAL_GPIO_TogglePin(ld6_GPIO_Port, ld6_Pin);
	for (int k=0 ;k<5 ;k++)
	{
		Value[k] = 0 ;
	}
}
void Dot(void)
{
	
	Value[i++] = '.';
	if (i == 6) EndLetter(); 

}

void Dash(void) 
{
	Value[i++] = '-';
	if (i == 6) EndLetter(); 
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		DisplayNumber(pressed);
		// Read the button state
        if (HAL_GPIO_ReadPin(sw2_GPIO_Port, sw2_Pin) == GPIO_PIN_RESET)
				{		
						HAL_Delay(50);  //debounce
						if (HAL_GPIO_ReadPin(sw2_GPIO_Port, sw2_Pin) == GPIO_PIN_RESET) 
						{
							if (!pressed)
								{
									InTime = HAL_GetTick();
									flag = 0;
									pressed = 1;
								}
						}
        }
				else
   			{
						HAL_Delay(50); //debounce
						if (HAL_GPIO_ReadPin(sw2_GPIO_Port, sw2_Pin) == GPIO_PIN_SET)
						{
							if (pressed)
								{
									OutTime = HAL_GetTick(); 
									Time = OutTime - InTime;

									if (Time > 1000) 			Dash();
									else 								  Dot();
								 
									pressed = 0;
									flag = 1;
									HAL_Delay(1000); // Wait before detecting the next press
								}
						}
       }
				if (flag ==  1) 
				{
					now = HAL_GetTick();
					if (now - OutTime >= 3000) {EndLetter();	flag = 0;}

				} 
					/* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

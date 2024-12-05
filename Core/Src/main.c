/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "snakeRunc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define INIT 0
#define PLAYING 1
#define GAMEOVER 2
#define L_MARGIN 20
#define T_MARGIN 20
#define R_MARGIN 20
//#define STARTPOINTX 	20
//#define STARTPOINTY		80
//#define ENDPOINTX 		220
//#define ENDPOINTY		250
int system_status = INIT;
//int height=ENDPOINTY- STARTPOINTY;
//int width=ENDPOINTX- STARTPOINTX;
//char snake[10]="O";
//int counterSnake=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void fsm();
void displayPlayingArea();
void displaySnake();
void displayFruit();
bool eatCheck();
bool headCheck();
void snakeRun();
uint8_t isButtonClear();
uint8_t isButtonStart();
uint8_t isButtonUp();
uint8_t isButtonDown();
uint8_t isButtonLeft();
uint8_t isButtonRight();
//uint8_t snakeStartX= 0;
//uint8_t snakeStartY= 0;
uint8_t fruit_x = 0;
uint8_t fruit_y = 0;
int isEat = 0;
int gameOver = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_snakeRunC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  system_init();
  /* USER CODE END 2 */
  srand(time(NULL));
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //touch_Adjust();
  lcd_Clear(BLACK);
  //lcd_Fill(10, 260, 10, 280, GBLUE);
 while (1)
  {
	  //scan touch screen
	  touch_Scan();
	  //lcd_Fill(10, 10, 200, 200, RED);
	  //check if touch screen is touched
	  // 50ms task
	  if(flag_timer2 == 1){
		  flag_timer2 = 0;
		  fsm();
		  snakeRun();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  timer_init();
	  button_init();
	  lcd_init();
	  touch_init();
	  setTimer2(10);
}

uint8_t count_led_debug = 0;
void test_LedDebug(){
	count_led_debug = (count_led_debug + 1) % 20;
	if (count_led_debug == 0) {
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

// para

enum directions {
	up,
	down,
	left,
	right
};
enum directions direction = right;

uint8_t head_x = 0, head_y = 0;
uint8_t tail_x = 0, tail_y = 0;

uint8_t arr[20][20] = {0};

// end


uint8_t count_move = 0;
void snakeRun() {
	count_move = (count_move + 1) % (20/2); // 0.5s
	if (count_move == 0) {
		switch (direction) {
			case up:
				arr[head_x][head_y] = 1;
				head_y -= 1;
				headCheck();
				if (arr[head_x][head_y] > 0) {
					system_status = GAMEOVER;
				}
				arr[head_x][head_y] = 1;
				break;
			case down:
				arr[head_x][head_y] = 2;
				head_y += 1;
				headCheck();
				if (arr[head_x][head_y] > 0) {
					system_status = GAMEOVER;
				}
				arr[head_x][head_y] = 2;
				break;
			case left:
				arr[head_x][head_y] = 3;
				head_x -= 1;
				headCheck();
				if (arr[head_x][head_y] > 0) {
					system_status = GAMEOVER;
				}
				arr[head_x][head_y] = 3;
				break;
			case right:
				arr[head_x][head_y] = 4;
				head_x += 1;
				headCheck();
				if (arr[head_x][head_y] > 0) {
					system_status = GAMEOVER;
				}
				arr[head_x][head_y] = 4;
				break;
			default:
				break;
		}

		if (!eatCheck()) { // if dont eat, del tail
			// tail_x tail_y
			switch (arr[tail_x][tail_y]) {
				case 1:
					tail_y -= 1;
					arr[tail_x][tail_y+1] = 0;
					break;
				case 2:
					tail_y += 1;
					arr[tail_x][tail_y-1] = 0;
					break;
				case 3:
					tail_x -= 1;
					arr[tail_x+1][tail_y] = 0;
					break;
				case 4:
					tail_x += 1;
					arr[tail_x-1][tail_y] = 0;
					break;
				default:
					break;
			}
		}
		//displaySnake();
	}
}

uint8_t isButtonClear(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 && touch_GetY() > 10 && touch_GetY() < 60;
}
uint8_t isButtonStart(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 && touch_GetY() > 10 && touch_GetY() < 60;
}
uint8_t isButtonUp(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 15 && touch_GetX() < 60 && touch_GetY() > 260 && touch_GetY() < 340;
}
uint8_t isButtonDown(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 70 && touch_GetX() < 115 && touch_GetY() > 260 && touch_GetY() < 340;
}
uint8_t isButtonLeft(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 125 && touch_GetX() < 170 && touch_GetY() > 260 && touch_GetY() < 340;
}
uint8_t isButtonRight(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 180 && touch_GetX() < 225 && touch_GetY() > 260 && touch_GetY() < 340;
}




void fsm(){
	switch (system_status) {
		case INIT:
			// init para
			head_x = 0;
			head_y = 0;
			tail_x = 0;
			tail_y = 0;
			direction = right;
			for (int i = 0; i < 20; i++) {
				for (int j = 0; j < 20; j++) {
					arr[i][j] = 0;
				}
			}
			arr[0][0] = 4;

			// end

			// show start button
			lcd_Fill(60, 10, 180, 60, GBLUE);
			lcd_ShowStr(90, 20, "Start", YELLOW, BLACK, 24, 1);

			//
			lcd_ShowStr(20,90,"Welcome to snake", GREEN, BLACK, 24,1);
			lcd_ShowStr(20,130,"game, press start to", GREEN, BLACK, 24,1);
			lcd_ShowStr(20,170,"begin the game", GREEN, BLACK, 24,1);
			if(isButtonStart()){
				lcd_Clear(BLACK);
				system_status = PLAYING;
			}
			break;

		case PLAYING:
			displayPlayingArea();
			displayFruit();
			displaySnake();

			if(isButtonUp()){
				if (direction != up && direction != down) {
					direction = up;
				}
//				logicCheck();
				displaySnake();
			}
			if(isButtonDown()){
				if (direction != up && direction != down) {
					direction = down;
				}
//				logicCheck();
				displaySnake();
			}
			if(isButtonLeft()){
				if (direction != left && direction != right) {
					direction = left;
				}
//				logicCheck();
				displaySnake();
			}
			if(isButtonRight()){
				if (direction != left && direction != right) {
					direction = right;
				}
//				logicCheck();
				displaySnake();
			}
			break;

		case GAMEOVER:
			lcd_Clear(BLACK);
			lcd_ShowStr(80, 20, "GAME OVER", RED, BLACK, 24, 1);

			delay_us(2000);
			system_status = INIT;
			break;

		default:
			break;
	}
}
void displayFruit(){
//	 lcd_ShowStr(fruit_x, fruit_y, "X", BLACK, RED, 24, 1);
	lcd_DrawRectangle(L_MARGIN+fruit_x*10 + 2, T_MARGIN+fruit_y*10 + 2, L_MARGIN+fruit_x*10 + 8, T_MARGIN+fruit_y*10 + 8, RED);
}

bool eatCheck(){
	if (head_x == fruit_x && head_y == fruit_y) { // Eat
		fruit_x = rand()%20;
		fruit_y = rand()%20;
		//system_status=PLAYING;//?
		return 1;
	} else {
		return 0;
	}
}

bool headCheck() {
	if (head_x<0 || head_x>19 || head_y<0 || head_y>19) { // check
//		gameOver=1;
		system_status=GAMEOVER;
		return 1;
	}
	return 0;
}



void displayPlayingArea(){
	// game area 200*200p

	// 20_200_20
	lcd_Fill(L_MARGIN, T_MARGIN, L_MARGIN+200, R_MARGIN+200, GREEN);

	//Display button
	// button up
	lcd_Fill(15, 260, 60, 340, GBLUE);
	lcd_ShowStr(20, 280, "U", YELLOW, BLACK, 24, 1);
	// button down
	lcd_Fill(70, 260, 115, 340, GBLUE);
	lcd_ShowStr(75, 280, "D", YELLOW, BLACK, 24, 1);
	// button left
	lcd_Fill(125, 260, 170, 340, GBLUE);
	lcd_ShowStr(130, 280, "L", YELLOW, BLACK, 24, 1);
	// button right
	lcd_Fill(180, 260, 225, 340, GBLUE);
	lcd_ShowStr(185, 280, "R", YELLOW, BLACK, 24, 1);
}
void displaySnake(){
	lcd_ShowSnake(arr);
	//lcd_DrawCircle(snakeStartY, snakeStartY, BLACK, 10, 1);

}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

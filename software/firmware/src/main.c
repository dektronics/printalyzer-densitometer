#include "stm32l0xx_hal.h"

#define LOG_TAG "main"
#include <elog.h>

#include <printf.h>
#include <cmsis_os.h>
#include <machine/endian.h>

#include "board_config.h"
#include "keypad.h"
#include "sensor.h"
#include "display.h"
#include "adc_handler.h"
#include "task_main.h"
#include "task_sensor.h"
#include "app_descriptor.h"

#define APP_ADDRESS 0x08000000UL
#define END_ADDRESS 0x0801FFFBUL
#define APP_SIZE ((uint32_t)(((END_ADDRESS - APP_ADDRESS) + 3UL) / 4UL))

ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;
CRC_HandleTypeDef hcrc;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;

static void system_clock_config(void);
static void usart1_uart_init(void);
static void logger_init(void);
static void gpio_init(void);
static void i2c1_init(void);
static void tim2_init(void);
static void spi1_init(void);
static void dma_init(void);
static void adc_init(void);
static void crc_init(void);
static void usb_init(void);
static void startup_log_messages(void);

void usb_device_task(void *argument);
void cdc_task_func(void *argument);

void error_handler(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void system_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /*
     * Initialize the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_6;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        error_handler();
    }

    /* Initialize the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        error_handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1
        | RCC_PERIPHCLK_USB;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        error_handler();
    }
}

void usart1_uart_init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        error_handler();
    }
}

void logger_init(void)
{
    /* Initialize EasyLogger */
    elog_init();

    /* Set log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    elog_set_text_color_enabled(true);

    /* Start EasyLogger */
    elog_start();
}

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure default GPIO pin output levels for the display */
    HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISP_RES_GPIO_Port, DISP_RES_Pin, GPIO_PIN_RESET);

    /* Configure GPIO pins: BTN4_Pin BTN3_Pin */
    GPIO_InitStruct.Pin = BTN4_Pin | BTN3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pins: BTN2_Pin BTN1_Pin BTN5_Pin */
    GPIO_InitStruct.Pin = BTN2_Pin | BTN1_Pin | BTN5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure GPIO pins: DISP_CS_Pin DISP_DC_Pin */
    GPIO_InitStruct.Pin = DISP_CS_Pin | DISP_DC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure GPIO pin: DISP_RES_Pin */
    GPIO_InitStruct.Pin = DISP_RES_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DISP_RES_GPIO_Port, &GPIO_InitStruct);

    /* Configure unused GPIO pins: PB1 PB4 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure GPIO pin: SENSOR_INT_Pin */
    GPIO_InitStruct.Pin = SENSOR_INT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SENSOR_INT_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init */
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void i2c1_init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00300F38;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        error_handler();
    }

    /* Configure analog filter */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        error_handler();
    }

    /* Configure digital filter */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
        error_handler();
    }
}

void tim2_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 127;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        error_handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        error_handler();
    }

    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        error_handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        error_handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 64;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
        error_handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
        error_handler();
    }

    HAL_TIM_MspPostInit(&htim2);
}

void spi1_init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        error_handler();
    }
}

void dma_init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /*
     * Configure the global features of the ADC
     * (Clock, Resolution, Data Alignment and number of conversions)
     */
    hadc.Instance = ADC1;
    hadc.Init.OversamplingMode = DISABLE;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ContinuousConvMode = ENABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerFrequencyMode = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    if (HAL_ADC_Init(&hadc) != HAL_OK) {
        error_handler();
    }

    /*
     * Configure for the selected ADC regular channel to be converted.
     */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
        error_handler();
    }

    /*
     * Configure for the selected ADC regular channel to be converted.
     */
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
        error_handler();
    }

    if (HAL_ADCEx_EnableVREFINT() != HAL_OK) {
        error_handler();
    }
    if (HAL_ADCEx_EnableVREFINTTempSensor() != HAL_OK) {
        error_handler();
    }
}

void crc_init(void)
{
    hcrc.Instance = CRC;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    if (HAL_CRC_Init(&hcrc) != HAL_OK) {
        error_handler();
    }
}

void usb_init(void)
{
    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USB_IRQn, 3, 0);
}

void startup_log_messages(void)
{
    const app_descriptor_t *app_descriptor = app_descriptor_get();
    uint32_t hal_ver = HAL_GetHalVersion();
    uint8_t hal_ver_code = ((uint8_t)(hal_ver)) & 0x0F;
    uint8_t *uniqueId = (uint8_t*)UID_BASE;

    log_i("\033[0m");
    log_i("---- %s Startup ----", app_descriptor->project_name);
    log_i("HAL Version: %d.%d.%d%c",
        ((uint8_t)(hal_ver >> 24)) & 0x0F,
        ((uint8_t)(hal_ver >> 16)) & 0x0F,
        ((uint8_t)(hal_ver >> 8)) & 0x0F,
        hal_ver_code > 0 ? (char)hal_ver_code : ' ');
    log_i("FreeRTOS: %s", tskKERNEL_VERSION_NUMBER);
    log_i("Device ID: 0x%lX", HAL_GetDEVID());
    log_i("Revision ID: 0x%lX", HAL_GetREVID());
    log_i("SysClock: %ldMHz", HAL_RCC_GetSysClockFreq() / 1000000);
    log_i("Unique ID: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        uniqueId[0], uniqueId[1], uniqueId[2], uniqueId[3], uniqueId[4],
        uniqueId[5], uniqueId[6], uniqueId[7], uniqueId[8], uniqueId[9],
        uniqueId[10], uniqueId[11]);
    log_i("App version: %s", app_descriptor->version);
    log_i("Build date: %s", app_descriptor->build_date);
    log_i("Build describe: %s", app_descriptor->build_describe);
    log_i("Build checksum: %08lX", __bswap32(app_descriptor->crc32));
    log_i("-----------------------");
}

void startup_verify_checksum()
{
    const app_descriptor_t *app_descriptor = app_descriptor_get();
    volatile uint32_t calculated_crc = 0;

    calculated_crc =
        HAL_CRC_Calculate(&hcrc, (uint32_t*)APP_ADDRESS, APP_SIZE);

    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();

    if (app_descriptor->crc32 != calculated_crc) {
        log_e("Checksum invalid: %08lX != %08lX",
            __bswap32(calculated_crc), __bswap32(app_descriptor->crc32));
        error_handler();
    } else {
        log_i("Checksum valid");
    }
}

int main(void)
{
    /*
     * Initialize the HAL, which will reset of all peripherals, initialize
     * the Flash interface and the Systick.
     */
    HAL_Init();

    /* Configure the system clock */
    system_clock_config();

    /* Initialize the debug UART */
    usart1_uart_init();

    /* Initialize the rest of the configured peripherals */
    gpio_init();
    i2c1_init();
    tim2_init();
    spi1_init();
    dma_init();
    adc_init();
    crc_init();
    usb_init();

    /* Initialize the FreeRTOS scheduler */
    osKernelInitialize();

    /*
     * Note: Initialization code from this point forward may call FreeRTOS
     * functions that have a side-effect of disabling all interrupts, including
     * the one necessary for the HAL tick timer to function.
     * Starting the FreeRTOS scheduler at the end of this function should
     * make things return to normal behavior.
     */

    /* Initialize the logger */
    logger_init();

    /* Print the initial startup messages */
    startup_log_messages();
    startup_verify_checksum();

    /* Create the main task */
    task_main_init();

    /* Start scheduler */
    log_i("Starting scheduler");
    osKernelStart();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == SENSOR_INT_Pin) {
        sensor_int_handler();
    } else {
        keypad_int_handler(GPIO_Pin);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    UNUSED(hadc);
    adc_completion_callback();
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        HAL_IncTick();
    }
}

void error_handler(void)
{
    __disable_irq();
    __ASM volatile("BKPT #01");
    while (1) { }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Assert failed: file %s on line %ld", file, line);
}
#endif /* USE_FULL_ASSERT */

#include "stm32l0xx_hal.h"

#include <tusb.h>
#include "board_config.h"
#include "board_api.h"
#include "uf2.h"

IWDG_HandleTypeDef hiwdg;
CRC_HandleTypeDef hcrc;
#ifdef HAL_SPI_MODULE_ENABLED
SPI_HandleTypeDef hspi1;
#endif
#ifdef HAL_UART_MODULE_ENABLED
UART_HandleTypeDef huart1;
#endif

static void system_clock_config(void);
static void iwdg_init(void);
static void usart1_uart_init(void);
static void usart1_uart_deinit(void);
static void gpio_init(void);
static void gpio_deinit(void);
static void spi1_init(void);
static void spi1_deinit(void);
static void crc_init(void);
static void crc_deinit(void);
static void usb_init(void);
static void usb_deinit(void);

static bool check_start_bootloader();
static void start_application();

void error_handler(void);

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
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI
        | RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USB;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        error_handler();
    }
}

void iwdg_init(void)
{
    /*
     * Configure the independent watchdog with a 2 second
     * timeout. This is a generous amount of time, and
     * mostly useful for catching a total system lockup.
     */
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
    hiwdg.Init.Window = 4095;
    hiwdg.Init.Reload = 4095;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        error_handler();
    }
}

void usart1_uart_init(void)
{
#ifdef HAL_UART_MODULE_ENABLED
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
#endif
}

void usart1_uart_deinit(void)
{
#ifdef HAL_UART_MODULE_ENABLED
    HAL_UART_DeInit(&huart1);
#endif
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
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pins: BTN2_Pin BTN1_Pin BTN5_Pin */
    GPIO_InitStruct.Pin = BTN2_Pin | BTN1_Pin | BTN5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
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
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SENSOR_INT_GPIO_Port, &GPIO_InitStruct);
}

void gpio_deinit(void)
{
    /* De-initialize GPIO pins */
    HAL_GPIO_DeInit(GPIOC, BTN4_Pin | BTN3_Pin);
    HAL_GPIO_DeInit(GPIOA, BTN2_Pin | BTN1_Pin | BTN5_Pin);
    HAL_GPIO_DeInit(GPIOA, DISP_CS_Pin | DISP_DC_Pin);
    HAL_GPIO_DeInit(DISP_RES_GPIO_Port, DISP_RES_Pin);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1 | GPIO_PIN_4);
    HAL_GPIO_DeInit(SENSOR_INT_GPIO_Port, SENSOR_INT_Pin);

    /* GPIO Ports Clock Disable */
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
}

void spi1_init(void)
{
#ifdef HAL_SPI_MODULE_ENABLED
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
#endif
}

void spi1_deinit(void)
{
#ifdef HAL_SPI_MODULE_ENABLED
    HAL_SPI_DeInit(&hspi1);
#endif
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

void crc_deinit(void)
{
    HAL_CRC_DeInit(&hcrc);
}

void usb_init(void)
{
    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USB_IRQn, 3, 0);
}

void usb_deinit(void)
{
    /* Peripheral interrupt disable */
    HAL_NVIC_DisableIRQ(USB_IRQn);

    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_DISABLE();
}

bool check_start_bootloader()
{
    BL_LOG_STR("Starting bootloader check\r\n");

    /* Check if the button combination is held down for about a second */
    uint8_t button_counter = 0;
    while (button_counter < 10) {
        if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_SET
            && HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) == GPIO_PIN_SET
            && HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) == GPIO_PIN_RESET) {
            HAL_Delay(50);
            button_counter++;
        } else {
            BL_LOG_STR("Falling through button check\r\n");
            break;
        }
        HAL_IWDG_Refresh(&hiwdg);
    }
    if (button_counter >= 10) {
        BL_LOG_STR("Buttons held, going to bootloader\r\n");
        return true;
    }

    /* Check if valid application code is in flash */
    if (!board_app_valid()) {
        BL_LOG_STR("App is not valid, going to bootloader\r\n");
        return true;
    } else {
        BL_LOG_STR("App is valid\r\n");
        return false;
    }
}

void start_application()
{
    /* De-initialize all the peripherals */
    usb_deinit();
    spi1_deinit();
    crc_deinit();
    gpio_deinit();
    usart1_uart_deinit();

    /* Save startup data into RTC backup registers */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    RTC->BKP0R = (RCC->CSR & 0xFF000000UL); /* Reset flags */
    HAL_PWR_DisableBkUpAccess();
    __HAL_RCC_PWR_CLK_DISABLE();

    /* Start the application */
    board_app_jump();
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

    /* Initialize the watchdog */
    iwdg_init();

    /* Initialize the debug UART */
    usart1_uart_init();

    /* Initialize the minimum peripherals to detect startup mode */
    gpio_init();
    crc_init();

    HAL_IWDG_Refresh(&hiwdg);

    BL_LOG_STR("---- Densitometer Bootloader ----\r\n");

    if (!check_start_bootloader()) {
        BL_LOG_STR("Starting application\r\n");
        HAL_IWDG_Refresh(&hiwdg);
        start_application();
        while(1) { }
    }

    BL_LOG_STR("Initializing bootloader\r\n");

    HAL_IWDG_Refresh(&hiwdg);

    /* Initialize additional peripherals needed for the bootloader */
    spi1_init();
    usb_init();

    /* Initialize UF2 components */
    uf2_init();

    /* Initialize the TinyUSB stack */
    tusb_init();

    HAL_IWDG_Refresh(&hiwdg);

    /* Initialize the display */
    board_display_init();
    indicator_set(STATE_BOOTLOADER_STARTED);

    HAL_IWDG_Refresh(&hiwdg);

    /* Main loop */
    while (1) {
        tud_task();
        HAL_IWDG_Refresh(&hiwdg);
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

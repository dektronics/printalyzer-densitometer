#include "board_api.h"
#include "stm32l0xx_hal.h"
#include "tusb.h"
#include "app_descriptor.h"
#ifdef HAL_SPI_MODULE_ENABLED
#include "display.h"
#endif

typedef void (*jump_function_t)(void); /*!< Function pointer definition */

extern IWDG_HandleTypeDef hiwdg;
extern CRC_HandleTypeDef hcrc;
#ifdef HAL_UART_MODULE_ENABLED
extern UART_HandleTypeDef huart1;
#endif

#define STM32_UUID ((uint32_t *)UID_BASE)

static uint32_t indicator_state = 0xFF;
static uint8_t progress_state = 0;

void board_uart_write_str(const char *msg)
{
#ifdef HAL_UART_MODULE_ENABLED
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
#else
    UNUSED(msg);
#endif
}

void board_uart_write_hex(uint32_t value)
{
#ifdef HAL_UART_MODULE_ENABLED
    const char nibble_to_hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char buf[16];

    buf[0] = '0';
    buf[1] = 'x';
    buf[2] = nibble_to_hex[(value >> 28UL) & 0xF];
    buf[3] = nibble_to_hex[(value >> 24L) & 0xF];
    buf[4] = nibble_to_hex[(value >> 20UL) & 0xF];
    buf[5] = nibble_to_hex[(value >> 16UL) & 0xF];
    buf[6] = nibble_to_hex[(value >> 12UL) & 0xF];
    buf[7] = nibble_to_hex[(value >> 8UL) & 0xF];
    buf[8] = nibble_to_hex[(value >> 4UL) & 0xF];
    buf[9] = nibble_to_hex[(value >> 0UL) & 0xF];
    buf[10] = '\r';
    buf[11] = '\n';
    buf[12] = '\0';
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, 12, HAL_MAX_DELAY);
#else
    UNUSED(value);
#endif
}

bool board_app_valid(void)
{
    volatile uint32_t const * app_vector = (volatile uint32_t const *)BOARD_FLASH_APP_START;
    volatile const app_descriptor_t *app_descriptor = (volatile const app_descriptor_t *)BOARD_FLASH_APP_DESCRIPTOR;
    volatile uint32_t calculated_crc = 0;

    /* First word is the stack pointer (should be in SRAM region) */
    if (app_vector[0] < SRAM_BASE || app_vector[0] > SRAM_BASE + SRAM_SIZE_MAX) {
        BL_LOG_STR("Invalid stack pointer\r\n");
        return false;
    }

    /* Second word is App entry point (reset vector) */
    if (app_vector[1] < BOARD_FLASH_APP_START || app_vector[1] > BOARD_FLASH_APP_END) {
        BL_LOG_STR("Invalid entry point\r\n");
        return false;
    }

    /* Verify the application checksum */
    calculated_crc =
        HAL_CRC_Calculate(&hcrc, (uint32_t *)BOARD_FLASH_APP_START, BOARD_FLASH_APP_SIZE);

    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();

    if (app_descriptor->crc32 != calculated_crc) {
        BL_LOG_STR("Invalid checksum\r\n");
        return false;
    } else {
        BL_LOG_STR("App checksum is valid\r\n");
    }

    return true;
}

__USED void board_app_jump(void)
{
    /*
     * This function must be tagged with the "used" attribute to prevent
     * LTO from messing with it. Otherwise, its behavior seems to be
     * unreliable and it may fail to jump to the application.
     */

    uint32_t jump_address = *(__IO uint32_t*)(BOARD_FLASH_APP_START + 4);
    jump_function_t jump = (jump_function_t)jump_address;

    /* De-initialize the clock and peripherals */
    HAL_RCC_DeInit();
    HAL_DeInit();

    /* Stop the SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Set the vector table location */
    SCB->VTOR = BOARD_FLASH_APP_START;

    /* Set the stack pointer location */
    __set_MSP(*(__IO uint32_t*)BOARD_FLASH_APP_START);

    jump();
}

void board_dfu_complete(void)
{
    BL_LOG_STR("DFU complete\r\n");
    NVIC_SystemReset();
}

uint8_t board_usb_get_serial(uint8_t *serial_id)
{
    const uint32_t deviceserial0 = HAL_GetUIDw0();
    const uint32_t deviceserial1 = HAL_GetUIDw1();
    const uint32_t deviceserial2 = HAL_GetUIDw2();
    memcpy(serial_id, &deviceserial0, 4);
    memcpy(serial_id + 4, &deviceserial1, 4);
    memcpy(serial_id + 8, &deviceserial2, 4);
    return 12;
}

void board_flash_init(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_FLASH_Unlock();

    /* Clear flash flags */
    __HAL_FLASH_CLEAR_FLAG(
        FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |
        FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR | FLASH_FLAG_FWWERR |
        FLASH_FLAG_NOTZEROERR);

    HAL_FLASH_Lock();
}

uint32_t board_flash_size(void)
{
    return BOARD_FLASH_SIZE;
}

static bool is_blank(uint32_t addr, uint32_t size)
{
    for (uint32_t i = 0; i < size; i += sizeof(uint32_t)) {
        if (*(uint32_t*)(addr + i) != 0x00000000UL) {
            return false;
        }
    }
    return true;
}

static bool flash_erase(uint32_t addr, uint32_t len)
{
    /*
     * This function assumes that the address is page aligned
     * and that the length is an even multiple of the page size.
     */

    if (!is_blank(addr, len)) {
        uint32_t page_error = 0;
        BL_LOG_STR("Erase flash pages\r\n");
        BL_LOG_HEX(len / FLASH_PAGE_SIZE);

        FLASH_EraseInitTypeDef erase_init = {0};
        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.PageAddress = addr;
        erase_init.NbPages = len / FLASH_PAGE_SIZE;
        HAL_FLASHEx_Erase(&erase_init, &page_error);
        if (page_error != 0xFFFFFFFF) {
            BL_LOG_STR("Failed to erase\r\n");
            return false;
        }
    }

    return true;
}

__USED __RAM_FUNC void flash_write(uint32_t dst, const uint8_t *src, int len)
{
    HAL_StatusTypeDef status = HAL_OK;

    /*
     * This function assumes that the address is page aligned
     * and that the length is an even multiple of the page size.
     */
    BL_LOG_STR("Writing to flash\r\n");

    for (size_t i = 0; i < len; i += FLASH_PAGE_SIZE/2) {
        uint32_t *data = (uint32_t *)((void*)(src + i));
        __disable_irq();
        status = HAL_FLASHEx_HalfPageProgram(dst + i, data);
        __enable_irq();
        if (status != HAL_OK) {
            BL_LOG_STR("Failed to write flash at address\r\n");
            BL_LOG_HEX(dst + i);
            BL_LOG_HEX(status);

            uint32_t ferror;
            HAL_FLASHEx_GetError(&ferror);
            BL_LOG_HEX(ferror);
        }
    }

    if (memcmp((void*)dst, src, len) != 0) {
        BL_LOG_STR("Failed to write\r\n");
    }
}

void board_flash_read(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    memcpy(buffer, (void*)addr, len);
}

void board_flash_flush(void)
{
}

void board_flash_write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    indicator_set(STATE_WRITING_STARTED);
    BL_LOG_STR("Write to flash requested\r\n");
    BL_LOG_HEX(addr);

    /* Make sure we have a valid address past the bootloader */
    if (addr < BOARD_FLASH_APP_START) {
        return;
    }

    /* Make sure our start address is aligned to a 128 byte page */
    if ((addr & ~(FLASH_PAGE_SIZE - 1)) != addr) {
        BL_LOG_STR("Address is not page aligned\r\n");
        return;
    }

    /* Check that length is a multiple of the page size */
    if (len % FLASH_PAGE_SIZE != 0) {
        BL_LOG_STR("Length is not page aligned\r\n");
        BL_LOG_HEX(len);
        return;
    }

    /* Check that length doesn't go past the remaining flash */
    if (len > BOARD_FLASH_SIZE - (addr - FLASH_BASE)) {
        BL_LOG_STR("Length is too big\r\n");
        BL_LOG_HEX(len);
        return;
    }

    /* Only write if the data has changed */
    HAL_IWDG_Refresh(&hiwdg);
    if (memcmp((void*)addr, data, len) != 0) {
        HAL_FLASH_Unlock();

        HAL_IWDG_Refresh(&hiwdg);
        if (flash_erase(addr, len)) {
            HAL_IWDG_Refresh(&hiwdg);
            flash_write(addr, data, len);
        }

        HAL_FLASH_Lock();
    }
    HAL_IWDG_Refresh(&hiwdg);

    /* Estimate and display progress */
    uint8_t progress = (uint8_t)((((addr + len) - BOARD_FLASH_APP_START) * 128UL) / (BOARD_FLASH_APP_SIZE * 4UL));
    if (progress_state != progress) {
#ifdef HAL_SPI_MODULE_ENABLED
        display_show_progress(progress);
#endif
        progress_state = progress;
    }
    HAL_IWDG_Refresh(&hiwdg);
}

void indicator_set(uint32_t state)
{
    if (indicator_state != state) {
#ifdef HAL_SPI_MODULE_ENABLED
        if (state == STATE_BOOTLOADER_STARTED) {
            display_show_usb_logo();
        } else if (state == STATE_WRITING_STARTED) {
            display_show_update_icon();
        }
#endif
        indicator_state = state;
    }
}

void board_display_init(void)
{
#ifdef HAL_SPI_MODULE_ENABLED
    display_init();
    display_enable();
#endif
}

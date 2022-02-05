#include "state_suspend.h"

#define LOG_TAG "state_suspend"
#include <elog.h>

#include <cmsis_os.h>

#include "display.h"
#include "keypad.h"
#include "task_sensor.h"
#include "util.h"
#include "main.h"
#include "board_config.h"

extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim2;

typedef enum {
    SUSPEND_IDLE,
    SUSPEND_ACTIVATED,
    SUSPEND_DEACTIVATED
} suspend_state_t;

typedef struct {
    state_t base;
    suspend_state_t suspend_state;
} state_suspend_t;

static void state_suspend_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_suspend_process(state_t *state_base, state_controller_t *controller);
static void state_suspend_exit(state_t *state, state_controller_t *controller, state_identifier_t next_state);
static state_suspend_t state_suspend_data = {
    .base = {
        .state_entry = state_suspend_entry,
        .state_process = state_suspend_process,
        .state_exit = state_suspend_exit
    },
    .suspend_state = SUSPEND_IDLE
};

state_t *state_suspend()
{
    return (state_t *)&state_suspend_data;
}

void state_suspend_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_suspend_t *state = (state_suspend_t *)state_base;
    state->suspend_state = SUSPEND_IDLE;

    log_i("Entering suspend state");

    /* Turn off all the external devices */
    sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
    sensor_stop();
    display_enable(false);

    /* Disable EXTI interrupts connected to external buttons */
    HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

    /* Reconfigure external button GPIO pins to analog */
    gpio_button_unconfig();

    /* Disable PWM timers */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* Disable HAL and FreeRTOS tick timers */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    HAL_SuspendTick();

    /* Enable the periodic RTC wakeup timer */
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 512, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    /* Suspend should now be active */
    state->suspend_state = SUSPEND_ACTIVATED;
}

void state_suspend_process(state_t *state_base, state_controller_t *controller)
{
    state_suspend_t *state = (state_suspend_t *)state_base;

    /*
     * This loop should be the only non-ISR code that can run while we are
     * in the low-power "suspend" state, because the system timers that
     * control FreeRTOS task switching should be disabled. The sequence
     * that breaks this loop is triggered by the USB wakeup interrupt.
     */
    while(state->suspend_state != SUSPEND_IDLE) {
        if (state->suspend_state == SUSPEND_ACTIVATED) {
            /* Suspend has been activated or reactivated, enter STOP mode */
            __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
            HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

        } else if (state->suspend_state == SUSPEND_DEACTIVATED) {
            /* Suspend has been deactivated, turn various clocks back on */

            /* Restore the system clock configuration */
            system_clock_config();

            /* Disable the RTC wakeup timer */
            HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

            /* Enable HAL and FreeRTOS tick timers */
            HAL_ResumeTick();
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

            state->suspend_state = SUSPEND_IDLE;
        }
    }

    /* Do nothing but let the scheduler run */
    osDelay(1);
}

void state_suspend_usb_wakeup_handler()
{
    /* Suspend should be deactivated when returning from the ISR */
    state_suspend_data.suspend_state = SUSPEND_DEACTIVATED;
}

void state_suspend_rtc_wakeup_handler()
{
    /*
     * The RTC wakeup just needs to be handled by refreshing the watchdog.
     * Once this ISR returns, the code above will loop around and re-enter
     * STOP mode.
     */
    watchdog_refresh();
}

void state_suspend_exit(state_t *state, state_controller_t *controller, state_identifier_t next_state)
{
    log_i("Leaving suspend state");

    /* Reconfigure external button GPIO pins to their normal state */
    gpio_button_config();

    /* Enable EXTI interrupts connected to external buttons */
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    /* Enable PWM timers */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* Turn on all the external devices */
    display_enable(true);
    display_clear();
    keypad_clear_events();
}

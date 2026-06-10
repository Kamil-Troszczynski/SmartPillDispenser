#include "power.hpp"
#include "app_state.hpp"
#include "schedules.hpp"
#include "persons.hpp"
#include "screen.hpp"
#include "buttons.hpp"
#include "servos.hpp"
#include "dfns_consts_libs.hpp"
#include "esp_sleep.h"

static const unsigned long DISPLAY_IDLE_MS = 30000;
static const unsigned long LOOP_IDLE_MS    = 50;

static int lastClockMinute = -1;

static bool any_schedule_activity() {
    for (int i = 0; i < NUM_PERSONS; i++) {
        if (appState.buzzerActive[i] || appState.waitingForSensor[i])
            return true;
        if (is_in_dose_window(i) && !appState.buzzerAcked[i])
            return true;
    }
    return false;
}

static bool display_should_be_on() {
    if (appState.detailView) return true;
    if (any_schedule_activity()) return true;
    if (millis() - appState.lastUserActivityMs < DISPLAY_IDLE_MS) return true;
    return false;
}

static bool can_light_sleep() {
    if (appState.detailView) return false;
    if (any_schedule_activity()) return false;
    return true;
}

static void display_sleep_hw() {
    if (!appState.displayOn) return;
    tft.fillScreen(C_SLEEP);
    tft.writecommand(ST7735_DISPOFF);
    delay(5);
    tft.writecommand(ST7735_SLPIN);
    appState.displayOn = false;
}

static void display_wake_hw() {
    if (appState.displayOn) return;
    tft.writecommand(ST7735_SLPOUT);
    delay(120);
    tft.writecommand(ST7735_DISPON);
    delay(5);
    appState.displayOn = true;
}

static void enter_light_sleep(uint64_t sleepUs, Adafruit_MCP23X17& mcp,
                              Adafruit_PWMServoDriver& pca) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_sleep_enable_timer_wakeup(sleepUs);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)MCP_INT_PIN, 0);

    esp_light_sleep_start();

    buttons_after_wake(mcp);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        power_notify_user_activity();
        handle_buttons_mcp(mcp, pca);
    }
}

void power_init(Adafruit_MCP23X17& mcp) {
    (void)mcp;
    appState.displayOn = true;
    appState.lastUserActivityMs = millis();
    lastClockMinute = rtc.getHour(true) * 60 + rtc.getMinute();

    if (WiFi.status() == WL_CONNECTED)
        WiFi.setSleep(true);
}

void power_notify_user_activity() {
    appState.lastUserActivityMs = millis();
    display_wake_hw();
}

void power_ensure_display_on() {
    display_wake_hw();
}

bool power_should_poll_buttons() {
    return display_should_be_on() || !can_light_sleep();
}

void power_tick(Adafruit_MCP23X17& mcp, Adafruit_PWMServoDriver& pca) {
    if (display_should_be_on()) {
        if (!appState.displayOn) {
            display_wake_hw();
            draw_ui();
        }

        int currentMinute = rtc.getHour(true) * 60 + rtc.getMinute();
        if (currentMinute != lastClockMinute) {
            lastClockMinute = currentMinute;
            draw_header();
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_IDLE_MS));
        return;
    }

    display_sleep_hw();

    if (!can_light_sleep()) {
        vTaskDelay(pdMS_TO_TICKS(LOOP_IDLE_MS));
        return;
    }

    enter_light_sleep(us_until_next_schedule_wake(), mcp, pca);
}

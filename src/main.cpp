#include "main.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "bitmaps.h"

void setup() 
{
    Serial.begin(115200);

    pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_LEFT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_A, INPUT_PULLUP);
    pinMode(PIN_BUTTON_B, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    uint8_t retries = 4;

    while (WiFi.status() != WL_CONNECTED && retries) 
    {
        delay(500);
        Serial.print(".");

        --retries;
    }
    
    if(WiFi.status() != WL_CONNECTED)
    {
        Serial.println("\nWiFi connected!");
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi!");
    }

    tft.init();
    tft.initDMA();
    tft.setRotation(3);
    tft.setSwapBytes(true);
    tft.setTextFont(1);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);

    set_app(ApplicationType::DESKTOP);
    create_desired_app(tft);
}

void check_button_input()
{
    const uint8_t button_repeat_rate = 5;

    static uint8_t button_pins[6] = 
    { 
        PIN_BUTTON_UP,
        PIN_BUTTON_DOWN,
        PIN_BUTTON_LEFT,
        PIN_BUTTON_RIGHT,
        PIN_BUTTON_A,
        PIN_BUTTON_B
    };

    static bool previous_button_states[6] = { 0 };
    static uint8_t button_down_count[6] = { 0 };

    for (int i = 0; i < 6; i++)
    {
        bool current_state = digitalRead(button_pins[i]);
        
        if (current_state == LOW && previous_button_states[i] == HIGH)
        {
            get_app()->button_input((Button)i, ButtonAction::PRESSED);

            button_down_count[i] = 1;
        }
        else if (current_state == LOW && previous_button_states[i] == LOW)
        {
            button_down_count[i]++;

            if (button_down_count[i] >= button_repeat_rate)
            {
                get_app()->button_input((Button)i, ButtonAction::BUTTON_DOWN);
                button_down_count[i] = 0;
            }
        }
        else if (current_state == HIGH && previous_button_states[i] == LOW)
        {
            get_app()->button_input((Button)i, ButtonAction::RELEASED);

            button_down_count[i] = 0;
        }

        previous_button_states[i] = current_state;
    }
}

void check_touch_input()
{
    uint16_t x, y;

    if(tft.getTouch(&x, &y))
    {
        x = tft.width() - x;

        get_app()->touch_input(x, y);
    }
}

void loop() 
{
    static ulong last_time = 0;
    static int frameCount = 0;
    static float fps = 0;
    static ulong last_frame = millis();

    frameCount++;

    if(get_app()->get_app_type() != get_desired_app())
    {
        create_desired_app(tft);
    }

    check_button_input();
    check_touch_input();
       
    get_app()->update(fps);


    if (millis() - last_time >= 1000) 
    {
        if(fps != frameCount)
        {
            fps = frameCount;

            get_app()->slow_update(fps);
        }

        frameCount = 0;
        last_time = millis();
    }

    ulong delta_time = millis() - last_frame;
    long delay_time = 1000 / 30 - delta_time;

    if(delay_time < 0)
    {
        delay_time = 0;
    }

    delay(delay_time);

    last_frame = millis();
}
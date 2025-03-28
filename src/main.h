#pragma once
#include "application.h"
#include "desktop.h"
#include "tetris.h"

#define PIN_BUTTON_UP       26
#define PIN_BUTTON_DOWN     25
#define PIN_BUTTON_LEFT     14
#define PIN_BUTTON_RIGHT    27
#define PIN_BUTTON_A        13
#define PIN_BUTTON_B        12

static const char* ssid = "FRITZ!Box 7490s";
static const char* password = "Jupiter1234567890";

static TFT_eSPI tft;

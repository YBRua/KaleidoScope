#ifndef KALEIDOCOMMON_INCLUDED
#define KALEIDOCOMMON_INCLUDED

/**
 * Pin Config
 */
# define CH1 33 // GPIO 32
# define CH2 32 // GPIO 33
# define BUTTON_SELECT 35
# define BUTTON_CONFIRM 0

/**
 * Global Settings
 */
# define READ_BUFFER_SIZE 1024
# define STORAGE_SIZE 240
# define I2S_SAMPLE_RATE 100000

/**
 * LCD Config
 */
// Screen parameters
# define SCR_WIDTH 240
# define SCR_HEIGHT 135
# define FIG_HEIGHT 120 // Region allowed for plotting
# define FIG_MARGIN 15 // Margin
# define ANALOG_MAX 4095.0 // Used for normalization
# define RATE_OF_RENDERING 25
# define SUBFIG_HEIGHT 55
# define CH1_HEIGHT 120
# define CH2_HEIGHT 60

int CH[2] = {CH1, CH2};
const int CH_HEIGHT[2] = {CH1_HEIGHT, CH2_HEIGHT};

short chBuffer[2][STORAGE_SIZE];
TFT_eSPI tSPI = TFT_eSPI();
TFT_eSprite tft = TFT_eSprite(&tSPI);

uint32_t sampleRate = I2S_SAMPLE_RATE;
bool fancy = true;

#endif

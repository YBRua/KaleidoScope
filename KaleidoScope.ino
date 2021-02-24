// Headers
# include <TFT_eSPI.h>
# include <SPI.h>
# include <driver/i2s.h>
# include <driver/dac.h>
# include "KaleidoCommon.h"
# include "KaleidoUtils.h"
# include "KaleidoManager.h"
# include "KaleidoPlotLib.h"

KaleidoManager kaleidoManager;
KaleidoPlotLib kaleidoArtist(&kaleidoManager);

// Core0 is responsible for Reading Data and Overall Control
void Core0Task(void* pvParameters)
{
  delay(22);
	// I2S Setup
	//i2sInit(ADC1_CHANNEL_4);
	//i2sInit(ADC1_CHANNEL_5);
	
	// Loop
	while(true)
	{
		// Timer
		long currentTick = micros();
		while(currentTick == micros());
		currentTick = micros();
		
		// Read data
		if(!kaleidoManager.isHolding())
		{
			KaleidoRead();
		}
		kaleidoArtist.Render();
		delay(RATE_OF_RENDERING);
	}
}

// Core1 is responsible for Plotting
void Core1Task(void* pvParameters)
{
	while(true)
	{
		//kaleidoArtist.RenderUI();
		kaleidoManager.DoHouseKeeping();
		delay(RATE_OF_RENDERING);
	}
}

void setup() {
	pinMode(BUTTON_SELECT, INPUT_PULLUP);
	pinMode(BUTTON_CONFIRM, INPUT_PULLUP);

	pinMode(CH1, INPUT);
	pinMode(CH2, INPUT);

	Serial.begin(115200);

	tSPI.init();
	tSPI.setRotation(1);
	tSPI.fillScreen(TFT_BLACK);
	
	tft.createSprite(240, 135);

	// I2S Setup
	//i2sInit(ADC1_CHANNEL_4);
	//i2sInit(ADC1_CHANNEL_5);
	
	if(fancy)
		goingFancy();
	
	kaleidoManager.ShowHelper();
	
	xTaskCreatePinnedToCore(Core0Task, "Input&Control", 20000, NULL, 1, NULL, 1); 
	xTaskCreatePinnedToCore(Core1Task, "Plot", 10000, NULL, 1, NULL, 0);
}

void loop() {
    delay(RATE_OF_RENDERING);
}

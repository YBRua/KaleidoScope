#ifndef KALEIDOMANAGER_INCLUDED
#define KALEIDOMANAGER_INCLUDED
# include "KaleidoUtils.h"
class KaleidoManager
{
public:
	static int freqSwitch[15]; // 15 Selectable Sampling Frequencies
	static int kaleidoPalette[4][2]; // Color Palette
	static int paletteNum;
	
	// Sample rate control
	int freqSwitchIndex;
	int samplingFrequency;
	
	// button_state: 0-CH1Ctrl; 1-CH2Ctrl; 2-FrequencyDecr; 3-FrequencyIncr; 4-Hold; 5-Palette
	int buttonSelectState;
	int buttonConfirmState;
	int buttonSelectRead;
	int buttonConfirmRead;
	int buttonSelectPrevious;
	int buttonConfirmPrevious;
	
	// channel_state: 0-Off; 1-On; 2-Half
	// channel_index: 0-ch1; 1-ch2
	int chStatus[2];
	
	// Palette
	int paletteIndex;
	
	// Hold
	bool onHold;

	// Constructor
	KaleidoManager();
  
	// Get Private Params
	int getButtonSelectState();
	int getCH1Status();
	int getCH2Status();
	int getSamplingFreq();
	int getPalette(int ch);
	bool isHolding();
	
	// Handle Button Presses
	void ButtonInputHandler();
	void ButtonConfirmHandler(int selected);
	
	// Handle Serial Command Input
	void CommandInputHandler();
	bool DirectSetChannelStatus(int channel, int status);
	bool DirectSetFreqIndex(int index);
	void RecoverFrequency();
	
	// All-in-One Control Manager
	void DoHouseKeeping();
	
	void ShowHelper();
	void ShowKnownBugs();
};

/**
 * Static Variables
 */
// Frequency Seletcion Table
int KaleidoManager::freqSwitch[15] = {5200, 6000, 7500, 10000, 15000, 20000, 30000, 50000, 
									75000, 100000, 150000, 200000, 300000, 500000, 600000};

// Color Palette
int KaleidoManager::kaleidoPalette[4][2] = {{TFT_CYAN, TFT_MAGENTA}, {TFT_BLUE, TFT_RED}, {TFT_DARKGREEN, TFT_YELLOW}, {TFT_SILVER, TFT_GOLD}};
int KaleidoManager::paletteNum = 4;

/**
 * Constructor
 */
KaleidoManager::KaleidoManager()
{
	freqSwitchIndex = 9;
	samplingFrequency = freqSwitch[freqSwitchIndex];
	
	paletteIndex = 0;
	
	buttonSelectState = 0;
	buttonConfirmState = 0;
	buttonSelectRead = HIGH;
	buttonConfirmRead = HIGH;
	buttonSelectPrevious = HIGH;
	buttonConfirmPrevious = HIGH;
	
	chStatus[0] = 0;
	chStatus[1] = 0;
}

/**
 * Dealing with button/console control inputs
 */
void KaleidoManager::ButtonInputHandler()
{
	buttonSelectRead = digitalRead(BUTTON_SELECT);
	if(buttonSelectPrevious != buttonSelectRead && buttonSelectRead == LOW) // SELECT button has been pressed
	{
		buttonSelectState = (buttonSelectState+1) % 6; // Update Button Selection State
	}
	buttonSelectPrevious = buttonSelectRead; // Update SELECT button status
	
	buttonConfirmRead = digitalRead(BUTTON_CONFIRM);
	if(buttonConfirmPrevious != buttonConfirmRead && buttonConfirmRead == LOW) // CONFIRM button has been pressed
	{
		ButtonConfirmHandler(buttonSelectState); // Handle CONFIRM Operations
	}
	buttonConfirmPrevious = buttonConfirmRead; // Update CONFIRM button status
	
	return;
}

void KaleidoManager::ButtonConfirmHandler(int selected)
{
	switch(selected)
	{
	case 0: // CH1 Ctrl
		chStatus[0] = (chStatus[0]+1) % 3;
		break;
	
	case 1: // CH2 Ctrl
		chStatus[1] = (chStatus[1]+1) % 3;
		break;
		
	case 2: // Freq Decr
		freqSwitchIndex = (freqSwitchIndex+14) % 15;
		samplingFrequency = freqSwitch[freqSwitchIndex];
		
		// Frequency set failed, recover.
		if(!SetFrequency(I2S_NUM_0, samplingFrequency))
			RecoverFrequency();
		break;
		
	case 3: // Freq Incr
		freqSwitchIndex = (freqSwitchIndex+1) % 15;
		samplingFrequency = freqSwitch[freqSwitchIndex];
		
		
		// Frequency set failed, recover.
		if(!SetFrequency(I2S_NUM_0, samplingFrequency))
			RecoverFrequency();
		break;
		
	case 4: // Hold
		onHold = !onHold;
		break;
		
	case 5: // Change Palette
		paletteIndex = (paletteIndex+1) % paletteNum;
		break;
	}
	
	return;
}

void KaleidoManager::ShowHelper()
{
	Serial.println("-===========      Two-Channel Simple Oscilloscope      ===========-");
	Serial.println("-===========               Kaleido Scope               ===========-");
	Serial.println("-===========               EE235 Group22               ===========-");
	Serial.println("Helper File:");
	Serial.println("- Command format: 'Kaleido + cmd + param'.");
	Serial.println("- Commands are case-insensitive.");
	Serial.println("- List of available commands:");
	Serial.println("	- SetFreq + [Value]: Set sample rate (sps).");
	Serial.println("	- SetFreqIndex + [Value]: Set pre-defined sample rate.");
	Serial.println("	- Ch1(or Ch2) + [Value]: Set channel on/off status");
	Serial.println("	  0:Off; 1:On(Full); 2:On(Half)");
	
	return;
}

void KaleidoManager::ShowKnownBugs()
{
	Serial.println("Known Bugs:");
	Serial.println("- Yes. We have noticed these bugs, but we have not fixed them, and we will not fix them.");
	Serial.println("- Extremely low or extremely high sample rate leads to erroneous input.");
	Serial.println("  We will not fix it because it is restricted by the ADC and I2S library functions.");
	Serial.println("- KaleidoScope randomly reboots for unknown reasons.");
	Serial.println("  We will not fix it because the bug lies deep inside the CPU.");
	Serial.println("  No programmers should be messing around with Computer Architectures on this level.");
	Serial.println("- Voltage and Frequency measurements have errors.");
	Serial.println("  We will not fix it because ADC is stupid and maths is hard.");
	Serial.println("- ADC sometimes goes dead.");
	Serial.println("  We will not fix it because it also lies deep inside the CPU.");
	
	return;
}

void KaleidoManager::CommandInputHandler()
{
	String cmdBlock1;
	String cmdBlock2;
	int cmdParam;
	
	if(Serial.available())
	{
		// Parse first commmand string
		cmdBlock1 = Serial.readStringUntil(' ');
		cmdBlock1.toLowerCase();
		if(cmdBlock1 != "kaleido" || !Serial.available())
		{
			Serial.println("KaleidoConsoleError: Invalid or Incomplete Command.");
			Serial.println("Use <Kaleido Help> for help.");
			return;
		}
		
		// Parse second command string
		cmdBlock2 = Serial.readStringUntil(' ');
		cmdBlock2.toLowerCase();
		if(cmdBlock2 == "help")
		{
			ShowHelper();
			return;
		}
		if(cmdBlock2 == "stupid")
		{
			ShowKnownBugs();
			return;
		}
		
		// Parse command parameters
		if(!Serial.available())
		{
			Serial.println("KaleidoConsoleError: Incomplete Command.");
			return;
		}
		cmdParam = Serial.parseInt();

		// Execute Commands
		// Set Sample Rate
		if(cmdBlock2 == "setfreq")
		{
			samplingFrequency = cmdParam;
			// Set failed, recover.
			if(!SetFrequency(I2S_NUM_0, samplingFrequency))
			{
				Serial.println("Failed to set sampling frequency.");
				RecoverFrequency();
			}
		}
		else if(cmdBlock2 == "setfreqindex")
		{
			if(!DirectSetFreqIndex(cmdParam))
			{
				Serial.println("Failed to set sampling frequency.");
			}
		}
		// Control Channel 1
		else if(cmdBlock2 == "ch1")
		{
			if(!DirectSetChannelStatus(0, cmdParam))
			{
				Serial.println("Failed to set status of channel 1.");
			}
			else
			{
				Serial.println("Successfully set status of channel 1 to:");
				Serial.println(cmdParam);
			}
		}
		// Control Channel 2
		else if(cmdBlock2 == "ch2")
		{
			if(!DirectSetChannelStatus(1, cmdParam))
			{
				Serial.println("Failed to set status of channel 2.");
			}
			else
			{
				Serial.println("Successfully set status of channel 2 to:");
				Serial.println(cmdParam);
			}
		}
		// Invalid Command Input
		else
		{
			Serial.println("KaleidoConsoleError: Invalid Command.");
			Serial.println("Use <Kaleido Help> for help.");
		}
	}
	
	return;
}

void KaleidoManager::RecoverFrequency()
{
	freqSwitchIndex = 9;
	samplingFrequency = freqSwitch[freqSwitchIndex];
	
	return;
}

bool KaleidoManager::DirectSetChannelStatus(int channel, int status)
{
	if(status < 0 || status > 2)
	{
		Serial.println("KaleidoValueError: Channel Status should be 0, 1, or 2.");
		return false;
	}
	else
	{
		chStatus[channel] = status;
	}
	
	return true;
	
}

bool KaleidoManager::DirectSetFreqIndex(int index)
{
	if(index < 0 || index > 14)
	{
		Serial.println("KaleidoValueError: Index should be an interger between [0,14].");
		return false;
	}
	freqSwitchIndex = index;
	samplingFrequency = freqSwitch[freqSwitchIndex];
	
	if(!SetFrequency(I2S_NUM_0, samplingFrequency))
	{
		RecoverFrequency();
		return false;
	}
	
	return true;
}

void KaleidoManager::DoHouseKeeping()
{
	ButtonInputHandler();
	CommandInputHandler();
	
	return;
}

/**
 * Fetching Private Variables
 */
int KaleidoManager::getButtonSelectState()
{
	return buttonSelectState;
}

int KaleidoManager::getCH1Status()
{
	return chStatus[0];
}

int KaleidoManager::getCH2Status()
{
	return chStatus[1];
}

int KaleidoManager::getSamplingFreq()
{
	return samplingFrequency;
}

int KaleidoManager::getPalette(int ch)
{
	return kaleidoPalette[paletteIndex][ch];
}

bool KaleidoManager::isHolding()
{
	return onHold;
}

i2s_config_t i2s_config = {
.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
//.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
.sample_rate = sampleRate,              // The format of the signal using ADC_BUILT_IN
.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
.communication_format = I2S_COMM_FORMAT_I2S_MSB,
.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
.dma_buf_count = 16,
.dma_buf_len = 240,
//.use_apll = false,
//.tx_desc_auto_clear = false,
//.fixed_mclk = 0
};

void KaleidoRead()
{
	size_t bytesRead;
	short mask = 0x0FFF;
	// Clear whatever garbage in ADC
	/*
	do
	{
	i2s_read(I2S_NUM_0, &dustbin, sizeof(dustbin), &bytesRead, 1);
	Serial.println("In While");
	}while(bytesRead > 0);
	*/
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	// Input in channel 1
	i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4);
	i2s_set_sample_rates(I2S_NUM_0, sampleRate);
	i2s_adc_enable(I2S_NUM_0);
	bytesRead = 0;
	i2s_read(I2S_NUM_0, &chBuffer[0], sizeof(chBuffer[0]), &bytesRead, portMAX_DELAY);
	i2s_adc_disable(I2S_NUM_0);
	i2s_driver_uninstall(I2S_NUM_0);

	// Clear again, get ready for input in channel 2.
	/*
	do
	{
	 i2s_read(I2S_NUM_0, &dustbin, sizeof(dustbin), &bytesRead, 1);
	}while(bytesRead > 0);
	*/

	// Input in channel 2
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_5);
	i2s_set_sample_rates(I2S_NUM_0, sampleRate);
	i2s_adc_enable(I2S_NUM_0);
	bytesRead = 0;
	i2s_read(I2S_NUM_0, &chBuffer[1], sizeof(chBuffer[1]), &bytesRead, portMAX_DELAY);
	i2s_adc_disable(I2S_NUM_0);
	i2s_driver_uninstall(I2S_NUM_0);

	// Process Data
	for(int i = 0; i < 240; ++i)
	{
	chBuffer[0][i] = -ANALOG_MAX+(chBuffer[0][i] & mask);
	chBuffer[1][i] = -ANALOG_MAX+(chBuffer[1][i] & mask);
	}
	return;
}

#endif

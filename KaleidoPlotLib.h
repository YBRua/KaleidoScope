#ifndef KALEIDOPLOTLIB_INCLUDED
#define KALEIDOPLOTLIB_INCLUDED

class KaleidoPlotLib
{
	public:
	KaleidoManager* manager;
	KaleidoPlotLib(KaleidoManager* managerPtr);
	int buttonState;
	void drawOnOffStatus(int ch, int state);
	void drawWaves(int ch, int bottom, int scaling);
	void drawFreq();
	void drawPalette();
	void drawHold();
	void drawUILines();
	void drawChannelOutput(int ch, int state);
	
	void Render();
	void RenderUI();
	void RenderWave();
	
	void DrawStats();
};

KaleidoPlotLib::KaleidoPlotLib(KaleidoManager* managerPtr)
{
	manager = managerPtr;
	buttonState = manager->getButtonSelectState();
}

void KaleidoPlotLib::DrawStats()
{
	// Calculation
	long result[2][STORAGE_SIZE];
	long stupid[2][STORAGE_SIZE];
	for(int i = 0; i < 2; ++i)
		for(int j = 0; j < STORAGE_SIZE; ++j)
		{
			stupid[i][j] = long(chBuffer[i][j]) + ANALOG_MAX;
		}
	long ch1Max = Max(stupid[0], 0, STORAGE_SIZE);
	long ch2Max = Max(stupid[1], 0, STORAGE_SIZE);
	long ch1Min = Min(stupid[0], 0, STORAGE_SIZE);
	long ch2Min = Min(stupid[1], 0, STORAGE_SIZE);

	AutoCorr(result[0], stupid[0], 0, STORAGE_SIZE);
	AutoCorr(result[1], stupid[1], 0, STORAGE_SIZE);
	int ch1Index = ArgMax(result[0], 1, STORAGE_SIZE);
	int ch2Index = ArgMax(result[1], 1, STORAGE_SIZE);
	
	// Conversion
	float ch1VoltageHigh = ch1Max / ANALOG_MAX * 3.3;
	float ch1VoltageLow = ch1Min / ANALOG_MAX * 3.3;
	float ch2VoltageHigh = ch2Max / ANALOG_MAX * 3.3;
	float ch2VoltageLow = ch2Min / ANALOG_MAX * 3.3;
	float ch1Freq = 2 * float(manager->getSamplingFreq()) / ch1Index;
	float ch2Freq = 2 * float(manager->getSamplingFreq()) / ch2Index;
	
	// Draw
	int ch1X = 0;
	int ch1Y = 0;
	int ch2X = 239;
	int ch2Y = 0;
	int bias = 8;
	// Channel 1
	if(manager->getCH1Status())
	{
		tft.setTextDatum(TL_DATUM);
		tft.setTextColor(manager->getPalette(0), TFT_BLACK);
		tft.drawString("Channel:1", ch1X, ch1Y, 1);
		ch1Y += bias;
		tft.drawString("Max:" + String(ch1VoltageHigh, 2) + "V", ch1X, ch1Y, 1);
		ch1Y += bias;
		tft.drawString("Min:" + String(ch1VoltageLow, 2) + "V", ch1X, ch1Y, 1);
		ch1Y += bias;
		if(ch1Freq > 1000)
		{
			ch1Freq /= 1000;
			tft.drawString("Freq:" + String(ch1Freq, 3) + "kHz", ch1X, ch1Y, 1);
		}
		else
			tft.drawString("Freq:" + String(int(ch1Freq)) + "Hz", ch1X, ch1Y, 1);	
	}
	// Channel 2
	if(manager->getCH2Status())
	{
		tft.setTextDatum(TR_DATUM);
		tft.setTextColor(manager->getPalette(1), TFT_BLACK);
		tft.drawString("Channel:2", ch2X, ch2Y, 1);
		ch2Y += bias;
		tft.drawString("Max:" + String(ch2VoltageHigh, 2) + "V", ch2X, ch2Y, 1);
		ch2Y += bias;
		tft.drawString("Min:" + String(ch2VoltageLow, 2) + "V", ch2X, ch2Y, 1);
		ch2Y += bias;
		if(ch2Freq > 1000)
		{
			ch2Freq /= 1000;
			tft.drawString("Freq:" + String(ch2Freq, 3) + "kHz", ch2X, ch2Y, 1);
		}
		else
			tft.drawString("Freq:" + String(int(ch2Freq)) + "Hz", ch2X, ch2Y, 1);
	}
	
	return;
}

// Ch1 and Ch2 On/Off Status
void KaleidoPlotLib::drawOnOffStatus(int ch, int state)
{
	// Setting Starting point of different channels
	int xStart = (ch==0) ? 2 : 27;
	int yStart = (ch==0) ? 4 : 0;
	int textCenter = (ch==0) ? 15 : 40;
	int chButtonWidth = 25;
	
	// Set Colors
	int textColor = TFT_WHITE;
	int bgColor = TFT_BLACK;
	int chColor = manager->getPalette(ch);
	if((ch == 0 && buttonState == 0)||(ch == 1 && buttonState == 1)) // Button selecting at CH1/CH2 Control
	{
		textColor = TFT_BLACK;
		bgColor = TFT_WHITE;
	}
	
	// Draw Text: gray if deactivated else white
	tft.setTextDatum(MC_DATUM);
	tft.setTextColor((state==0) ? TFT_DARKGREY : textColor, bgColor);
	tft.drawString((ch==0)?"CH1":"CH2", textCenter, SCR_HEIGHT-1-(FIG_MARGIN/2), 1);
	
	// Draw Line: GRAY: off; FULL: centered; HALF: separated
	switch(state)
	{
	case 0: // OFF
		tft.drawLine(xStart, SCR_HEIGHT-12, xStart, SCR_HEIGHT-6, TFT_DARKGREY);
		tft.drawLine(xStart+1, SCR_HEIGHT-12, xStart+1, SCR_HEIGHT-6, TFT_DARKGREY);
		break;
		
	case 1: // FULL
		tft.drawLine(xStart, SCR_HEIGHT-12, xStart, SCR_HEIGHT-6, chColor);
		tft.drawLine(xStart+1, SCR_HEIGHT-12, xStart+1, SCR_HEIGHT-6, chColor);
		break;
		
	case 2: // HALF
		tft.drawLine(xStart, SCR_HEIGHT-12, xStart, SCR_HEIGHT-6, TFT_DARKGREY);
		tft.drawLine(xStart+1, SCR_HEIGHT-12, xStart+1, SCR_HEIGHT-6, TFT_DARKGREY);
		tft.drawLine(xStart, SCR_HEIGHT-12+yStart, xStart, SCR_HEIGHT-9+yStart, chColor);
		tft.drawLine(xStart+1, SCR_HEIGHT-12+yStart, xStart+1, SCR_HEIGHT-9+yStart, chColor);
		break;
	}
	
	return;
}

// Input waves
void KaleidoPlotLib::drawWaves(int ch, int bottom, int scaling)
{
	int chColor = manager->getPalette(ch);
	for(int x = 1; x < SCR_WIDTH; ++x)
	{
		tft.drawLine(x-1, bottom + float(chBuffer[ch][x-1]) / ANALOG_MAX * scaling,
		x, bottom + float(chBuffer[ch][x]) / ANALOG_MAX * scaling, chColor);
	}
	
	return;
}

// Show current sample rate and selection buttons
void KaleidoPlotLib::drawFreq()
{
	int rate = manager->getSamplingFreq();
	
	// Button pos and color config
	int decrButtonColor = (buttonState==2) ? TFT_WHITE : TFT_BLACK;
	int decrTextColor = (buttonState==2) ? TFT_BLACK : TFT_WHITE;
	int decrXStart = 50;
	int decrButtonCenter = 57;
	int incrButtonColor = (buttonState==3) ? TFT_WHITE : TFT_BLACK;
	int incrTextColor = (buttonState==3) ? TFT_BLACK : TFT_WHITE;
	int incrXStart = 180;
	int incrButtonCenter = 177;
	int buttonWidth = 5;
	
	// Freq pos config
	int freqStrCenter = 117;
	
	// Draw Buttons
	tft.setTextDatum(MC_DATUM);
	tft.setTextColor(decrTextColor, decrButtonColor);
	tft.drawString("<", decrButtonCenter, SCR_HEIGHT-1-(FIG_MARGIN/2), 1);
	tft.setTextColor(incrTextColor, incrButtonColor);
	tft.drawString(">", incrButtonCenter, SCR_HEIGHT-1-(FIG_MARGIN/2), 1);
	
	// Display Freq
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.drawNumber(rate, freqStrCenter, SCR_HEIGHT-1-(FIG_MARGIN/2));
	
	return;
}

// Display "HOLD" button
void KaleidoPlotLib::drawHold()
{
	// Button pos and color config
	int xStart = 185;
	int buttonCenter = 207;
	int buttonWidth = 45;
	int holdTextColor = (buttonState==4) ? TFT_BLACK : TFT_WHITE;
	int holdButtonColor = (buttonState==4) ? TFT_WHITE : TFT_BLACK;
	holdTextColor = manager->isHolding() ? holdTextColor : TFT_DARKGREY;
	
	
	// Draw button
	tft.setTextDatum(MC_DATUM);
	tft.setTextColor(holdTextColor, holdButtonColor);
	tft.drawString("HOLD", buttonCenter, SCR_HEIGHT-1-(FIG_MARGIN/2), 1);
	
	return;
}

// Display color palette
void KaleidoPlotLib::drawPalette()
{
	// Button pos and color config
	int buttonStart = 230;
	int ch1XStart = 232;
	int ch2XStart = 235;
	int paletteWidth = 3;
	int buttonWidth = 10;
	int ch1Color = manager->getPalette(0);
	int ch2Color = manager->getPalette(1);
	int bgColor = (buttonState == 5) ? TFT_WHITE : TFT_BLACK;
	
	tft.fillRect(buttonStart, FIG_HEIGHT+1, buttonWidth, FIG_MARGIN, bgColor);
	for(int i = 0; i < paletteWidth; ++i)
	{
		tft.drawLine(ch1XStart+i, SCR_HEIGHT-12, ch1XStart+i, SCR_HEIGHT-6, ch1Color);
		tft.drawLine(ch2XStart+i, SCR_HEIGHT-12, ch2XStart+i, SCR_HEIGHT-6, ch2Color);
	}
	
	return;
}

// Draw UI lines
// To be further implemented
void KaleidoPlotLib::drawUILines()
{
	tft.drawLine(0, FIG_HEIGHT, SCR_WIDTH, FIG_HEIGHT, TFT_WHITE);
}

// Display the channel ch
void KaleidoPlotLib::drawChannelOutput(int ch, int state)
{
	switch(state)
	{
	case 0: // Channel is turned-off
		drawOnOffStatus(ch, state);
		break;
	
	case 1: // Plot at center
		drawOnOffStatus(ch, state);
		drawWaves(ch, FIG_HEIGHT-1, FIG_HEIGHT);
		break;
	
	case 2: // Plot Separately
		drawOnOffStatus(ch, state);
		drawWaves(ch, CH_HEIGHT[ch]-1, SUBFIG_HEIGHT);
		break;
	}
}

// Draw user interface
void KaleidoPlotLib::RenderUI()
{
	buttonState = manager->getButtonSelectState();
	tft.fillSprite(TFT_BLACK);
	drawUILines();
	drawFreq();
	drawHold();
	drawPalette();
	return;
}

// Draw waves
void KaleidoPlotLib::RenderWave()
{
	buttonState = manager->getButtonSelectState();
	drawChannelOutput(0, manager->getCH1Status());
	drawChannelOutput(1, manager->getCH2Status());
	return;
}

// Draw everything
void KaleidoPlotLib::Render()
{
	buttonState = manager->getButtonSelectState();
	tft.fillSprite(TFT_BLACK);
	drawUILines();
	drawFreq();
	drawHold();
	drawPalette();
	drawChannelOutput(0, manager->getCH1Status());
	drawChannelOutput(1, manager->getCH2Status());
	if(manager->isHolding())
		DrawStats();
	tft.pushSprite(0,0);
	
	return;
}

#endif

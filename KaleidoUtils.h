#ifndef KALEIDOUTILS_INCLUDED
#define KALEIDOUTILS_INCLUDED

bool SetFrequency(i2s_port_t i2s_num, uint32_t rate)
{
	if(rate < 5200 || rate > 600000)
		Serial.println("Warning: Too Low or Too High Rate is likely to cause stupid behaviours.");
	sampleRate = rate;
	Serial.println("Successfully set sample rate to:");
	Serial.println(rate);
	
	return true;
}

void goingFancy()
{
	tSPI.setTextSize(1);
    tSPI.setTextColor(TFT_WHITE, TFT_BLACK);
    tSPI.setTextDatum(BC_DATUM);
    tSPI.fillScreen(TFT_BLACK);
    tSPI.drawString("Projekt Kaleido", 120, 70, 4);
	tSPI.drawString("Remastered", 180, 80, 1);
    tSPI.drawString("EE235 Grp 22", 120, 100, 2);
    tSPI.drawRect(68, 110, 102, 17, TFT_WHITE); // From 68 to 169; 110 to 126
    tSPI.drawRect(69, 111, 100, 15, TFT_BLACK); // From 69 to 168; 111 to 125
    
	for(int j = 0; j <= 100; ++j)
    {
			tSPI.drawLine(69+j, 111, 69+j, 125, TFT_WHITE);
			// Pretend to be doing some difficult initializations	
			delay(12);
    }
	delay(500);
}

long Max(long array[], int start, int end)
{
	long max = -1;
	for(int i = start; i < end; ++i)
		if(array[i] > max)
			max = array[i];
		
	return max;
}

int ArgMax(long array[], int start, int end)
{
	int index = 1903802;
	int max = -1;
	for(int i = start; i < end; ++i)
	if(array[i] > max && array[i-1] < array[i] && array[i+1] < array[i])
		{
			max = array[i];
			index = i;
		}
		
	return index;
}

long Min(long array[], int start, int end)
{
	long min = 5000;
	for(int i = start; i < end; ++i)
		if(array[i] < min)
			min = array[i];
		
	return min;
}

void AutoCorr(long result[], long array[], int start, int end)
{
	for(int i = start; i < end; ++i)
	{
		long sum = 0;
		for(int j = i; j < end; ++j)
		{
			sum += array[j] * array[j-i];
		}
		result[i] = sum;
	}
	
	return;
}

#endif

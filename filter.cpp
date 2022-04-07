#include "daisy_pod.h"
#include "daisysp.h"

#include "utilities.h"
#include "filter.h"

using namespace daisy;
using namespace daisysp;


void Filters::Init(float sr) 
{ 
	sampleRate = sr;
	currentFilterSelector = NO_FILTER; 
	
	pfilter = &nfilter;
	
	sfilter.Init(sampleRate);
	mfilter.Init(sampleRate);
}


void Filters::Select(int8_t sel)
{
	if (sel == 0)
	{
		return;
	}
	
	int8_t s = currentFilterSelector;
	
	s += sel;
	if (s < 0)
	{
		s = NUM_FILTERS - 1;
	}
	
	if (s > NUM_FILTERS - 1)
	{
		s = 0;
	}
	
	
	currentFilterSelector = s;
	
	switch (currentFilterSelector)
	{
	case SV_FILTER:
		log("SV filter");
		pfilter = &sfilter;
		break;
		
	case MOOG_FILTER:
		log("Moog filter");
		pfilter = &mfilter;
		break;

	case NO_FILTER:
		log("No filter");
		pfilter = &nfilter;
		break;

	default:
		log("Unknown filter");
		break;
	}	
}


void Filters::SetFreqCC(uint8_t value)
{
	//f must be between 0.0 and sample_rate / 3
	float f = (float)value / 127.0f * sampleRate / 3.0f;
	pfilter->SetFreq(f);
}


void Filters::SetResCC(uint8_t value)
{	
	pfilter->SetRes((float)value / 127.0f);
}


float Filters::Process(float in)
{	
	return pfilter->Process(in);
}


void Filters::SetCC0(uint8_t value)
{	
	SetFreqCC(value);
}


void Filters::SetCC1(uint8_t value)
{	
	SetResCC(value);
}

void Filters::CCProcess(uint8_t ccFuncNumber, uint8_t value)
{
	switch (ccFuncNumber)
	{
	case 0:
		SetFreqCC(value);
		break;
		
	case 1:
		SetResCC(value);
		break;
		
	default:
		break;
		
	}
}



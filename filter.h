#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;


class NullFilter
{
public:
	void Init() {}
	virtual void SetRes(float r) {}
	virtual void SetFreq(float f) {}
	virtual float Process(float in) { return in; }	
};



class SVFilter: public NullFilter
{
public:
	void Init(float sampleRate) {filter.Init(sampleRate); }
	void SetRes(float r) { filter.SetRes(r); }
	void SetFreq(float f) { filter.SetFreq(f); }
	float Process(float in) { filter.Process(in);  return filter.Low(); }	
	
private:
	Svf	filter;	
};



class MoogFilter : public NullFilter
{
public:
	void Init(float sampleRate) {filter.Init(sampleRate); }
	void SetRes(float r) { filter.SetRes(r); }
	void SetFreq(float f) { filter.SetFreq(f); }
	float Process(float in) { return filter.Process(in);  }	
	
private:
	MoogLadder	filter;	
};



// a container for filters

class Filters
{
public:
	Filters() {}
		
	void Init(float sampleRate); 
	
	// 1/-1 rotates thru filters, 0 does nothing
	void Select(int8_t sel);
	
	float Process(float in);
	
	void SetCC0(uint8_t value);
	void SetCC1(uint8_t value);
	

private:
	float sampleRate;	
	uint8_t currentFilterSelector;
	
	typedef enum
	{
		NO_FILTER,
		SV_FILTER,
		MOOG_FILTER
	}FILTER_TYPE;
	
	#define NUM_FILTERS 3
	
	NullFilter	nfilter;
	SVFilter	sfilter;
	MoogFilter	mfilter;
	
	NullFilter *pfilter;
	
	void SetFreqCC(uint8_t value);
	void SetResCC(uint8_t value);
};


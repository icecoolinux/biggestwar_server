#ifndef _area_h_
#define _area_h_

#include "vec2.h"
#include "../time.h"
#include "../defs.h"
#include "../config.h"
#include <stdio.h>

class Area
{
	private:
		
		bool thereIsArea;
		unsigned long long tsArea;
		bool areaIsClosing;
		vec2 futureCenterArea;
		
		float currentBottomArea;
		float currentTopArea;
		float currentLeftArea;
		float currentRightArea;
		
		float pastBottomArea;
		float pastTopArea;
		float pastLeftArea;
		float pastRightArea;
		
		float futureBottomArea;
		float futureTopArea;
		float futureLeftArea;
		float futureRightArea;
		
		void getSpeedClose(int ms, float &closeBottom, float &closeTop, float &closeLeft, float &closeRight);
		
		void initFutureArea();
		void stepFutureArea();
		
	public:
		Area();
		~Area();
		
		void start();
		void update(int ms);
		
		bool isClosing();
		
		void get(bool &isClosing, int &msLeft, 
					vec2 &futureCenter, int &currentBottom, int &currentTop, int &currentLeft, int &currentRight,
					int &futureBottom, int &futureTop, int &futureLeft, int &futureRight,
					float &speedCloseSecBottom, float &speedCloseSecTop, float &cspeedCloseSecLeft, float &speedCloseSecRight);
		
		bool isOut(vec2 &pos);
};

#endif

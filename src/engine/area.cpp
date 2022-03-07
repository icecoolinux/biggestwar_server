
#include "area.h"

Area::Area()
{
	this->thereIsArea = false;
	this->areaIsClosing = false;
	this->tsArea = Time::currentMs();
	
	futureCenterArea.x = SIDE_MAP_METERS/2;
	futureCenterArea.y = SIDE_MAP_METERS/2;
		
	currentBottomArea = 0.0f;
	currentTopArea = SIDE_MAP_METERS;
	currentLeftArea = 0.0f;
	currentRightArea = SIDE_MAP_METERS;
		
	pastBottomArea = 0.0f;
	pastTopArea = SIDE_MAP_METERS;
	pastLeftArea = 0.0f;
	pastRightArea = SIDE_MAP_METERS;
		
	futureBottomArea = 0.0f;
	futureTopArea = SIDE_MAP_METERS;
	futureLeftArea = 0.0f;
	futureRightArea = SIDE_MAP_METERS;
}

Area::~Area()
{
}

void Area::start()
{
	if(thereIsArea)
		return;
	
	thereIsArea = true;
	tsArea = Time::currentMs();
	areaIsClosing = true;
	
	this->initFutureArea();
	
	pastBottomArea = currentBottomArea;
	pastTopArea = currentTopArea;
	pastLeftArea = currentLeftArea;
	pastRightArea = currentRightArea;
}

void Area::getSpeedClose(int ms, float &closeBottom, float &closeTop, float &closeLeft, float &closeRight)
{
	float closed = (((float)SPEED_CLOSE_AREA) * ((float)ms))/1000.0f;
		
	closeBottom = futureBottomArea - pastBottomArea;
	closeTop = pastTopArea - futureTopArea;
	closeLeft = futureLeftArea - pastLeftArea;
	closeRight = pastRightArea - futureRightArea;

	if(closeBottom > closeTop && closeBottom > closeLeft && closeBottom > closeRight)
	{
		closeTop = -closed * (closeTop/closeBottom);
		closeLeft = closed * (closeLeft/closeBottom);
		closeRight = -closed * (closeRight/closeBottom);
		closeBottom = closed;
	}
	else if(closeTop > closeLeft && closeTop > closeRight)
	{
		closeBottom = closed * (closeBottom/closeTop);
		closeLeft = closed * (closeLeft/closeTop);
		closeRight = -closed * (closeRight/closeTop);
		closeTop = -closed;
	}
	else if(closeLeft > closeRight)
	{
		closeBottom = closed * (closeBottom/closeLeft);
		closeTop = -closed * (closeTop/closeLeft);
		closeRight = -closed * (closeRight/closeLeft);
		closeLeft = closed;
	}
	else
	{
		closeBottom = closed * (closeBottom/closeRight);
		closeTop = -closed * (closeTop/closeRight);
		closeLeft = closed * (closeLeft/closeRight);
		closeRight = -closed;
	}
}

void Area::update(int ms)
{
	if(thereIsArea && areaIsClosing)
	{
		float closeBottom, closeTop, closeLeft, closeRight;
		getSpeedClose(ms, closeBottom, closeTop, closeLeft, closeRight);

		currentBottomArea += closeBottom;
		currentTopArea += closeTop;
		currentLeftArea += closeLeft;
		currentRightArea += closeRight;

		if( currentBottomArea >= futureBottomArea || currentTopArea <= futureTopArea || currentLeftArea >= futureLeftArea || currentRightArea <= futureRightArea )
		{
			areaIsClosing = false;
			tsArea = Time::currentMs();
			
			pastBottomArea = currentBottomArea;
			pastTopArea = currentTopArea;
			pastLeftArea = currentLeftArea;
			pastRightArea = currentRightArea;
			
			this->stepFutureArea();
		}
	}
	else if(thereIsArea && !areaIsClosing && ((Time::currentMs()-tsArea) > TIME_WAIT_BEFORE_TO_AREA_CLOSE) )
	{
		areaIsClosing = true;
		this->tsArea = Time::currentMs();
	}
}

bool Area::isClosing()
{
	return areaIsClosing;
}

void Area::get(bool &isClosing, int &msLeft, 
				vec2 &futureCenter, int &currentBottom, int &currentTop, int &currentLeft, int &currentRight,
			   int &futureBottom, int &futureTop, int &futureLeft, int &futureRight,
				float &speedCloseSecBottom, float &speedCloseSecTop, float &cspeedCloseSecLeft, float &speedCloseSecRight)
{
	isClosing = areaIsClosing;
	
	if(areaIsClosing)
		msLeft = 0;
	// Area already started to close
	else if(thereIsArea)
		msLeft = TIME_WAIT_BEFORE_TO_AREA_CLOSE - (Time::currentMs() - tsArea);
	// Area didnt start to close yet
	else
		msLeft = (START_TO_CLOSE_SEC*1000) - (Time::currentMs() - tsArea);

	futureCenter.x = futureCenterArea.x;
	futureCenter.y = futureCenterArea.y;
	currentBottom = currentBottomArea;
	currentTop = currentTopArea;
	currentLeft = currentLeftArea;
	currentRight = currentRightArea;
	futureBottom = futureBottomArea;
	futureTop = futureTopArea;
	futureLeft = futureLeftArea;
	futureRight = futureRightArea;
		
	speedCloseSecBottom = 0;
	speedCloseSecTop = 0;
	cspeedCloseSecLeft = 0;
	speedCloseSecRight = 0;
	if(areaIsClosing)
	{
		getSpeedClose(1000, speedCloseSecBottom, speedCloseSecTop, cspeedCloseSecLeft, speedCloseSecRight);
	}
}

bool Area::isOut(vec2 &pos)
{
	if(!thereIsArea)
		return false;
	
	if(pos.x < currentLeftArea)
		return true;
	if(pos.x > currentRightArea)
		return true;
	if(pos.y < currentBottomArea)
		return true;
	if(pos.y > currentTopArea)
		return true;
	return false;
}



void Area::initFutureArea()
{
	futureCenterArea.x = SIDE_MAP_METERS / 2.0f;
	futureCenterArea.y = SIDE_MAP_METERS / 2.0f;
	
	futureBottomArea = futureCenterArea.y - SIDE_MAP_METERS/4;
	futureTopArea = futureCenterArea.y + SIDE_MAP_METERS/4;
	futureLeftArea = futureCenterArea.x - SIDE_MAP_METERS/4;
	futureRightArea = futureCenterArea.x + SIDE_MAP_METERS/4;
	
	currentBottomArea = 0.0f;
	currentTopArea = SIDE_MAP_METERS;
	currentLeftArea = 0.0f;
	currentRightArea = SIDE_MAP_METERS;
	
	/*
	futureCenterArea.x = (((float)rand()) / ((float)(RAND_MAX))) * ((float)SIDE_MAP_METERS);
	futureCenterArea.y = (((float)rand()) / ((float)(RAND_MAX))) * ((float)SIDE_MAP_METERS);
	
	futureBottomArea = futureCenterArea.y - SIDE_MAP_METERS/2;
	futureTopArea = futureCenterArea.y + SIDE_MAP_METERS/2;
	futureLeftArea = futureCenterArea.x - SIDE_MAP_METERS/2;
	futureRightArea = futureCenterArea.x + SIDE_MAP_METERS/2;
	
	currentBottomArea = futureCenterArea.y - SIDE_MAP_METERS;
	currentTopArea = futureCenterArea.y + SIDE_MAP_METERS;
	currentLeftArea = futureCenterArea.x - SIDE_MAP_METERS;
	currentRightArea = futureCenterArea.x + SIDE_MAP_METERS;
	*/
}

void Area::stepFutureArea()
{
	float side = currentRightArea - currentLeftArea;
	
	futureBottomArea = this->futureCenterArea.y-side/4;
	futureTopArea = this->futureCenterArea.y+side/4;
	futureLeftArea = this->futureCenterArea.x-side/4;
	futureRightArea = this->futureCenterArea.x+side/4;
	
	/*
	float side = currentRightArea - currentLeftArea;
	
	// Always center inside map.
	do {
		float r = ((float)rand()) / ((float)(RAND_MAX));
		this->futureCenterArea.x = currentLeftArea + (0.5f+r)*side/2.0f;
		r = ((float)rand()) / ((float)(RAND_MAX));
		this->futureCenterArea.y = currentBottomArea + (0.5f+r)*side/2.0f;
	}while(this->futureCenterArea.x < 0 || this->futureCenterArea.x > SIDE_MAP_METERS ||
		this->futureCenterArea.y < 0 || this->futureCenterArea.y > SIDE_MAP_METERS );

	futureBottomArea = this->futureCenterArea.y-side/4;
	futureTopArea = this->futureCenterArea.y+side/4;
	futureLeftArea = this->futureCenterArea.x-side/4;
	futureRightArea = this->futureCenterArea.x+side/4;
	*/
}




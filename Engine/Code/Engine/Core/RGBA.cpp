#include "RGBA.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <string>

const RGBA RGBA::WHITE			= RGBA(255,255,255,255);
const RGBA RGBA::BLACK			= RGBA(0,0,0,255);
const RGBA RGBA::RED			= RGBA(255,0,0,255);
const RGBA RGBA::GREEN			= RGBA(0,255,0,255);
const RGBA RGBA::BLUE			= RGBA(0,0,255,255);
const RGBA RGBA::YELLOW			= RGBA(255,255,0,255);
const RGBA RGBA::MAGENTA		= RGBA(255,0,255,255);
const RGBA RGBA::CYAN			= RGBA(0,255,255,255);
const RGBA RGBA::LIGHTGRAY		= RGBA(200,200,200,255);
const RGBA RGBA::GRAY			= RGBA(128,128,128,255);
const RGBA RGBA::DARKGRAY		= RGBA(80,80,80,255);
const RGBA RGBA::NICEBLACK		= RGBA(0,32,32,255);
const RGBA RGBA::BLANCHEDALMOND	= RGBA(255,235,205,255);
const RGBA RGBA::BEEFEE			= RGBA(190,239,238,255);

float RGBA::s_currentRatioNumber = 0.f;





RGBA::RGBA()
{
	r = 255;
	g = 255;
	b = 255;
	a = 255;
}

RGBA::~RGBA()
{
}

RGBA::RGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

bool RGBA::operator==(const RGBA & compare) const
{
	if ((r == compare.r) && (g == compare.g) && (b == compare.b) && (a == compare.a)){
		return true;
	}
	return false; 
}

void RGBA::SetAsBytes(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	r = red;
	g = green;
	b = blue;
	a  = alpha;
}

void RGBA::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = (unsigned char) (255.f *normalizedRed);
	g = (unsigned char) (255.f *normalizedGreen);
	b = (unsigned char) (255.f *normalizedBlue);
	a = (unsigned char) (255.f *normalizedAlpha);
}

void RGBA::GetAsFloats(float & out_normalizedRed, float & out_normalizedGreen, float & out_normalizedBlue, float & out_normalizedAlpha) const
{
	out_normalizedRed = static_cast<float>(r)/255.f;
	out_normalizedGreen = static_cast<float>(g)/255.f;
	out_normalizedBlue =  static_cast<float>(b)/255.f;
	out_normalizedAlpha =  static_cast<float>(a)/255.f;
}

Vector4 RGBA::GetNormalized() const
{
	float normRed =		((float) r)/255.f;
	float normGreen =	((float) g)/255.f;
	float normBlue =	((float) b)/255.f;
	float normAlpha =	((float) a)/255.f;
	return Vector4(normRed, normGreen, normBlue, normAlpha);
}

void RGBA::ScaleRGB(float rgbScale)
{
	r=static_cast<unsigned char>(rgbScale*r);
	g=static_cast<unsigned char>(rgbScale*g);
	b=static_cast<unsigned char>(rgbScale*b);
	ClampRGB();
}

void RGBA::ScaleAlpha(float alphaScale)
{
	a=(unsigned char) (alphaScale* (float) a);

	ClampAlpha();
}

void RGBA::ClampRGB()
{
	if (r > 255){
		r = 255;
	}
	if (r < 0){
		r = 0;
	}

	if (g > 255){
		g = 255;
	}	
	if (g < 0){
		g = 0;
	}

	if (b > 255){
		b = 255;
	}	
	if (b < 0){
		b = 0;
	}


}


void RGBA::ClampAlpha()
{
	if (a > 255){
		a = 255;
	}	
	if (a < 0){
		a = 0;
	}
}

void RGBA::ClampVals()
{
	ClampRGB();
	ClampAlpha();
}

void RGBA::SetFromText(const char * text)
{
	std::string stringText = (std::string) text;
	Strip(stringText, '(');
	Strip(stringText, ')');
	unsigned int commaIndex1 = (int) stringText.find(',');
	unsigned int commaIndex2 = (int) stringText.find(',', commaIndex1+1);
	unsigned int commaIndex3 = (int) stringText.find(',', commaIndex2+1);
	if ((commaIndex1 != std::string::npos) && (commaIndex2 != std::string::npos)){
		std::string rString = stringText.substr(0, commaIndex1);
		std::string gString = stringText.substr(commaIndex1+1, commaIndex2);
		std::string bString = stringText.substr(commaIndex2+1, commaIndex3);
		r = (unsigned char) atoi(rString.c_str());
		g = (unsigned char) atoi(gString.c_str());
		b = (unsigned char) atoi(bString.c_str());
		if (commaIndex3 != std::string::npos){
			std::string aString = stringText.substr(commaIndex3+1);
			a = (unsigned char) atoi(aString.c_str());
		} else {
			a = (unsigned char) 255;
		}
	}
}

bool RGBA::CompareWithoutAlpha(RGBA compareVal)
{
	if (r == compareVal.r && g == compareVal.g && b == compareVal.b){
		return true;
	} 
	return false;
	
}

float RGBA::PercentAlpha() const
{
	return ((float) a / 255.f);
}

RGBA RGBA::GetColorWithAlpha(unsigned char alpha) const
{
	return RGBA(r,g,b,alpha);
}

RGBA RGBA::GetRandomRainbowColor()
{
	RGBA ret;
	float x = GetRandomFloatZeroToOne();
	float lerpVal = GetRandomFloatZeroToOne();
	if (IsInRange(x, 0.f, .17f)){
		ret = Interpolate(RGBA::RED, RGBA::YELLOW, lerpVal);
	} else if (IsInRange(x, .17f, .33f)){
		ret = Interpolate(RGBA::YELLOW, RGBA::GREEN, lerpVal);
	} else if (IsInRange(x, .33f, .50f)){
		ret = Interpolate(RGBA::GREEN, RGBA::CYAN, lerpVal);
	} else if (IsInRange(x, .50f, .67f)){
		ret = Interpolate(RGBA::CYAN, RGBA::BLUE, lerpVal);
	} else if (IsInRange(x, .67f, .84f)){
		ret = Interpolate(RGBA::BLUE, RGBA::MAGENTA, lerpVal);
	} else {
		ret = Interpolate(RGBA::MAGENTA, RGBA::RED, lerpVal);
	}
	return ret;
}

RGBA RGBA::GetRandomColor()
{
	unsigned char r = (unsigned char) GetRandomIntLessThan(256);
	unsigned char g = (unsigned char) GetRandomIntLessThan(256);
	unsigned char b = (unsigned char) GetRandomIntLessThan(256);
	return RGBA(r,g,b);
}

RGBA RGBA::GetRandomMutedColor()
{
	int r =  GetRandomIntInRange(55, 255);
	int g =  GetRandomIntInRange(55, 255);
	int b =  GetRandomIntInRange(55, 255);
	bool tooExtreme = true;
	int tries = 0;
	while (tooExtreme && tries < 10){
		int minVal = Min(r, g);
		minVal = Min(minVal, b);

		int maxVal = Max(r,g);
		maxVal = Max(maxVal, b);

		if ((maxVal - minVal) > 164){
			//determine which values to lerp
			if (r == minVal){
				if (g == maxVal){
					r = Interpolate(r, g, .05f);
					g = Interpolate(g, r, .05f);
					maxVal = g;
				} else {
					r = Interpolate(r, b, .05f);
					b = Interpolate(b, r, .05f);
					maxVal = b;
				}
				minVal = r;
				
			}

			if (g == minVal){
				if (r == maxVal){
					r = Interpolate(r, g, .05f);
					g = Interpolate(g, r, .05f);
					maxVal = r;
				} else {
					g = Interpolate(g, b, .05f);
					b = Interpolate(b, g, .05f);
					maxVal = b;
				}
				minVal = g;
			}

			if (b == minVal){
				if (r == maxVal){
					b = Interpolate(b, r, .05f);
					r = Interpolate(r, b, .05f);
					maxVal = b;
				} else {
					b = Interpolate(b, g, .05f);
					g = Interpolate(g, b, .05f);
					maxVal = g;
				}
				minVal = b;
			}
			tooExtreme = ((maxVal - minVal) > 164);
			tries++;
		} else {
			tooExtreme = false;
		}
	}
	
	return RGBA((unsigned char) r, (unsigned char) g, (unsigned char) b);
}

RGBA RGBA::GetRandomMixedColor(const RGBA & mixColor, float mixWeight)
{
	RGBA random = RGBA::GetRandomColor();
	return Interpolate(random, mixColor, mixWeight);
}

RGBA RGBA::GetRGBAFromHSV(float hue, float saturation, float value)
{
	int hue_int = (int) (hue*6.f);
	float f = hue*6.f - (float) hue_int;
	float p = value * (1.f - saturation);
	float q = value * (1.f - f*saturation);
	float t = value * (1.f - (1.f - f) * saturation);

	float r;
	float g;
	float b;

	switch (hue_int)
	{
	case (0):
		r = value;
		g = t;
		b = p;
		break;
	case(1):
		r = q;
		g = value;
		b = p;
		break;
	case (2):
		r = p;
		g = value;
		b = t;
		break;
	case(3):
		r = p;
		g = q;
		b = value;
		break;
	case(4):
		r = t;
		g = p;
		b = value;
		break;
	case(5):
		r = value;
		g = p;
		b = q;
		break;
	default:
		r = 1.f;
		g = 1.f;
		b = 1.f;
		break;
	}
	RGBA returnVal = RGBA();
	returnVal.SetAsFloats(r,g,b,1.f);
	return returnVal;
}

RGBA RGBA::GetRandomEarthTone()
{
	return GetRGBAFromHSV(GetRandomFloatInRange(0.04f, .33f), GetRandomFloatInRange(.45f, .8f), GetRandomFloatInRange(.15f, .5f));
}

RGBA RGBA::GetRandomPastelColor(float lightness)
{
	return GetRandomMixedColor(RGBA::WHITE, lightness);
}

RGBA RGBA::GetGoldenRatioColorSequence(float saturation, float value)
{
	if (RGBA::s_currentRatioNumber == 0.f){
		RGBA::s_currentRatioNumber = GetRandomFloatZeroToOne();
	} else {
		RGBA::s_currentRatioNumber += s_goldenRatioConjugate * .5f;
	}
	while (s_currentRatioNumber > 1.f){
		RGBA::s_currentRatioNumber -= 1.f;
	}
	
	return RGBA::GetRGBAFromHSV(RGBA::s_currentRatioNumber, saturation, value);

}

const RGBA Interpolate(const RGBA & start, const RGBA & end, float fractionTowardEnd)
{

	unsigned char r = Interpolate(start.r, end.r, fractionTowardEnd);
	unsigned char g = Interpolate(start.g, end.g, fractionTowardEnd);
	unsigned char b = Interpolate(start.b, end.b, fractionTowardEnd);
	unsigned char a = Interpolate(start.a, end.a, fractionTowardEnd);

	return RGBA(r,g,b,a);
}


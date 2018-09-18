#pragma once
#include "Engine/Math/Matrix44.hpp"



class RGBA{
public:
	RGBA();
	~RGBA();
	explicit RGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
	//explicit RGBA(float redZeroToOne, float greenZeroToOne, float blueZeroToOne, float alphaZeroToOne = 1.f);

	bool operator==( const RGBA& compare ) const;


	unsigned char r;		//red byte, 0-255
	unsigned char g;		//green byte, 0-255
	unsigned char b;		//blue byte, 0-255
	unsigned char a;		//alpha byte, 0-255, 0 = transparent, 255 = opaque


	void SetAsBytes(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

	void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha);

	void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;
	Vector4 GetNormalized() const;

	void ScaleRGB( float rgbScale);		//Scales (and clamps) RGB components (not A)

	void ScaleAlpha(float alphaScale);	//Scales (and clamps) Alpha, not RGB

	void ClampRGB();
	void ClampAlpha();
	void ClampVals();

	void SetFromText(const char* text);

	bool CompareWithoutAlpha(RGBA compareVal);
	float PercentAlpha() const;
	RGBA GetColorWithAlpha(unsigned char alpha) const;

	static RGBA GetRandomRainbowColor();
	static RGBA GetRandomColor();


	const static RGBA WHITE;
	const static RGBA BLACK;
	const static RGBA RED;
	const static RGBA GREEN;
	const static RGBA BLUE;
	const static RGBA YELLOW;
	const static RGBA MAGENTA;
	const static RGBA CYAN;
	const static RGBA LIGHTGRAY;
	const static RGBA GRAY;
	const static RGBA DARKGRAY;
	const static RGBA BLANCHEDALMOND;

};

const RGBA Interpolate( const RGBA& start, const RGBA& end, float fractionTowardEnd );


//static RGBA WHITE = RGBA(255,255,255,255);
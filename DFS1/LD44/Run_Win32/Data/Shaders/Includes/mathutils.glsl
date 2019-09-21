float RangeMap( float x, float d0, float d1, float r0, float r1 )
{
   return (x - d0) / (d1 - d0) * (r1 - r0) + r0;    
}

float smoothStart2(float t) {
	return t * t;
}

float SmoothStart3(float t) {
	return t * t * t;
}

float SmoothStop2(float t){
	float flipT = 1.f - t;
	return 1.f - (flipT * flipT);
}

float SmoothStop3(float t){
	float flipT = 1.f - t;
	return 1.f - (flipT * flipT * flipT);
}

float Crossfade(float a, float b, float t){
	return (a + (t * (b - a)));
}

float SmoothStep3(float t) {
	return Crossfade(SmoothStart3(t), SmoothStop3(t), t);
}
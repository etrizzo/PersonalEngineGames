#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"


class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(){};
	PerspectiveCamera(PerspectiveCamera* m_camera);
	float m_fovDegrees;		//needed later perhaps?

	void SetPerspectiveOrtho( float fovDegrees, float aspect, float nearZ, float farZ);


};
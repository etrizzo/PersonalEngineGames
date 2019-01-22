#include "Game/GameCommon.hpp"

class IsoSprite;

class IsoAnimation{
public:
	IsoAnimation(tinyxml2::XMLElement* isoAnimElement);

	IsoSprite* Evaluate (float time) ;
	bool IsFinished() const				{ return m_isFinished; }
	bool IsPlaying() const				{ return m_isPlaying; }

	bool m_isFinished = false;
	bool m_isPlaying = true;
	ePlaybackMode m_playbackMode = PLAYBACK_MODE_LOOP;
	float m_duration = 0.f;
	std::vector<IsoSprite*> m_frames;
	std::vector<float> m_frameDurations;

	static IsoAnimation* GetAnimation(std::string name);
	static std::map<std::string, IsoAnimation*> s_isoAnimations;
};

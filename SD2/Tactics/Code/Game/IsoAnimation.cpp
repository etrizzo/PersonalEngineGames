#include "IsoAnimation.hpp"
#include "Game/IsoSprite.hpp"

std::map<std::string, IsoAnimation*>	IsoAnimation::s_isoAnimations;

IsoAnimation::IsoAnimation(tinyxml2::XMLElement* isoAnimElement)
{
	m_duration = 0.f;
	for (tinyxml2::XMLElement* frameElem = isoAnimElement->FirstChildElement("frame"); frameElem != nullptr; frameElem = frameElem->NextSiblingElement("frame")){
		std::string src = ParseXmlAttribute(*frameElem, "src", "NO_ID");
		float duration = ParseXmlAttribute(*frameElem, "time", 1.f);
		IsoSprite* sprite = IsoSprite::GetIsoSprite(src);
		ASSERT_OR_DIE(sprite != nullptr, "IsoSprite " + src + " not found!");
		m_frames.push_back(sprite);
		m_frameDurations.push_back(duration);
		m_duration+=duration;
	}
	std::string loopString = ParseXmlAttribute(*isoAnimElement, "loop", "loop");
	std::string animID = ParseXmlAttribute(*isoAnimElement, "id", "NO_ID");
	if (loopString == "loop"){
		m_playbackMode = PLAYBACK_MODE_LOOP;
	} else if (loopString == "clamp"){
		m_playbackMode = PLAYBACK_MODE_PLAY_TO_END;
	}

	s_isoAnimations.insert(std::pair<std::string, IsoAnimation*>(animID, this));

}

IsoSprite * IsoAnimation::Evaluate(float time)
{
	
	if (m_playbackMode == PLAYBACK_MODE_LOOP){
		if (time >= m_duration){
			//int divide = int (time / m_duration);
			//time -= (m_duration * (float) divide);
			while (time >= m_duration){
				time -=(m_duration);
			}
		}
	} else if (m_playbackMode == PLAYBACK_MODE_PLAY_TO_END){
		if (time >= m_duration){
			m_isFinished = true;
			return m_frames[m_frames.size() - 1];
		}
	}

	float timeInAnim = 0.f;
	for (int i = 0; i < (int) m_frames.size(); i++){
		timeInAnim+=m_frameDurations[i];
		if (time <= timeInAnim){
			return m_frames[i];
		}
	}
	return nullptr;		//should never reach this point?
}

IsoAnimation * IsoAnimation::GetAnimation(std::string name)
{
	auto containsAnim = s_isoAnimations.find((std::string)name); 
	IsoAnimation* anim = nullptr;
	if (containsAnim != s_isoAnimations.end()){
		anim = containsAnim->second;
	}
	return anim;
}

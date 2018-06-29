#pragma once
#include "Engine/Core/XmlUtilities.hpp"
#include <vector>

//AUDIO
//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs

const std::string AUDIO_DIRECTORY = "Data/Audio/";

//-----------------------------------------------------------------------------------------------

class AudioSystem;

class AudioGroup{
public:
	AudioGroup(tinyxml2::XMLElement* audioGroupNode, AudioSystem* system);

	SoundID GetRandomFromGroup() const;

	std::string m_name;
	std::vector<SoundID> m_soundIDs;

	static std::map<std::string, AudioGroup*> s_audioGroups;
	static AudioGroup* GetAudioGroup(std::string groupName);
	static SoundID GetRandomSoundFromGroup(std::string groupName);
};
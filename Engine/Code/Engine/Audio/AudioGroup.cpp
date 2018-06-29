#include "AudioGroup.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Audio/AudioSystem.hpp"

std::map<std::string, AudioGroup*>	AudioGroup::s_audioGroups;

AudioGroup::AudioGroup(tinyxml2::XMLElement * audioGroupNode,  AudioSystem* system)
{
	m_name = ParseXmlAttribute(*audioGroupNode, "name", "NO_NAME");
	for (tinyxml2::XMLElement* soundNode = audioGroupNode->FirstChildElement("Sound"); soundNode!=NULL; soundNode = soundNode->NextSiblingElement("Sound")){
		std::string file = ParseXmlAttribute(*soundNode, "file", "NO_FILE");
		SoundID id = system->CreateOrGetSound(AUDIO_DIRECTORY + file);
		m_soundIDs.push_back(id);
	}
	s_audioGroups.insert(std::pair<std::string, AudioGroup*>(m_name, this));
}

SoundID AudioGroup::GetRandomFromGroup() const
{
	int index = GetRandomIntLessThan((int) m_soundIDs.size());
	return m_soundIDs[index];
}

AudioGroup * AudioGroup::GetAudioGroup(std::string groupName)
{
	auto contains = s_audioGroups.find((std::string)groupName); 
	AudioGroup* group = nullptr;
	if (contains != s_audioGroups.end()){
		group = contains->second;
	} 
	return group;
}

SoundID AudioGroup::GetRandomSoundFromGroup(std::string groupName)
{
	AudioGroup* group = GetAudioGroup(groupName);
	ASSERT_OR_DIE(group != nullptr, "No audio group called: " + groupName);

	return group->GetRandomFromGroup();
}

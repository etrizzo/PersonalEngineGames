#include "SpriteAnim.hpp"

//TODO: Fix this for incursion
SpriteAnim::SpriteAnim(const SpriteSheet* spriteSheet, float durationSeconds, ePlaybackMode playbackMode, int startSpriteIndex, int endSpriteIndex)
{
	m_elapsedSeconds = 0.f;
	m_isFinished = false;
	m_isPlaying = true;
	m_playForward = true;
	m_spriteSheet = spriteSheet;
	m_definition = nullptr;
	m_startIndex = startSpriteIndex;
	m_endIndex = endSpriteIndex;	
	m_playbackMode = playbackMode;
	m_duration = durationSeconds;
}

SpriteAnim::SpriteAnim(SpriteAnimDef * animDef)
{
	m_definition = animDef;
	m_elapsedSeconds = 0.F;
	m_isFinished = false;
	m_isPlaying = true;
	m_playForward = true;
	if (m_definition->m_chooseOneIndexInRange){
		m_randomIndex = m_definition->m_indices.GetRandomInRange();
	}
}

SpriteAnim::~SpriteAnim()
{
}

void SpriteAnim::Update(float deltaSeconds)
{
	if (m_isPlaying){
		m_elapsedSeconds+=deltaSeconds;
		if (m_definition != nullptr){
			if (m_elapsedSeconds > m_definition->GetDuration()){
				if (m_definition->m_playbackMode == PLAYBACK_MODE_PLAY_TO_END){
					m_isFinished = true;
				} else {
					Reset();
				}
			}
		} else {
			if (m_elapsedSeconds > m_duration){
				if (m_playbackMode == PLAYBACK_MODE_PLAY_TO_END){
					m_isFinished = true;
				} else {
					Reset();
				}
			}
		}
	}

}

AABB2 SpriteAnim::GetCurrentTexCoords() const
{
	if (m_definition != nullptr){
		if (m_definition->m_chooseOneIndexInRange){
			return m_definition->GetSpriteSheet()->GetTexCoordsForSpriteIndex(m_randomIndex);
		} else {
			int endFrameIndex = (int) m_definition->m_spriteIndexes.size()-1;

			
			float percentThroughAnim = m_elapsedSeconds / m_definition->GetDuration();
			int texCoordIndex;
			if (m_playForward){
				texCoordIndex = Interpolate(0, endFrameIndex, percentThroughAnim);
			} else {
				texCoordIndex = Interpolate(0, endFrameIndex, 1 - percentThroughAnim);
			}
			texCoordIndex = ClampInt(texCoordIndex, 0, endFrameIndex);
			return m_definition->GetSpriteSheet()->GetTexCoordsForSpriteIndex(m_definition->m_spriteIndexes[texCoordIndex]);
		}
	} else {
		float percentThroughAnim = m_elapsedSeconds / m_duration;
		int texCoordIndex;
		if (m_playForward){
			texCoordIndex = Interpolate(m_startIndex, m_endIndex, percentThroughAnim);
		} else {
			texCoordIndex = Interpolate(m_startIndex, m_endIndex, 1 - percentThroughAnim);
		}
		texCoordIndex = ClampInt(texCoordIndex, m_startIndex, m_endIndex);
		return m_spriteSheet->GetTexCoordsForSpriteIndex(texCoordIndex);
	}
}

AABB2 SpriteAnim::GetTexCoordsAtTime(float time) const
{
	if (m_definition != nullptr){
		int endFrameIndex = (int) m_definition->m_spriteIndexes.size()-1;

		float percentThroughAnim = time / m_definition->GetDuration();
		int texCoordIndex;
		if (m_playForward){
			texCoordIndex = Interpolate(0, endFrameIndex, percentThroughAnim);
		} else {
			texCoordIndex = Interpolate(0, endFrameIndex, 1 - percentThroughAnim);
		}
		texCoordIndex = ClampInt(texCoordIndex, 0, endFrameIndex);
		return m_definition->GetSpriteSheet()->GetTexCoordsForSpriteIndex(m_definition->m_spriteIndexes[texCoordIndex]);
	} else {
		return AABB2();
	}
}

 Texture * SpriteAnim::GetTexture() const
{
	 Texture* tex;
	if (m_definition != nullptr){
		tex = m_definition->GetSpriteSheet()->GetTexture();
	} else {
		tex = m_spriteSheet->GetTexture();
	}
	return tex;
}

void SpriteAnim::Pause()
{
	m_isPlaying = false;
}

void SpriteAnim::Resume()
{
	m_isPlaying = true;
}

void SpriteAnim::Reset()
{
	m_elapsedSeconds = 0.f;
	m_isFinished = false;

	if (m_definition != nullptr){
		if (m_definition->m_playbackMode == PLAYBACK_MODE_PINGPONG){
			if (m_playForward){
				m_playForward = false;
			} else {
				m_playForward = true;
			}
		}
	} else {
		if (m_playbackMode == PLAYBACK_MODE_PINGPONG){
			if (m_playForward){
				m_playForward = false;
			} else {
				m_playForward = true;
			}
		}
	}

}

float SpriteAnim::GetSecondsRemaining() const
{
	if (m_definition != nullptr){
		return m_definition->GetDuration() - m_elapsedSeconds;
	} else {
		return m_duration - m_elapsedSeconds;
	}
}

float SpriteAnim::GetFractionElapsed() const
{
	if (m_definition != nullptr){
		return m_elapsedSeconds/m_definition->GetDuration();
	} else {
		return m_elapsedSeconds/m_duration;
	}
}

float SpriteAnim::GetFractionRemaining() const
{
	return 1.f - GetFractionElapsed();
}

void SpriteAnim::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
}

void SpriteAnim::SetFractionElapsed(float fractionElapsed)
{
	if (m_definition != nullptr){
		m_elapsedSeconds = m_definition->GetDuration() * fractionElapsed;
	} else {
		m_elapsedSeconds = m_duration * fractionElapsed;
	}
}

std::string SpriteAnim::GetName()
{
	if (m_definition != nullptr){
		return m_definition->m_name;
	} else {
		return std::string();
	}
}



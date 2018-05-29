#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDef.hpp"



class SpriteAnim
{
public:
	SpriteAnim(const SpriteSheet* spriteSheet, float durationSeconds, ePlaybackMode playbackMode, int startSpriteIndex, int endSpriteIndex);
	SpriteAnim(SpriteAnimDef* animDef);
	~SpriteAnim();
	

	void Update(float deltaSeconds);
	AABB2 GetCurrentTexCoords() const;
	AABB2 GetTexCoordsAtTime(float time) const;
	const Texture* GetTexture() const;
	void Pause();
	void Resume();
	void Reset();
	bool IsFinished() const							{ return m_isFinished; }
	bool IsPlaying() const							{ return m_isPlaying; }
	float GetDurationSeconds() const				{ return m_definition->GetDuration(); }
	float GetSecondsElapsed() const					{ return m_elapsedSeconds; }
	SpriteAnimDef* GetDefinition() const		{return m_definition;}
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	void SetSecondsElapsed( float secondsElapsed );
	void SetFractionElapsed( float fractionElapsed );

	std::string GetName();

private:
	SpriteAnimDef* m_definition = nullptr;
	const SpriteSheet* m_spriteSheet = nullptr;
	float m_elapsedSeconds;
	float m_duration;
	ePlaybackMode m_playbackMode;
	int m_startIndex;
	int m_endIndex;
	bool m_isFinished;
	bool m_isPlaying;
	bool m_playForward;

};
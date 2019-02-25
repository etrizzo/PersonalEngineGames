#include "ClothingSet.hpp"
#include "Game/ClothingSetDefinition.hpp"

ClothingSet::ClothingSet()
{
	//m_currentTextures = std::vector<Texture*>();
	//m_defaultTextures = std::vector<Texture*>();
	//m_currentTextures.resize(NUM_RENDER_SLOTS);
	//m_defaultTextures.resize(NUM_RENDER_SLOTS);

	//m_currentTints = std::vector<RGBA>();
	//m_defaultTints = std::vector<RGBA>();
	//m_currentTints.resize(NUM_RENDER_SLOTS);
	//m_defaultTints.resize(NUM_RENDER_SLOTS);
	m_currentLayers = std::vector<ClothingLayer*>();
	m_defaultLayers = std::vector<ClothingLayer*>();
	m_currentLayers.resize(NUM_RENDER_SLOTS);
	m_defaultLayers.resize(NUM_RENDER_SLOTS);
	m_portraitLayers.resize(NUM_PORTRAIT_SLOTS);
}

void ClothingSet::InitLayer(RENDER_SLOT slot, ClothingLayer * layer)
{
	ClothingLayer* initLayer = nullptr;
	if (layer != nullptr){
		initLayer = new ClothingLayer(layer);
	}
	m_defaultLayers[slot] = initLayer;
	m_currentLayers[slot] = initLayer;
}

void ClothingSet::SetDefaultLayer(RENDER_SLOT slot)
{
	m_currentLayers[slot] = m_defaultLayers[slot];
}

void ClothingSet::SetLayer(RENDER_SLOT slot, ClothingLayer * layer)
{
	m_currentLayers[slot] = layer;
}

//void ClothingSet::InitTexture(RENDER_SLOT slot, Texture * texture, RGBA tint)
//{
//	m_defaultTextures[slot] = texture;
//	m_currentTextures[slot] = texture;
//	m_currentTints[slot] = tint;
//	m_defaultTints[slot] = tint;
//}
//
//
//void ClothingSet::SetDefaultTexture(RENDER_SLOT slot)
//{
//	m_currentTextures[slot] = m_defaultTextures[slot];
//}
//
//
//void ClothingSet::SetTexture(RENDER_SLOT slot, Texture * newTexture, RGBA tint)
//{
//	m_currentTextures[slot] = newTexture;
//	m_currentTints[slot] = tint;
//}

//void ClothingSet::SetTexture(RENDER_SLOT slot, ClothingSetDefinition* setDefinition, RGBA tint)
//{
//	SetTexture(slot, setDefinition->GetRandomOfType(slot), tint);
//}

ClothingLayer * ClothingSet::GetLayer(RENDER_SLOT slot) const
{
	return m_currentLayers[slot];
}

Texture * ClothingSet::GetTexture(RENDER_SLOT slot) const
{
	if (m_currentLayers[slot] != nullptr){
		return m_currentLayers[slot]->GetTexture();
	}
	return nullptr;
}

Texture * ClothingSet::GetTexture(int i) const
{
	return GetTexture((RENDER_SLOT) i);
}

RGBA ClothingSet::GetTint(RENDER_SLOT slot) const
{
	return m_currentLayers[slot]->GetTint();
}

RGBA ClothingSet::GetTint(int i) const
{
	return GetTint((RENDER_SLOT) i);
}

void ClothingSet::InitPortrait()
{
	InitPortraitBody();
	InitPortraitHair();
	InitPortraitFace();
	InitPortraitFeatures();
}

int ClothingSet::GetNumPortraitLayersForSlot(ePortraitSlot slot) const
{
	return m_portraitLayers[slot].size();
}

Texture * ClothingSet::GetPortraitTexture(ePortraitSlot slot, int index) const
{
	if ((int) m_portraitLayers[slot].size() > index){
		return m_portraitLayers[slot][index]->m_texture;
	} else {
		return nullptr;
	}
	
}

void ClothingSet::InitPortraitBody()
{
	//set the face layer
	m_portraitSkinTone = m_defaultLayers[BODY_SLOT]->m_portraitTint;
	PortraitLayer* faceLayer = new PortraitLayer(FACE_PORTRAIT, m_portraitSkinTone);
	faceLayer->SetUVsFromSpriteCoords(m_portraitDef->GetRandomFaceCoords());
	m_portraitLayers[FACE_PORTRAIT].push_back(faceLayer);

	if (m_defaultLayers[EARS_SLOT] != nullptr){
		m_earPortraitCoords = m_defaultLayers[EARS_SLOT]->m_portraitCoords;
		PortraitLayer* earLayer = new PortraitLayer(EAR_PORTRAIT, m_portraitSkinTone);
		earLayer->SetUVsFromSpriteCoords(m_earPortraitCoords);
		m_portraitLayers[EAR_PORTRAIT].push_back(earLayer);
	}
}

void ClothingSet::InitPortraitHair()
{
	m_hairTint = m_defaultLayers[HEAD_SLOT]->m_portraitTint;
	m_hairPortraitCoords = m_defaultLayers[HEAD_SLOT]->m_portraitCoords;
	//get the front layer of the hair
	PortraitLayer* hairFrontLayer = new PortraitLayer(HAIR_FRONT_PORTRAIT, m_hairTint);
	hairFrontLayer->SetUVsFromSpriteCoords(m_hairPortraitCoords);
	m_portraitLayers[HAIR_FRONT_PORTRAIT].push_back(hairFrontLayer);

	//get the back layer of the hair
	PortraitLayer* hairBackLayer = new PortraitLayer(HAIR_BACK_PORTRAIT, m_hairTint);
	hairBackLayer->SetUVsFromSpriteCoords(m_hairPortraitCoords + IntVector2(0, 1));
	m_portraitLayers[HAIR_BACK_PORTRAIT].push_back(hairBackLayer);

}

void ClothingSet::InitPortraitFace()
{
	//init nose
	PortraitLayer* noseLayer = new PortraitLayer(NOSE_PORTRAIT);
	noseLayer->SetUVsFromSpriteCoords(m_portraitDef->GetRandomNoseCoords());
	m_portraitLayers[NOSE_PORTRAIT].push_back(noseLayer);

	//init brows
	PortraitLayer* browLayer = new PortraitLayer(BROWS_PORTRAIT);
	browLayer->SetUVsFromSpriteCoords(m_portraitDef->GetRandomBrowCoords());
	m_portraitLayers[BROWS_PORTRAIT].push_back(browLayer);

	//init mouth
	InitPortraitMouth();


	//init eyes
	InitPortraitEyes();
}

void ClothingSet::InitPortraitFeatures()
{
	int numFeatures = m_portraitDef->m_numFeaturesPossible.GetRandomInRange();
	for (int i = 0; i < numFeatures; i++){
		PortraitLayer* featureLayer = new PortraitLayer(FEATURES_PORTRAIT);
		featureLayer->SetUVsFromSpriteCoords(m_portraitDef->GetRandomFeatureCoords());
		m_portraitLayers[FEATURES_PORTRAIT].push_back(featureLayer);
	}
}

void ClothingSet::InitPortraitMouth()
{
	int mouthIndex = GetRandomIntLessThan(m_portraitDef->m_mouthLines.size());
	sPortraitPieceDefinition* mouthLines = m_portraitDef->m_mouthLines[mouthIndex];
	PortraitLayer* mouthLinesLayer = new PortraitLayer(MOUTH_PORTRAIT);
	mouthLinesLayer->SetUVsFromSpriteCoords(mouthLines->m_spriteCoords);
	//if you pass the lip chance, set up the lip color layer
	if (CheckRandomChance(m_portraitDef->m_lipColorChance)){
		sPortraitPieceDefinition* lipMask = m_portraitDef->m_lipMasks[mouthIndex];
		RGBA color = lipMask->GetColorForInstance();
		PortraitLayer* lipLayer = new PortraitLayer(MOUTH_PORTRAIT, color);
		lipLayer->SetUVsFromSpriteCoords(lipMask->m_spriteCoords);
		m_portraitLayers[MOUTH_PORTRAIT].push_back(lipLayer);
	}
	m_portraitLayers[MOUTH_PORTRAIT].push_back(mouthLinesLayer);
}

void ClothingSet::InitPortraitEyes()
{
	int eyeIndex = GetRandomIntLessThan(m_portraitDef->m_eyeWhites.size());
	sPortraitPieceDefinition* eyeWhite = m_portraitDef->m_eyeWhites[eyeIndex];
	sPortraitPieceDefinition* eyePupil = m_portraitDef->m_eyePupils[eyeIndex];

	PortraitLayer* whiteLayer = new PortraitLayer(EYES_PORTRAIT);
	whiteLayer->SetUVsFromSpriteCoords(eyeWhite->m_spriteCoords);
	RGBA eyeColor = eyePupil->GetColorForInstance();
	PortraitLayer* pupilLayer = new PortraitLayer(EYES_PORTRAIT, eyeColor);
	pupilLayer->SetUVsFromSpriteCoords(eyePupil->m_spriteCoords);

	m_portraitLayers[EYES_PORTRAIT].push_back(whiteLayer);
	m_portraitLayers[EYES_PORTRAIT].push_back(pupilLayer);
}

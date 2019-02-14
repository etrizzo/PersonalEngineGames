#pragma once
#include "GameCommon.hpp"
#include "Game/TagPair.hpp"

class TagSet
{
public:
	TagSet(){ m_tags = std::vector<TagPair>();};
	TagSet(const std::string& commaSeparatedTagNames);
	TagSet( const TagSet& copy);


	int GetNumTagSet() const;
	std::string GetTagsAsString() const;
	void SetOrRemoveTags( const std::string& commaSeparatedTagNames ); // "blessed,!poisoned"
	bool HasTags( const std::string& commaSeparatedTagNames ); // "good,!cursed"
	void SetTag( const std::string& tagName );
	void SetTagWithValue( const std::string& tagName, const std::string& value, std::string type);
	void RemoveTag( const std::string& tagName );
	bool HasTag( const std::string& tagName );
	bool HasTag(TagPair tag);
	bool HasTagWithValue(const std::string& tagName, const std::string& value);
	bool ContainsTagWithAnyValue(const std::string& tagName, const std::string & tagType);

protected:
	std::vector<TagPair> m_tags;
};

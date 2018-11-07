#pragma once
#include "GameCommon.hpp"
#include "Game/TagPair.hpp"

class Tags
{
public:
	Tags(){ m_tags = std::vector<TagPair>();};
	Tags(const std::string& commaSeparatedTagNames);
	Tags( const Tags& copy);
	~Tags();

	int GetNumTags() const;
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

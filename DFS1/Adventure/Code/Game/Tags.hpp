#include "GameCommon.hpp"

class Tags
{
public:
	Tags(){ m_tags = Strings();};
	Tags(const std::string& commaSeparatedTagNames);
	~Tags();

	int GetNumTags() const;
	std::string GetTagsAsString() const;
	void SetOrRemoveTags( const std::string& commaSeparatedTagNames ); // "blessed,!poisoned"
	bool HasTags( const std::string& commaSeparatedTagNames ); // "good,!cursed"
	void SetTag( const std::string& tagName );
	void RemoveTag( const std::string& tagName );
	bool HasTag( const std::string& tagName );

protected:
	Strings m_tags;
};

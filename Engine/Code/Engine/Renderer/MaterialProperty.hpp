#pragma once
#include "Engine/Core/EngineCommon.hpp"

class MaterialProperty
{
public:
	//MaterialProperty(){};
	explicit MaterialProperty(std::string name);
	// need a name, a type, and a value
	// need to know how to bind itself
	//one option would be a pure virtual, and several subclasses for each type
public:
	virtual void Bind(unsigned int programHandle) = 0;

	std::string m_name;
	//type/value determined in subclasses

};

//and then a buncha subclasses for each type
class MaterialPropertyFloat : public MaterialProperty
{
public:
	explicit MaterialPropertyFloat(std::string name, float val);
	virtual void Bind(unsigned int programHandle) override;
	float m_value;
};


class MaterialPropertyRGBA : public MaterialProperty
{
public:
	explicit MaterialPropertyRGBA(std::string name, RGBA val);
	virtual void Bind(unsigned int programHandle) override;
	RGBA m_value;
};

class MaterialPropertyVec2 : public MaterialProperty
{
public:
	explicit MaterialPropertyVec2(std::string name, Vector2 val);
	virtual void Bind(unsigned int programHandle) override;
	Vector2 m_value;

};
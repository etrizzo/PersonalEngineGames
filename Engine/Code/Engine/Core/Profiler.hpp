#pragma once
#include "Engine/Core/Time.hpp"


class ProfileLogScoped
{
public:
	ProfileLogScoped( const char* tag ); 
	~ProfileLogScoped(); 

public:
	uint64_t m_hpc_start; 
	char const *tag;        
};
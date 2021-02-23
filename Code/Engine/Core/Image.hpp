#pragma once

#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"

struct Rgba8;

class Image
{
public:
	Image( const char* imageFilePath );

	const std::string& GetImageFilePath() const;
	IntVec2		       GetDimensions() const;
	Rgba8		       GetTexelColor( int texelX, int texelY ) const;
	Rgba8		       GetTexelColor( const IntVec2& texelCoords ) const;

	void               SetTexelColor( int texelX, int texelY, const Rgba8& newColor );
	void               SetTexelColor( const IntVec2& texelCoords, const Rgba8& newColor );
	void               RotateByNumber( int rotationNumber = 0 );
	void               MirrorByXAxis();

private:
	IntVec2            m_dimensions;
	std::string        m_imageFilePath;	
	std::vector<Rgba8> m_rgbaTexels;
};
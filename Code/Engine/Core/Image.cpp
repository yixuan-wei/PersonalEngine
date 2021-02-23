#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "ThirdParty/stb/stb_image.h"

//////////////////////////////////////////////////////////////////////////
Image::Image( const char* imageFilePath )
{
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	//stbi_set_flip_vertically_on_load( 1 ); // We prefer m_uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

	m_imageFilePath = imageFilePath;
	m_dimensions = IntVec2( imageTexelSizeX, imageTexelSizeY );

	int bitNum = imageTexelSizeX*imageTexelSizeY*numComponents;
	int texelCounter = 0;
	for( int bitIdx = 0; bitIdx < bitNum; bitIdx++ )
	{
		texelCounter++;
		if( texelCounter == numComponents )
		{
			Rgba8 texel;
			if( numComponents == 3 )
			{
				texel = Rgba8( imageData[bitIdx - 2], imageData[bitIdx - 1], imageData[bitIdx] );
			}
			else//num components should be 4
			{
				texel = Rgba8( imageData[bitIdx - 3], imageData[bitIdx - 2], imageData[bitIdx - 1], imageData[bitIdx] );
			}
			m_rgbaTexels.push_back( texel );
			texelCounter = 0;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}


//////////////////////////////////////////////////////////////////////////
IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}


//////////////////////////////////////////////////////////////////////////
Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
	int index = m_dimensions.x * texelY + texelX;
	return m_rgbaTexels[index];
}

//////////////////////////////////////////////////////////////////////////
Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
	int index = m_dimensions.x * texelCoords.y + texelCoords.x;
	return m_rgbaTexels[index];
}

//////////////////////////////////////////////////////////////////////////
void Image::SetTexelColor( int texelX, int texelY, const Rgba8& newColor )
{
	int index = m_dimensions.x * texelY + texelX;
	m_rgbaTexels[index] = newColor;
}

//////////////////////////////////////////////////////////////////////////
void Image::SetTexelColor( const IntVec2& texelCoords, const Rgba8& newColor )
{
	int index = m_dimensions.x * texelCoords.y + texelCoords.x;
	m_rgbaTexels[index] = newColor;
}

//////////////////////////////////////////////////////////////////////////
void Image::RotateByNumber( int rotationNumber )
{
	std::vector<Rgba8> oldTexels = m_rgbaTexels;
	switch( rotationNumber )
	{
		case 1://rotate 90 degrees anti-clockwise
		{			
			size_t xSize = m_dimensions.x;
			size_t ySize = m_dimensions.y;
			for( int y = 0; y < ySize; y++ )
			{
				size_t newXPos = ySize - y - 1;
				size_t oldYPos = y * xSize;
				for( int x = 0; x < xSize; x++ )
				{
					m_rgbaTexels[newXPos + x * ySize] = oldTexels[oldYPos + x];
				}
			}
			//change dimension
			m_dimensions.x = (int)ySize;
			m_dimensions.y = (int)xSize;
			break;
		}
		case 2://rotate 180 degrees
		{
			size_t xSize = m_dimensions.x;
			size_t ySize = m_dimensions.y;
			for( int y = 0; y < ySize; y++ )
			{
				size_t oldYPos = y * xSize;
				size_t newPos = (ySize-y - 1) * xSize + xSize-1;
				for( int x = 0; x < xSize; x++ )
				{
					m_rgbaTexels[-x + newPos] = oldTexels[oldYPos + x];
				}
			}
			break;
		}
		case 3: //rotate 270 degrees anti-clockwise
		{
			size_t xSize = m_dimensions.x;
			size_t ySize = m_dimensions.y;
			for( int x = 0; x < xSize; x++ )
			{
				size_t newPos = (xSize - x - 1) * ySize;
				for( int y = 0; y < ySize; y++ )
				{
					m_rgbaTexels[y + newPos] = oldTexels[y * xSize + x];
				}
			}
			//change dimension
			m_dimensions.x = (int)ySize;
			m_dimensions.y = (int)xSize;
			break;
		}
		default:
		{
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Image::MirrorByXAxis()
{
	size_t xSize = m_dimensions.x;
	size_t ySize = m_dimensions.y;
	std::vector<Rgba8> oldTexels = m_rgbaTexels;
	for( int y = 0; y < ySize; y++ )
	{
		size_t yPos = y * xSize;
		for( int x = 0; x < xSize; x++ )
		{
			m_rgbaTexels[yPos + xSize - 1 - x] = oldTexels[yPos + x];
		}
	}
}

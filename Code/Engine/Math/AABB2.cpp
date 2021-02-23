#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string>

//////////////////////////////////////////////////////////////////////////
AABB2::AABB2( const AABB2& copyFrom )
	:mins(copyFrom.mins)
	,maxs(copyFrom.maxs)
{
}

//////////////////////////////////////////////////////////////////////////
AABB2::AABB2( const Vec2& inMins, const Vec2& inMaxs )
	:mins(inMins)
	,maxs(inMaxs)
{
}

//////////////////////////////////////////////////////////////////////////
AABB2::AABB2( float minX, float minY, float maxX, float maxY )
	:mins(Vec2(minX,minY))
	,maxs(Vec2(maxX,maxY))
{
}

//////////////////////////////////////////////////////////////////////////
void AABB2::SetFromText( const char* text )
{
	if (text == nullptr) {
		return;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), ',' );
	if (subStrings.size() != 4) {
		g_theConsole->PrintError(Stringf("AABB2 can't construct from improper string \"%s\"", text) );
		return;
	}

	mins.x = StringConvert(subStrings[0].c_str(), 0.f);
	mins.y = StringConvert(subStrings[1].c_str(), 0.f);
	maxs.x = StringConvert(subStrings[2].c_str(), 0.f);
	maxs.y = StringConvert(subStrings[3].c_str(), 0.f);
}

//////////////////////////////////////////////////////////////////////////
bool AABB2::IsBoundsOverlap(AABB2 const& bounds) const
{
	return (maxs.x >= bounds.mins.x && mins.x <= bounds.maxs.x&&
		maxs.y >= bounds.mins.y && mins.y<=bounds.maxs.y);
}

//////////////////////////////////////////////////////////////////////////
bool AABB2::IsBoundsInside(AABB2 const& bounds) const
{
	return bounds.mins.x>=mins.x && bounds.mins.y>=mins.y && bounds.maxs.x<=maxs.x && bounds.maxs.y<=maxs.y;
}

//////////////////////////////////////////////////////////////////////////
bool AABB2::IsPointInside( const Vec2& point ) const
{
	if( point.x<maxs.x && point.x>mins.x && point.y<maxs.y && point.y>mins.y )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
Vec2 AABB2::GetCenter() const
{
	return Vec2( (mins.x + maxs.x) * .5f, (mins.y + maxs.y) * .5f );
}

//////////////////////////////////////////////////////////////////////////
Vec2 AABB2::GetDimensions() const
{
	return Vec2( maxs.x - mins.x, maxs.y - mins.y );
}

//////////////////////////////////////////////////////////////////////////
Vec2 AABB2::GetNearestPoint( const Vec2 referencePos ) const
{
	float x = Clamp( referencePos.x, mins.x, maxs.x );
	float y = Clamp( referencePos.y, mins.y, maxs.y );
	return Vec2( x, y );
}

//////////////////////////////////////////////////////////////////////////
Vec2 AABB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 dimension = GetDimensions();
	return Vec2( mins.x + dimension.x * uvCoordsZeroToOne.x, mins.y + dimension.y * uvCoordsZeroToOne.y );
}

//////////////////////////////////////////////////////////////////////////
Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	float x = GetFractionInRange( mins.x, maxs.x, point.x );
	float y = GetFractionInRange( mins.y, maxs.y, point.y );
	return Vec2( x, y );
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const
{
	Vec2 curDim = GetDimensions();
	Vec2 boxMins = (curDim - dimensions) * alignment + mins;
	Vec2 boxMaxs = boxMins+dimensions;
	return AABB2( boxMins, boxMaxs );
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::GetBoxAtRight( float fractionOfWidth, float additionalWidth /*= 0.f */ )const
{
	float width = maxs.x - mins.x;
	float boxWidth = width * fractionOfWidth + additionalWidth;
	if( boxWidth >= width )
	{
		g_theConsole->PrintString( Rgba8::RED, Stringf( "AABB2::GetBoxAtRight has longer box width than AABB2 itself" ) );
		return *this;
	}

	return AABB2( Vec2( maxs.x - boxWidth, mins.y ), maxs );
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::GetBoxAtLeft( float fractionOfWidth, float additionalWidth /*= 0.f */ )const
{
	float width = maxs.x - mins.x;
	float boxWidth = width * fractionOfWidth + additionalWidth;
	if( boxWidth >= width )
	{
		g_theConsole->PrintString( Rgba8::RED, Stringf( "AABB2::GetBoxAtLeft has longer box width than AABB2 itself" ) );
		return *this;
	}

	return AABB2( mins, Vec2( mins.x + boxWidth, maxs.y ) );
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::GetBoxAtTop( float fractionOfHeight, float additionalHeight /*= 0.f */ )const
{
	float height = maxs.y - mins.y;
	float boxHeight = height * fractionOfHeight + additionalHeight;
	if( boxHeight >= height )
	{
		g_theConsole->PrintString( Rgba8::RED, Stringf( "AABB2::GetBoxAtTop has longer box height than AABB2 itself" ) );
		return *this;
	}

	return AABB2( Vec2( mins.x, maxs.y - boxHeight ), maxs );
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::GetBoxAtBottom( float fractionOfHeight, float additionalHeight /*= 0.f */ )const
{
	float height = maxs.y - mins.y;
	float boxHeight = height * fractionOfHeight + additionalHeight;
	if( boxHeight >= height )
	{
		g_theConsole->PrintString( Rgba8::RED, Stringf( "AABB2::GetBoxAtBottom has longer box height than AABB2 itself" ) );
		return *this;
	}

	return AABB2( mins, Vec2( maxs.x, mins.y + boxHeight ) );
}

//////////////////////////////////////////////////////////////////////////
float AABB2::GetOutterRadius() const
{
	Vec2 dim = GetDimensions();
	return dim.GetLength() * .5f;
}

//////////////////////////////////////////////////////////////////////////
float AABB2::GetInnerRadius() const
{
	Vec2 dim = GetDimensions();
	float minLength = dim.x > dim.y ? dim.y : dim.x;
	return minLength * .5f;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	out_fourPoints[0] = mins;
	out_fourPoints[1] = Vec2( maxs.x, mins.y );
	out_fourPoints[2] = maxs;
	out_fourPoints[3] = Vec2( mins.x, maxs.y );
}

//////////////////////////////////////////////////////////////////////////
void AABB2::Translate( const Vec2& translation )
{
	mins += translation;
	maxs += translation;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::SetCenter( const Vec2& newCenter )
{
	Vec2 halfDimension = GetDimensions()*.5f;
	mins.x = newCenter.x - halfDimension.x;
	mins.y = newCenter.y - halfDimension.y;
	maxs.x = newCenter.x + halfDimension.x;
	maxs.y = newCenter.y + halfDimension.y;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::SetDimensions( const Vec2& newDimension )
{
	Vec2 center = GetCenter();
	Vec2 halfDimension = newDimension * .5f;
	mins.x = center.x - halfDimension.x;
	mins.y = center.y - halfDimension.y;
	maxs.x = center.x + halfDimension.x;
	maxs.y = center.y + halfDimension.y;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::StretchToIncludePoint( const Vec2& point )
{
	if( point.x > maxs.x )
	{
		maxs.x = point.x;
	}
	else if( point.x < mins.x )
	{
		mins.x = point.x;
	}
	if( point.y > maxs.y )
	{
		maxs.y = point.y;
	}
	else if( point.y < mins.y )
	{
		mins.y = point.y;
	}
}

//////////////////////////////////////////////////////////////////////////
void AABB2::StretchToIncludeBounds(AABB2 const& bounds)
{
	mins.x = MinFloat(mins.x, bounds.mins.x);
	maxs.x = MaxFloat(maxs.x, bounds.maxs.x);
	mins.y = MinFloat(mins.y, bounds.mins.y);
	maxs.y = MaxFloat(maxs.y, bounds.maxs.y);
}

//////////////////////////////////////////////////////////////////////////
void AABB2::FitWithinBounds( const AABB2& bounds )
{
	if( mins.x < bounds.mins.x )
		Translate( Vec2( bounds.mins.x - mins.x, 0.f ) );
	if( maxs.x > bounds.maxs.x )
		Translate( Vec2( bounds.maxs.x - maxs.x, 0.f ) );
	if( mins.y < bounds.mins.y )
		Translate( Vec2( 0.f, bounds.mins.y - mins.y ) );
	if( maxs.y > bounds.maxs.y )
		Translate( Vec2( 0.f, bounds.maxs.y - maxs.y) );
}

//////////////////////////////////////////////////////////////////////////
void AABB2::FitInBoundsAndResize( const AABB2& bounds )
{
	Vec2 scale = GetDimensions();
	const Vec2 inScale = bounds.GetDimensions();
	if( scale.y == 0.f || inScale.y == 0.f )
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "AABB2::FitInBoundsAndResize: one scale's y is 0, return" ) );
		return;
	}

	SetCenter( bounds.GetCenter() );
	if( scale.x / scale.y < inScale.x / inScale.y )//resize according to x size
	{
		if( inScale.x > scale.x )//should resize
		{
			float minYLength = scale.y;
			if( scale.x == 0.f )
			{
				if( scale.y < inScale.y )
				{
					minYLength = inScale.y;
				}
			}
			else
			{
				minYLength = (scale.y / scale.x) * inScale.x;
			}
			SetDimensions( Vec2( inScale.x, minYLength ) );
		}
	}
	else//resize according to y size
	{
		if( inScale.y > scale.y )
		{
			float minXLength = scale.x;
			if( scale.y == 0.f )
			{
				if( scale.x < inScale.x )
				{
					minXLength = inScale.x;
				}
			}
			else
			{
				minXLength = (scale.x / scale.y) * inScale.y;
			}
			SetDimensions( Vec2( minXLength,inScale.y ) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void AABB2::AlignWithinBox( const AABB2& box, const Vec2& alignment )
{
	Vec2 curDim = GetDimensions();
	Vec2 boxDim = box.GetDimensions();
	Vec2 deltaDim = boxDim - curDim;
	mins = deltaDim * alignment + box.mins;
	maxs = mins + curDim;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::FitTightlyWithinBox( const AABB2& box, const Vec2& alignment /*= ALIGN_CENTERED */ )
{
	Vec2 dim = GetDimensions();
	Vec2 boxDim = box.GetDimensions();
	if( dim.y == 0.f || boxDim.y == 0.f ||dim.x==0.f||boxDim.x==0.f)//simple filter out
		return;

	float aspect = dim.x / dim.y;
	float boxAspect = boxDim.x / boxDim.y;
	Vec2 newDim = dim;
	if( aspect > boxAspect )//compress according to x
	{		
		if( dim.x > boxDim.x )
		{
			newDim = boxDim.x / dim.x * dim;
			SetDimensions( newDim );
		}		
	}
	else//compress according to y
	{
		if( dim.y > boxDim.y )
		{
			newDim = boxDim.y / dim.y * dim;
			SetDimensions( newDim );
		}
	}
	mins = (boxDim - newDim) * alignment + box.mins;
	maxs = mins + newDim;
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::ChopBoxOffRight( float fractionOfWidth, float additionalWidth /*= 0.f */ )
{
	AABB2 result = GetBoxAtRight( fractionOfWidth, additionalWidth );
	maxs.x = result.mins.x;
	return result;
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::ChopBoxOffLeft( float fractionOfWidth, float additionalWidth /*= 0.f */ )
{
	AABB2 result = GetBoxAtLeft( fractionOfWidth, additionalWidth );
	mins.x = result.maxs.x;
	return result;
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::ChopBoxOffTop( float fractionOfHeight, float additionalHeight /*= 0.f */ )
{
	AABB2 result = GetBoxAtTop( fractionOfHeight, additionalHeight );
	maxs.y = result.mins.y;
	return result;
}

//////////////////////////////////////////////////////////////////////////
AABB2 AABB2::ChopBoxOffBottom( float fractionOfHeight, float additionalHeight /*= 0.f */ )
{
	AABB2 result = GetBoxAtBottom( fractionOfHeight, additionalHeight );
	mins.y = result.maxs.y;
	return result;
}

//////////////////////////////////////////////////////////////////////////
bool AABB2::operator!=(const AABB2& compareWith) const
{
	return mins!=compareWith.mins || maxs!=compareWith.maxs;
}

//////////////////////////////////////////////////////////////////////////
void AABB2::operator=( const AABB2& assignFrom )
{
	mins = assignFrom.mins;
	maxs = assignFrom.maxs;
}

//////////////////////////////////////////////////////////////////////////
bool AABB2::operator==( const AABB2& compareWith ) const
{
	if( mins == compareWith.mins && maxs == compareWith.maxs )
		return true;
	else 
		return false;
}

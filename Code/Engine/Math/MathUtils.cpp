#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <cmath>

//////////////////////////////////////////////////////////////////////////
Vec3 Power(Vec3 const& base, float index)
{
	return Vec3(std::powf(base.x, index), std::powf(base.y, index), std::powf(base.z, index));
}

//////////////////////////////////////////////////////////////////////////
float ConvertDegreesToRadians( float degrees )
{
	return degrees * (fPI/180.f);
}

//////////////////////////////////////////////////////////////////////////
float ConvertRadiansToDegrees( float radians )
{
	return radians * (180.0f/fPI);
}

//////////////////////////////////////////////////////////////////////////
float CosDegrees( float degrees )
{
	return std::cosf(ConvertDegreesToRadians(degrees));
}

//////////////////////////////////////////////////////////////////////////
float SinDegrees( float degrees )
{
	return std::sinf(ConvertDegreesToRadians(degrees));
}

//////////////////////////////////////////////////////////////////////////
float TanDegrees(float degrees)
{
	return std::tanf(ConvertDegreesToRadians(degrees));
}

//////////////////////////////////////////////////////////////////////////
float Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees(std::atan2f(y,x));
}

//////////////////////////////////////////////////////////////////////////
float GetSmallestSameDegrees( float degrees )
{
	while( degrees > 360.f )
		degrees -= 360.f;

	while( degrees < -360.f )
		degrees += 360.f;

	if( degrees > 180.f )
	{
		degrees = degrees - 360.f;
	}
	else if( degrees < -180.f )
	{
		degrees = 360.f + degrees;
	}

	return degrees;
}

//////////////////////////////////////////////////////////////////////////
float GetShortestAngularDisplacement( float fromDegrees, float toDegrees )
{
	float rawDisplacement = toDegrees - fromDegrees;
	rawDisplacement = GetSmallestSameDegrees( rawDisplacement );
	return rawDisplacement;
}

//////////////////////////////////////////////////////////////////////////
float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	float displacement = GetShortestAngularDisplacement( currentDegrees, goalDegrees );

	if( displacement <= maxDeltaDegrees && displacement >= -maxDeltaDegrees )
		return goalDegrees;
	else if( displacement > maxDeltaDegrees )
		return currentDegrees+maxDeltaDegrees;
	else 
		return currentDegrees-maxDeltaDegrees;
}

//////////////////////////////////////////////////////////////////////////
float GetDistance2D( const Vec2& firstVec2, const Vec2& secondVec2 )
{
	return std::sqrtf( GetDistanceSquared2D( firstVec2, secondVec2 ) );
}

//////////////////////////////////////////////////////////////////////////
float GetDistanceSquared2D( const Vec2& firstVec2, const Vec2& secondVec2 )
{
	Vec2 result = firstVec2 - secondVec2;
	return result.x*result.x + result.y*result.y;
}

//////////////////////////////////////////////////////////////////////////
float GetDistance3D( const Vec3& firstVec3, const Vec3& secondVec3 )
{
	return std::sqrtf( GetDistanceSquared3D( firstVec3, secondVec3 ) );
}

//////////////////////////////////////////////////////////////////////////
float GetDistanceSquared3D( const Vec3& firstVec3, const Vec3& secondVec3 )
{
	Vec3 result = firstVec3-secondVec3;
	return result.x*result.x + result.y*result.y + result.z*result.z;
}

//////////////////////////////////////////////////////////////////////////
float GetDistanceXY3D( const Vec3& firstVec3, const Vec3& secondVec3 )
{
	return std::sqrtf( GetDistanceXYSquared3D( firstVec3, secondVec3 ) );
}

//////////////////////////////////////////////////////////////////////////
float GetDistanceXYSquared3D( const Vec3& firstVec3, const Vec3& secondVec3 )
{
	Vec3 result = firstVec3 - secondVec3;
	return result.x*result.x + result.y*result.y;
}

//////////////////////////////////////////////////////////////////////////
int GetTaxicabDistance2D( const IntVec2& positionA, const IntVec2& positionB )
{
	IntVec2 displacement = positionA - positionB;
	return displacement.GetTaxicabLength();
}

//////////////////////////////////////////////////////////////////////////
const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	//change scale of position
	Vec2 result = position*uniformScale;
	//rotate
	if( rotationDegrees != 0.f )
	{
		float newRadians = ConvertDegreesToRadians( Atan2Degrees( result.y, result.x ) + rotationDegrees );
		float length = std::sqrtf( result.x * result.x + result.y * result.y );
		result.x = length * std::cosf( newRadians );
		result.y = length * std::sinf( newRadians );
	}
	//translate
	result = result+translation;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 TransformPosition2D( const Vec2& position, const Vec2& basisIVector, const Vec2& basisJVector, const Vec2& translation )
{
	Vec2 newPosition;
	newPosition = basisIVector * position.x + basisJVector * position.y + translation;
	return newPosition;
}

//////////////////////////////////////////////////////////////////////////
const Vec3 TransformPosition3DXY( const Vec3& position, float uniformScale, float zRotationDegrees, const Vec2& translationXY )
{
	Vec2 resultXY = TransformPosition2D(Vec2(position.x, position.y), uniformScale, zRotationDegrees, translationXY);
	return Vec3(resultXY.x, resultXY.y, position.z);
}

//////////////////////////////////////////////////////////////////////////
const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& basisIVector, const Vec2& basisJVector, const Vec2& translationXY )
{
	Vec2 positionXY ( position.x, position.y);
	positionXY = TransformPosition2D( positionXY, basisIVector, basisJVector, translationXY );
	return Vec3( positionXY.x, positionXY.y, position.z );
}

//////////////////////////////////////////////////////////////////////////
float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float lengthA = vectorA.GetLengthSquared();
	float lengthB = vectorB.GetLengthSquared();
	if( lengthA == 0.f || lengthB == 0.f )
	{
		g_theConsole->PrintString( Rgba8::WHITE, Stringf( "MathUtils::GetAngleDegreesBetweenVectors2D has one 0 vector, return 0" ) );
		return 0.f;
	}

	float dotProduct = DotProduct2D( vectorA, vectorB );
	float radians = std::acosf( dotProduct / std::sqrtf(lengthA *lengthB) );
	return ConvertRadiansToDegrees( radians );
}

//////////////////////////////////////////////////////////////////////////
float GetAngleDegreesBetweenVectors3D(Vec3 const& vectorA, Vec3 const& vectorB)
{
	float lengthA = vectorA.GetLengthSquared();
	float lengthB = vectorB.GetLengthSquared();
	if (lengthB == 0.f || lengthA == 0.f) {
		g_theConsole->PrintError("MathUtils::GetAngleDegreesBetweenVectors3D has one 0 vector, return 0");
		return 0.f;
	}

	float dotProduct = DotProduct3D(vectorA, vectorB);
	float radians = std::acosf(dotProduct/std::sqrtf(lengthA*lengthB));
	return ConvertRadiansToDegrees(radians);
}

//////////////////////////////////////////////////////////////////////////
float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	float ontoLength = ontoVector.GetLength();
	if( ontoLength == 0.f )
	{
		g_theConsole->PrintString( Rgba8::WHITE, Stringf( "MathUtils::GetProjectedLength2D has 0 ontoVector, return 0" ) );
		return 0;
	}

	float dotProduct = DotProduct2D( sourceVector, ontoVector );
	return dotProduct / ontoLength;
}

//////////////////////////////////////////////////////////////////////////
float GetProjectedLength3D(Vec3 const& sourceVec, Vec3 const& ontoVec)
{
	float ontoLength = ontoVec.GetLength();
	if (ontoLength == 0.f) {
		g_theConsole->PrintString(Rgba8::WHITE, Stringf("MathUtils::GetProjectedLength3D has 0 ontoVector, return 0"));
		return 0.f;
	}

	float dotProduct = DotProduct3D(sourceVec,ontoVec);
	return dotProduct/ontoLength;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	float ontoLengthSquared = ontoVector.GetLengthSquared();
	if( ontoLengthSquared == 0.f )
	{
		g_theConsole->PrintString( Rgba8::WHITE, Stringf( "MathUtils::GetProjectedOnto2D has 0 ontoVector, return ZERO" ) );
		return Vec2::ZERO;
	}

	float dotProduct = DotProduct2D( sourceVector, ontoVector );
	return ontoVector * dotProduct / ontoLengthSquared;
}

//////////////////////////////////////////////////////////////////////////
bool DoDiscsOverlap2D( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distanceSquared = GetDistanceSquared2D( centerA, centerB );
	float radiusSum = radiusA + radiusB;
	if( radiusSum*radiusSum > distanceSquared )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool DoSpheresOverlap3D( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	float distanceSquared = GetDistanceSquared3D( centerA, centerB );
	float radiusSum = radiusA + radiusB;
	if( radiusSum*radiusSum > distanceSquared )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool DoAABBsOverlap2D( const AABB2& aabb2D1, const AABB2& aabb2D2 )
{
	if( aabb2D1.maxs.x<aabb2D2.mins.x || aabb2D1.maxs.y<aabb2D2.mins.y
		|| aabb2D1.mins.x>aabb2D2.maxs.x || aabb2D1.mins.y>aabb2D2.maxs.y )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool DoDiscAndAABBOverlap2D( const Vec2& center, float radius, const AABB2& aabb2D )
{
	Vec2 nearestDiscCenterOnAABB = GetNearestPointOnAABB2D( center, aabb2D );
	if( GetDistance2D( center, nearestDiscCenterOnAABB ) > radius )
		return false;
	else 
		return true;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnDisc2D( const Vec2& point, const Vec2& center, float radius )
{
	Vec2 toPoint = point - center;
	toPoint.ClampLength( radius );
	return center + toPoint;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnAABB2D( const Vec2& point, const AABB2& aabb2D )
{
	float newX = Clamp( point.x, aabb2D.mins.x, aabb2D.maxs.x );
	float newY = Clamp( point.y, aabb2D.mins.y, aabb2D.maxs.y );
	return Vec2( newX, newY );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 someToAnother = anotherPointOnLine - somePointOnLine;
	Vec2 someToRef = refPos - somePointOnLine;
	Vec2 someToRefOnLine = GetProjectedOnto2D( someToRef, someToAnother );
	return somePointOnLine + someToRefOnLine;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end )
{
	Vec2 startToEnd = end - start;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		return start;
	}

	Vec2 startToRefPos = refPos - start;
	float projectedUnit = DotProduct2D( startToEnd, startToRefPos )/segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;
	
	if( projectedUnit <= 0.f ) // nearest point is start
	{
		return start;
	}
	else if( projectedUnit >= 1.f )//nearest point is end
	{
		return end;
	}
	else
	{
		return start + startToEnd*projectedUnit;
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	Vec2 startToEnd = capsuleMidEnd - capsuleMidStart;
	Vec2 startToRefPos = refPos - capsuleMidStart;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		return startToRefPos.GetClamped(capsuleRadius) + capsuleMidStart;
	}
	
	float projectedUnit = DotProduct2D( startToEnd, startToRefPos ) / segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;

	if( projectedUnit <= 0.f ) // nearest point is start semi-circle
	{
		return startToRefPos.GetClamped( capsuleRadius ) + capsuleMidStart;
	}
	else if( projectedUnit >= 1.f )//nearest point is end semi-circle
	{
		Vec2 endToRefPos = refPos - capsuleMidEnd;
		return capsuleMidEnd + endToRefPos.GetClamped( capsuleRadius );
	}
	else
	{
		Vec2 pointOnLine = capsuleMidStart + startToEnd * projectedUnit;
		Vec2 onLineToBound = (refPos - pointOnLine).GetClamped( capsuleRadius );
		return pointOnLine + onLineToBound;
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box )
{
	return box.GetNearestPoint( refPos );
}

//////////////////////////////////////////////////////////////////////////
FloatRange GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeCenter, const Vec2& axisNormalized )
{
	float* projectedLength = new float[numPoints];
	for( int pIdx = 0; pIdx < numPoints; pIdx++ )
	{
		Vec2 centerToPoint = points[pIdx] - relativeCenter;
		float length = DotProduct2D( centerToPoint, axisNormalized );
		projectedLength[pIdx] = length;
	}

	if( numPoints <1  )
	{
		return FloatRange( 0.f );
	}
	else 
	{
		float minimum = projectedLength[0];
		float maximum = projectedLength[0];
		for( int pIdx = 1; pIdx < numPoints; pIdx++ )
		{
			float curLength = projectedLength[pIdx];
			if( curLength < minimum )
			{
				minimum = curLength;
			}
			else if( curLength > maximum )
			{
				maximum = curLength;
			}
		}
		delete[] projectedLength;
		return FloatRange( minimum, maximum );
	}
}

//////////////////////////////////////////////////////////////////////////
bool DoesRayHitPlane2D(Vec2 const& start, Vec2 const& forward, Plane2D const& plane)
{
	if (plane.IsPointInFront(start)) {
		return DotProduct2D(forward,plane.normal)<0.f;
	}
	else {
		return DotProduct2D(forward, plane.normal)>0.f;
	}
}

//////////////////////////////////////////////////////////////////////////
bool DoesRayHitBoundingBox2D(Vec2 const& start, Vec2 const& end, AABB2 const& bounds)
{
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	Vec2 raySeg = end-start;
    if (raySeg.x == 0.f) {  //ray parallel to y axis
        float rayMinY = MinFloat(start.y, end.y);
        float rayMaxY = MaxFloat(start.y, end.y);
        FloatRange rayYRange(rayMinY, rayMaxY);
        FloatRange boundYRange(bounds.mins.y, bounds.maxs.y);
        FloatRange boundXRange(bounds.mins.x, bounds.maxs.x);
        return boundXRange.IsInRange(start.x) && rayYRange.DoesOverlap(boundYRange);
    }

    if (raySeg.y == 0.f) {  //ray parallel to x axis
        float rayMinX = MinFloat(start.x, end.x);
        float rayMaxX = MaxFloat(start.x, end.x);
        FloatRange rayXRange(rayMinX, rayMaxX);
        FloatRange boundYRange(bounds.mins.y, bounds.maxs.y);
        FloatRange boundXRange(bounds.mins.x, bounds.maxs.x);
        return boundYRange.IsInRange(start.y) && rayXRange.DoesOverlap(boundXRange);
    }

    float raySegXFrac = 1.f / raySeg.x;
    float raySegYFrac = 1.f / raySeg.y;
    float boundFirstX = raySeg.x < 0 ?  bounds.maxs.x : bounds.mins.x;
    float boundSecondX = raySeg.x < 0 ? bounds.mins.x : bounds.maxs.x;
    float boundFirstY = raySeg.y < 0 ?  bounds.maxs.y : bounds.mins.y;
    float boundSecondY = raySeg.y < 0 ? bounds.mins.y : bounds.maxs.y;
    float minInterX = (boundFirstX - start.x) * raySegXFrac;
    float maxInterX = (boundSecondX - start.x) * raySegXFrac;
    float minInterY = (boundFirstY - start.y) * raySegYFrac;
    float maxInterY = (boundSecondY - start.y) * raySegYFrac;
    if (minInterX > maxInterY || minInterY > maxInterX) {
        return false;
    }

	//out of reach by length
	float minLength = MinFloat(minInterX,maxInterX);
	minLength = MinFloat(minLength,minInterY);
	minLength = MinFloat(maxInterY, minLength);
	if (minLength * minLength > raySeg.GetLengthSquared()) {
		return false;
	}

    return true;
}

//////////////////////////////////////////////////////////////////////////
bool DoesRayHitLineSegment2D(Vec2 const& start, Vec2 const& end, Vec2 const& lineA, Vec2 const& lineB)
{	
	//referencing https://www.youtube.com/watch?v=c065KoXooSw
	Vec2 sToA = lineA-start;
	Vec2 sToB = lineB-start;
	Vec2 forward = end-start;
	float sToADot = DotProduct2D(sToA, forward);
	float sToBDot = DotProduct2D(sToB, forward);
	if (sToADot < 0.f && sToBDot < 0.f) {
		return false;
	}
	float rayLengthSquared = GetDistanceSquared2D(start,end);
	if (sToADot > rayLengthSquared && sToBDot > rayLengthSquared) {
		return false;
	}
	Vec2 vertical = forward.GetRotated90Degrees();
	float sToAVertDot = DotProduct2D(vertical, sToA);
	float sToBVertDot = DotProduct2D(vertical, sToB);
	if ((sToAVertDot > 0.f && sToBVertDot > 0.f) || (sToAVertDot < 0.f && sToBVertDot<0.f)) {
		return false;
	}

	Vec2 aToB = lineB-lineA;
	float fraction = 1.f/(CrossProduct2D(forward, aToB));
	float rayParameter = CrossProduct2D(sToA,aToB)*fraction;
	float segParameter = CrossProduct2D(sToA, forward)*fraction;
	return rayParameter>=0.f && segParameter>=0.f && segParameter<=1.f;
}

//////////////////////////////////////////////////////////////////////////
bool DoesRayHitLineSegment2D(Vec2 const& start, Vec2 const& end, Vec2 const& lineA, Vec2 const& lineB, Vec2& outHitPoint, Vec2& outHitNormal, float& outDistance)
{
    Vec2 sToA = lineA - start;
    Vec2 forward = end - start;
    Vec2 sToB = lineB - start;
    float sToADot = DotProduct2D(sToA, forward);
    float sToBDot = DotProduct2D(sToB, forward);
    if (sToADot < 0.f && sToBDot < 0.f) {
        return false;
    }
    float rayLengthSquared = GetDistanceSquared2D(start, end);
    if (sToADot > rayLengthSquared && sToBDot > rayLengthSquared) {
        return false;
    }

    Vec2 vertical = forward.GetRotated90Degrees();
    float sToAVertDot = DotProduct2D(vertical, sToA);
    float sToBVertDot = DotProduct2D(vertical, sToB);
    if ((sToAVertDot > 0.f && sToBVertDot > 0.f) || (sToAVertDot < 0.f && sToBVertDot < 0.f)) {
        return false;
    }

    Vec2 aToB = lineB - lineA;
    float fraction = 1.f / CrossProduct2D(forward, aToB);
    float rayParameter = CrossProduct2D(sToA, aToB) * fraction;
    if (rayParameter<0.f||rayParameter>1.f) {
        return false;
    }

    float edgeParameter = CrossProduct2D(sToA, forward) * fraction;
    if (edgeParameter>=0.f && edgeParameter<=1.f) {//ray hit line
        outHitPoint = start + rayParameter * forward;
        outHitNormal = aToB.GetNormalized().GetRotatedMinus90Degrees();
        outDistance = rayParameter;
        return true;
    }
	return false;
}

//////////////////////////////////////////////////////////////////////////
Vec2 GetCentroidOfPolygon(Vec2 const* points, int pointCount)
{
	if (pointCount <= 0)
	{
		return Vec2::ZERO;
	}
	else if (pointCount <= 2)
	{
		return points[0];
	}

	std::vector<Vec2> relativePoints;
	for (size_t idx = 0; idx < pointCount; idx++)
	{
		relativePoints.push_back(points[idx] - points[0]);
	}

	float areaSum = 0.f;
	Vec2 centroidSum;
	for (size_t idx = 1; idx < pointCount - 1; idx++)
	{
		Vec2 first = relativePoints[idx];
		Vec2 second = relativePoints[idx + 1];

		float tempArea = first.x * second.y - first.y * second.x;
		tempArea = tempArea < 0 ? -tempArea : tempArea;
		areaSum += tempArea;
		centroidSum += tempArea * Vec2(first.x + second.x, first.y + second.y);
	}

	return centroidSum / (areaSum * 3.f) + points[0];
}

//////////////////////////////////////////////////////////////////////////S
Disc2 GetMinimumOuterDiscForPolygon(Vec2 const* points, int pointCount)
{
	float maxDiameterSquared = 0.f;
	Vec2 maxCenter;

	//TODO less complexity?
	for (int i = 0; i < pointCount; i++) {
		Vec2 first = points[i];
		for (int j = i+1; j < pointCount; j++) {
			Vec2 second = points[j];
			for (int k = j+1; k < pointCount; k++) {
				Vec2 third = points[k];

				float diameterSquared = 0.f;
				float cosineSquared = 0.f;
				Vec2 center;

				float f2tSquared = GetDistanceSquared2D(first, third);
				float s2tSquared = GetDistanceSquared2D(second, third);
				float f2sSquared = GetDistanceSquared2D(first, second);
				//Non-acute triangle
				if (f2tSquared + s2tSquared <= f2sSquared) {
					diameterSquared = f2sSquared;
					if (diameterSquared > maxDiameterSquared) {
						maxCenter = (first + second) * .5f;
						maxDiameterSquared = diameterSquared;
					}
					continue;
				}
				else if (f2tSquared + f2sSquared <= s2tSquared) {
					diameterSquared = s2tSquared;
					if (diameterSquared > maxDiameterSquared) {
						maxCenter = (second + third) * .5f;
						maxDiameterSquared = diameterSquared;
					}
					continue;
				}
				else if (f2sSquared + s2tSquared <= f2tSquared) {
					diameterSquared = f2tSquared;
					if (diameterSquared > maxDiameterSquared) {
						maxCenter = (first + third) * .5f;
						maxDiameterSquared = diameterSquared;
					}
					continue;
				}

				//acute triangle
				float dotProduct = DotProduct2D(first - third, second - third);
				float dotProductSquared = dotProduct * dotProduct;
				if (f2tSquared != 0.f && s2tSquared != 0.f) {
					cosineSquared = dotProductSquared / (f2tSquared * s2tSquared);
					if (cosineSquared == 1.f) {
						diameterSquared = s2tSquared > f2tSquared ? s2tSquared : f2tSquared;
					}
					else {
						diameterSquared = f2sSquared / (1 - cosineSquared);
					}
				}
				else if (f2tSquared == 0.f) {
					diameterSquared = s2tSquared;
				}
				else {
					diameterSquared = f2tSquared;
				}

				if (diameterSquared > maxDiameterSquared) {
					maxDiameterSquared = diameterSquared;

					float centerToFSLength = std::sqrtf(diameterSquared * cosineSquared) * .5f;
					Vec2 halfFirst2Second = (second - first) * .5f;
					Vec2 edgeToCenter = halfFirst2Second.GetRotated90Degrees();
					edgeToCenter.SetLength(centerToFSLength);
					maxCenter = first + halfFirst2Second + edgeToCenter;
				}
			}
		}
	}

	return Disc2(maxCenter,std::sqrtf(maxDiameterSquared) * .5f);
}

//////////////////////////////////////////////////////////////////////////
bool IsPointOnRightSide(Vec2 const& point, Vec2 const& start, Vec2 const& end) 
{
    Vec2 normal = (end - start).GetRotated90Degrees();
    Vec2 startToP = point - start;
    if (DotProduct2D(startToP, normal) < 0.f)
    {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist )
{
	Vec2 obToPoint = point - observerPos;
	float dist = obToPoint.GetLengthSquared();
	if( dist >= maxDist*maxDist )
		return false;

	float obToPointDegrees = obToPoint.GetAngleDegrees();
	float deltaDegrees = GetShortestAngularDisplacement( forwardDegrees, obToPointDegrees );
	float validDeltaDegrees = .5f * apertureDegrees;
	if( deltaDegrees>=validDeltaDegrees || deltaDegrees<=-validDeltaDegrees)
		return false;
	else 
		return true;
}

//////////////////////////////////////////////////////////////////////////
bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	Vec2 discToPoint = point - discCenter;
	if( discToPoint.GetLengthSquared() < discRadius * discRadius )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box )
{
	Vec2 boxCenterToPoint = point - box.GetCenter();
	if( boxCenterToPoint.x < box.maxs.x && boxCenterToPoint.x > box.mins.x
		&& boxCenterToPoint.y < box.maxs.y && boxCenterToPoint.y > box.mins.y )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	Vec2 startToEnd = capsuleMidEnd - capsuleMidStart;
	Vec2 startToRefPos = point - capsuleMidStart;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		if( startToRefPos.GetLengthSquared() < capsuleRadius * capsuleRadius )
			return true;
		else
			return false;
	}

	float projectedUnit = DotProduct2D( startToEnd, startToRefPos ) / segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;
	if( projectedUnit <= 0.f ) // nearest point is start semi-circle
	{
		if( startToRefPos.GetLengthSquared() < capsuleRadius * capsuleRadius )
			return true;
		else
			return false;
	}
	else if( projectedUnit >= 1.f )//nearest point is end semi-circle
	{
		Vec2 endToRefPos = point - capsuleMidEnd;
		if( endToRefPos.GetLengthSquared() < capsuleRadius * capsuleRadius )
			return true;
		else
			return false;
	}
	else
	{
		Vec2 pointOnLine = capsuleMidStart + startToEnd * projectedUnit;
		Vec2 onLineToRef = (point - pointOnLine);
		if( onLineToRef.GetLengthSquared() < capsuleRadius * capsuleRadius )
			return true;
		else
			return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box )
{
	return box.IsPointInside( point );
}

//////////////////////////////////////////////////////////////////////////
float DotProduct2D( const Vec2& vec2DA, const Vec2& vec2DB )
{
	return vec2DA.x * vec2DB.x + vec2DA.y * vec2DB.y;
}

//////////////////////////////////////////////////////////////////////////
float DotProduct3D( const Vec3& vec3DA, const Vec3& vec3DB )
{
	return vec3DA.x * vec3DB.x + vec3DA.y * vec3DB.y + vec3DA.z * vec3DB.z;
}

//////////////////////////////////////////////////////////////////////////
float DotProduct4D( const Vec4& vec4DA, const Vec4& vec4DB )
{
	return vec4DA.x * vec4DB.x + vec4DA.y * vec4DB.y + vec4DA.z * vec4DB.z + vec4DA.w * vec4DB.w;
}

//////////////////////////////////////////////////////////////////////////
float CrossProduct2D(Vec2 const& vec2DA, Vec2 const& vec2DB)
{
	return vec2DA.x * vec2DB.y - vec2DA.y * vec2DB.x;
}

//////////////////////////////////////////////////////////////////////////
Vec3 CrossProduct3D(Vec3 const& vec3DA, Vec3 const& vec3DB)
{
	return Vec3(vec3DA.y*vec3DB.z - vec3DA.z*vec3DB.y,
				vec3DA.z*vec3DB.x - vec3DA.x*vec3DB.z,
				vec3DA.x*vec3DB.y - vec3DA.y*vec3DB.x);
}

//////////////////////////////////////////////////////////////////////////
bool DoOBBAndOBBOverlap2D(const OBB2& boxA, const OBB2& boxB)
{
	//set boxA as relative center
	Vec2 boxPoints[4];
	boxB.GetCornerPositions( &boxPoints[0] );
	FloatRange boxBRange = GetRangeOnProjectedAxis( 4, &boxPoints[0], boxA.GetCenter(), boxA.GetIBasisNormal() );
	Vec2 boxAHalfDim = boxA.GetDimensions()*.5f;
	FloatRange boxARange( -boxAHalfDim.x, boxAHalfDim.x );
	if( !boxARange.DoesOverlap( boxBRange ) )
		return false;

	boxBRange = GetRangeOnProjectedAxis( 4, &boxPoints[0], boxA.GetCenter(), boxA.GetJBasisNormal() );
	boxARange = FloatRange( -boxAHalfDim.y, boxAHalfDim.y );
	if( !boxARange.DoesOverlap( boxBRange ) )
		return false;

	//Set boxB as relative center
	boxA.GetCornerPositions( &boxPoints[0] );
	boxARange = GetRangeOnProjectedAxis( 4, &boxPoints[0], boxB.GetCenter(), boxB.GetIBasisNormal() );
	Vec2 boxBHalfDim = boxB.GetDimensions() * .5f;
	boxBRange= FloatRange( -boxBHalfDim.x, boxBHalfDim.x );
	if( !boxBRange.DoesOverlap( boxARange ) )
		return false;

	boxARange = GetRangeOnProjectedAxis( 4, &boxPoints[0], boxB.GetCenter(), boxB.GetJBasisNormal() );
	boxBRange = FloatRange( -boxBHalfDim.y, boxBHalfDim.y );
	if( !boxBRange.DoesOverlap( boxARange ) )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void PushDiscOutOfDisc2D( Vec2& discMobile, float radiusMobile, const Vec2& discStill, float radiusStill )
{
	Vec2 stillToPushed = discMobile - discStill;
	float radiusSum = radiusMobile + radiusStill;
	float stillToPushedSquared = stillToPushed.GetLengthSquared();
	if( stillToPushedSquared < radiusSum*radiusSum )
	{
		float scale = radiusSum / std::sqrtf( stillToPushedSquared );
		stillToPushed *= scale;
		discMobile = discStill + stillToPushed;
	}
}

//////////////////////////////////////////////////////////////////////////
void PushDiscsOutOfEachOther2D( Vec2& discA, float radiusA, Vec2& discB, float radiusB)
{
	Vec2 difference = discA - discB;
	float radiusSum = radiusA + radiusB;
	float diffSquared = difference.GetLengthSquared();
	if( diffSquared < radiusSum*radiusSum )
	{
		float diff = std::sqrtf( diffSquared );
		float scale = (radiusA + radiusB - diff) * .5f / diff;
		difference *= scale;
		discA += difference;
		discB -= difference;
	}
}

//////////////////////////////////////////////////////////////////////////
void PushDiscOutOfPoint2D( Vec2& disc, float radius, const Vec2& point )
{
	Vec2 pointToDisc = disc-point;
	float pointToDiscDistanceSquared = pointToDisc.GetLengthSquared();
	if( radius*radius > pointToDiscDistanceSquared )
	{
		float scale = radius / std::sqrtf( pointToDiscDistanceSquared );
		pointToDisc *= scale;
		disc = point + pointToDisc;
	}
}

//////////////////////////////////////////////////////////////////////////
void PushDiscOutOfAABB2D( Vec2& disc, float radius, const AABB2& bounds )
{
	Vec2 pointOnBounds =  GetNearestPointOnAABB2D( disc, bounds );
	PushDiscOutOfPoint2D( disc, radius, pointOnBounds );
}

//////////////////////////////////////////////////////////////////////////
bool NearlyEqual(float a, float b, float epsilon)
{
	float diff = a - b;
	return (diff < epsilon) && (diff > -epsilon);
}

//////////////////////////////////////////////////////////////////////////
void SwapFloat(float& a, float& b)
{
	float temp = b;
	b = a;
	a = temp;
}

//////////////////////////////////////////////////////////////////////////
float SqrtFloat(float a)
{
	if (a < 0.f) {
		g_theConsole->PrintString(Rgba8::RED, "sqartFloat receive negative");
		return 0.f;
	}
	else {
		return std::sqrtf(a);
	}
}

//////////////////////////////////////////////////////////////////////////
float AbsFloat(float a)
{
	return a >= 0.f ? a : -a;
}

//////////////////////////////////////////////////////////////////////////
float SignFloat(float a)
{
	return (a >= 0.f) ? 1.f : -1.f;
}

//////////////////////////////////////////////////////////////////////////
float MaxFloat(float a, float b, float c)
{
	if (a > b) {
		if (a > c) {
			return a;
		}
		else {
			return c;
		}
	}
	else {
		if (b > c) {
			return b;
		}
		else {
			return c;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
float MaxFloat(float a, float b)
{
	return a > b ? a : b;
}

//////////////////////////////////////////////////////////////////////////
float MinFloat(float a, float b, float c)
{
	if (a < b) {
		if (a < c) {
			return a;
		}
		else {
			return c;
		}
	}
	else {
		if (b < c) {
			return b;
		}
		else {
			return c;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
float MinFloat(float a, float b)
{
	return a < b ? a : b;
}

//////////////////////////////////////////////////////////////////////////
bool IsAbsValueBigger( float a, float b )
{
	return AbsFloat(a) > AbsFloat(b);
}

//////////////////////////////////////////////////////////////////////////
float RangeMapFloatClamped(float inBegin, float inEnd, float outBegin, float outEnd, float inTarget)
{
	inTarget = Clamp(inTarget, inBegin, inEnd);
	return RangeMapFloat(inBegin,inEnd,outBegin,outEnd,inTarget);
}

//////////////////////////////////////////////////////////////////////////
float RangeMapFloat( float inBegin, float inEnd, float outBegin, float outEnd, float inTarget )
{
	float fraction = GetFractionInRange( inBegin, inEnd, inTarget );
	return outBegin + fraction * (outEnd - outBegin);
}

//////////////////////////////////////////////////////////////////////////
Vec2 RangeMapVector2D(Vec2 const& inBegin, Vec2 const& inEnd, Vec2 const& outBegin, Vec2 const& outEnd, Vec2 const& inTarget)
{
	float x = RangeMapFloat(inBegin.x, inEnd.x, outBegin.x, outEnd.x, inTarget.x);
	float y = RangeMapFloat(inBegin.y, inEnd.y, outBegin.y, outEnd.y, inTarget.y);
	return Vec2(x, y);
}

//////////////////////////////////////////////////////////////////////////
Vec3 RangeMapVector3D(Vec3 const& inBegin, Vec3 const& inEnd, Vec3 const& outBegin, Vec3 const& outEnd, Vec3 const& inTarget)
{
	float x = RangeMapFloat(inBegin.x, inEnd.x, outBegin.x, outEnd.x, inTarget.x);
	float y = RangeMapFloat(inBegin.y, inEnd.y, outBegin.y, outEnd.y, inTarget.y);
	float z = RangeMapFloat(inBegin.z, inEnd.z, outBegin.z, outEnd.z, inTarget.z);
	return Vec3(x, y, z);
}

//////////////////////////////////////////////////////////////////////////
float GetFractionInRange( float begin, float end, float target )
{
	if( begin == end ) 
		return 0.f;
	else 
		return (target - begin) / (end - begin);
}

//////////////////////////////////////////////////////////////////////////
LineSegment2 ClipLineSegmentToLineSegment(LineSegment2 const& toClip, LineSegment2 const& refLine)
{
	Vec2 start = refLine.GetNearestPoint(toClip.start);
	Vec2 end = refLine.GetNearestPoint(toClip.end);
	return LineSegment2(start, end);
}

//////////////////////////////////////////////////////////////////////////
float Clamp( float target, float begin, float end )
{
	if( target < begin )
		return begin;
	else if( target > end )
		return end;
	else 
		return target;
}

//////////////////////////////////////////////////////////////////////////
int Clamp(int target, int begin, int end)
{
	if (target > end) {
		return end;
	}
	else if (target < begin) {
		return begin;
	}
	
	return target;
}

//////////////////////////////////////////////////////////////////////////
double Clamp(double target, double begin, double end)
{
    if (target > end) {
        return end;
    }
    else if (target < begin) {
        return begin;
    }

    return target;
}

//////////////////////////////////////////////////////////////////////////
float ClampZeroToOne( float target )
{
	if( target < 0.f )
		return 0.f;
	else if( target > 1.f )
		return 1.f;
	else 
		return target;
}

//////////////////////////////////////////////////////////////////////////
float Interpolate( float a, float b, float fractionOfB )
{
	return (1 - fractionOfB) * a + b * fractionOfB;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Interpolate(Vec2 const& vecA, Vec2 const& vecB, float fractionOfB)
{
	return fractionOfB*vecB + (1.f-fractionOfB)*vecA;
}

//////////////////////////////////////////////////////////////////////////
float Round( float value )
{
	return static_cast<float>(RoundToNearestInt( value ));
}

//////////////////////////////////////////////////////////////////////////
int RoundDownToInt( float value )
{
	if( value >= 0.f )
		return static_cast<int>(value);
	else 
		return static_cast<int>(value) - 1;
}

//////////////////////////////////////////////////////////////////////////
int RoundToNearestInt( float value )
{
	if( value > 0 )
	{
		value += .5f;
	}
	else
	{
		value -= .5f;
	}
	return static_cast<int>(value);
}

//////////////////////////////////////////////////////////////////////////
float SmoothStart2( float t )
{
	return t * t;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStart3( float t )
{
	return t * t * t;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStart4( float t )
{
	return t * t * t * t;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStart5( float t )
{
	return t * t * t * t * t;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStop2( float t )
{
	return -t * t + 2 * t;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStop3( float t )
{
	float m = t - 1;
	return -m * m * m + 1;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStop4( float t )
{
	float m = t - 1;
	return -m * m * m * m + 1;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStop5( float t )
{
	float m = t - 1;
	return -m * m * m * m * m + 1;
}

//////////////////////////////////////////////////////////////////////////
float SmoothStep3( float t )
{
	return 3 * t * t - 2 * t * t * t;
}

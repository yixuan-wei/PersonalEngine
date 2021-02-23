#pragma once

constexpr float fEPSILON = 0.0001f;
constexpr float fHalf_PI = 1.57079632679f;
constexpr float fPI     = 3.1415926535897932384626433832795f;
constexpr float fTWO_PI = 6.283185307179586476925286766559f;
constexpr float fSQRT_3_OVER_3 = 0.5773502691896257645091f;
constexpr float fSQRT_2_OVER_2 = 0.7071067811865475f;

struct Vec2;
struct Vec3;
struct Vec4;
struct Vertex_PCU;
struct IntVec2;
struct AABB2;
struct OBB2;
struct Plane2D;
struct FloatRange;
struct Disc2;
struct LineSegment2;

Vec3 Power(Vec3 const& base, float index);

float ConvertDegreesToRadians(float degrees);
float ConvertRadiansToDegrees(float radians);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float TanDegrees(float degrees);
float Atan2Degrees(float y, float x);
float GetSmallestSameDegrees( float degrees );
float GetShortestAngularDisplacement( float fromDegrees, float toDegrees );
float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees );//turn to goal within maximum delta degrees. maximum delta > 0, return result

float GetDistance2D ( const Vec2& firstVec2, const Vec2& secondVec2 );
float GetDistanceSquared2D ( const Vec2& firstVec2, const Vec2& secondVec2 );
float GetDistance3D ( const Vec3& firstVec3, const Vec3& secondVec3 );
float GetDistanceSquared3D( const Vec3& firstVec3, const Vec3& secondVec3 );
float GetDistanceXY3D( const Vec3& firstVec3, const Vec3& secondVec3 );
float GetDistanceXYSquared3D( const Vec3& firstVec3, const Vec3& secondVec3 );
int   GetTaxicabDistance2D( const IntVec2& positionA, const IntVec2& positionB );

const Vec2 TransformPosition2D(const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation );
const Vec2 TransformPosition2D( const Vec2& position, const Vec2& basisIVector, const Vec2& basisJVector, const Vec2& translation );
const Vec3 TransformPosition3DXY(const Vec3& position, float uniformScale, float zRotationDegrees, const Vec2& translationXY );
const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& basisIVector, const Vec2& basisJVector, const Vec2& translationXY );

float      GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB );
float      GetAngleDegreesBetweenVectors3D(Vec3 const& vectorA, Vec3 const& vectorB);
float      GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector );
float      GetProjectedLength3D( Vec3 const& sourceVec, Vec3 const& ontoVec );
const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );
const Vec2 GetNearestPointOnDisc2D( const Vec2& point, const Vec2& center, float radius );
const Vec2 GetNearestPointOnAABB2D( const Vec2& point, const AABB2& aabb );
const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine );
const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end );
const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
const Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box );
FloatRange GetRangeOnProjectedAxis(int numPoints, const Vec2* points, const Vec2& relativeCenter, const Vec2& axixNormalized);//relativeCenter is the "origin" for axis 

bool DoesRayHitBoundingBox2D(Vec2 const& start, Vec2 const& end, AABB2 const& bounds);
bool DoesRayHitPlane2D(Vec2 const& start, Vec2 const& forward, Plane2D const& plane);
bool DoesRayHitLineSegment2D(Vec2 const& start, Vec2 const& end, Vec2 const& lineA, Vec2 const& lineB);
bool DoesRayHitLineSegment2D(Vec2 const& start, Vec2 const& end, Vec2 const& lineA, Vec2 const& lineB, Vec2& outHitPoint, Vec2& outHitNormal, float& outDistance);//cross-product

Vec2 GetCentroidOfPolygon(Vec2 const* points, int pointCount);
Disc2 GetMinimumOuterDiscForPolygon(Vec2 const* points, int pointCount);

bool IsPointOnRightSide( Vec2 const& point, Vec2 const& start, Vec2 const& end );
bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist );
bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius );
bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box );
bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box );

float DotProduct2D( const Vec2& vec2DA, const Vec2& vec2DB );
float DotProduct3D( const Vec3& vec3DA, const Vec3& vec3DB );
float DotProduct4D( const Vec4& vec4DA, const Vec4& vec4DB );

float CrossProduct2D(Vec2 const& vec2DA, Vec2 const& vec2DB);
Vec3 CrossProduct3D(Vec3 const& vec3DA, Vec3 const& vec3DB);

bool DoOBBAndOBBOverlap2D(const OBB2& boxA, const OBB2& boxB);
bool DoDiscsOverlap2D( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB );
bool DoSpheresOverlap3D( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB );
bool DoAABBsOverlap2D( const AABB2& aabb2D1, const AABB2& aabb2D2 );
bool DoDiscAndAABBOverlap2D( const Vec2& center, float radius, const AABB2& aabb2D );

void PushDiscOutOfDisc2D( Vec2& discMobile, float radiusMobile, const Vec2& discStill, float radiusStill );
void PushDiscsOutOfEachOther2D( Vec2& discA, float radiusA, Vec2& discB, float radiusB );
void PushDiscOutOfPoint2D( Vec2& disc, float radius, const Vec2& point );
void PushDiscOutOfAABB2D( Vec2& disc, float radius, const AABB2& bounds );

bool  NearlyEqual(float a, float b, float epsilon = fEPSILON);
void  SwapFloat(float& a, float& b);
float SqrtFloat(float a);
float AbsFloat(float a);
float SignFloat(float a);
float MaxFloat(float a, float b);
float MinFloat(float a, float b);
float MaxFloat(float a, float b, float c);
float MinFloat(float a, float b, float c);
bool  IsAbsValueBigger( float bigger, float smaller );
float RangeMapFloatClamped( float inBegin, float inEnd, float outBegin, float outEnd, float inTarget );
float RangeMapFloat( float inBegin, float inEnd, float outBegin, float outEnd, float inTarget );
Vec2  RangeMapVector2D(Vec2 const& inBegin, Vec2 const& inEnd, Vec2 const& outBegin, Vec2 const& outEnd, Vec2 const& inTarget);
Vec3  RangeMapVector3D(Vec3 const& inBegin, Vec3 const& inEnd, Vec3 const& outBegin, Vec3 const& outEnd, Vec3 const& inTarget);
float GetFractionInRange( float begin, float end, float target );

LineSegment2 ClipLineSegmentToLineSegment(LineSegment2 const& toClip, LineSegment2 const& refLine);

double Clamp(double target, double begin, double end);
float Clamp( float target, float begin, float end );
int   Clamp(int target, int begin, int end);
float ClampZeroToOne( float target );
float Interpolate( float a, float b, float fractionOfB );
Vec2  Interpolate(Vec2 const& vecA, Vec2 const& vecB, float fractionOfB);
float Round( float value );
int   RoundDownToInt( float value );
int   RoundToNearestInt( float value );

float SmoothStart2( float t );
float SmoothStart3( float t );
float SmoothStart4( float t );
float SmoothStart5( float t );
float SmoothStop2( float t );
float SmoothStop3( float t );
float SmoothStop4( float t );
float SmoothStop5( float t );
float SmoothStep3( float t );
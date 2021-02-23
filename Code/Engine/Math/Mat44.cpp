#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"

const Mat44 Mat44::IDENTITY;

//////////////////////////////////////////////////////////////////////////
Mat44::Mat44( float* sixteenValuesBasis )
	:Ix(sixteenValuesBasis[0])
	,Iy(sixteenValuesBasis[1])
	,Iz(sixteenValuesBasis[2])
	,Iw(sixteenValuesBasis[3])

	,Jx(sixteenValuesBasis[4])
	,Jy(sixteenValuesBasis[5])
	,Jz(sixteenValuesBasis[6])
	,Jw(sixteenValuesBasis[7])

	,Kx(sixteenValuesBasis[8])
	,Ky(sixteenValuesBasis[9])
	,Kz(sixteenValuesBasis[10])
	,Kw(sixteenValuesBasis[11])

	,Tx(sixteenValuesBasis[12])
	,Ty(sixteenValuesBasis[13])
	,Tz(sixteenValuesBasis[14])
	,Tw(sixteenValuesBasis[15])
{

}

//////////////////////////////////////////////////////////////////////////
Mat44::Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
	:Ix(iBasis2D.x)
	,Iy(iBasis2D.y)

	,Jx(jBasis2D.x)
	,Jy(jBasis2D.y)

	,Tx(translation2D.x)
	,Ty(translation2D.y)
{
}

//////////////////////////////////////////////////////////////////////////
Mat44::Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
	:Ix(iBasis3D.x)
	,Iy(iBasis3D.y)
	,Iz(iBasis3D.z)

	,Jx(jBasis3D.x)
	,Jy(jBasis3D.y)
	,Jz(jBasis3D.z)

	,Kx(kBasis3D.x)
	,Ky(kBasis3D.y)
	,Kz(kBasis3D.z)

	,Tx(translation3D.x)
	,Ty(translation3D.y)
	,Tz(translation3D.z)
{
}

//////////////////////////////////////////////////////////////////////////
Mat44::Mat44( const Vec4& iBasisHomo, const Vec4& jBasisHomo, const Vec4& kBasisHomo, const Vec4& translationHomo )
	: Ix( iBasisHomo.x )
	, Iy( iBasisHomo.y )
	, Iz( iBasisHomo.z )
	, Iw( iBasisHomo.w )

	, Jx( jBasisHomo.x )
	, Jy( jBasisHomo.y )
	, Jz( jBasisHomo.z )
	, Jw( jBasisHomo.w )

	, Kx( kBasisHomo.x )
	, Ky( kBasisHomo.y )
	, Kz( kBasisHomo.z )
	, Kw( kBasisHomo.w )

	, Tx( translationHomo.x )
	, Ty( translationHomo.y )
	, Tz( translationHomo.z )
	, Tw( translationHomo.w )
{
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Mat44::TransformVector2D( const Vec2& vectorToTransform ) const
{
	return Vec2( Ix * vectorToTransform.x + Jx * vectorToTransform.y, 
		         Iy * vectorToTransform.x + Jy * vectorToTransform.y );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::TransformVector3D( const Vec3& vectorToTransform ) const
{
	return Vec3( Ix * vectorToTransform.x + Jx * vectorToTransform.y + Kx * vectorToTransform.z,
		         Iy * vectorToTransform.x + Jy * vectorToTransform.y + Ky * vectorToTransform.z,
		         Iz * vectorToTransform.x + Jz * vectorToTransform.y + Kz * vectorToTransform.z );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Mat44::TransformPosition2D( const Vec2& position ) const
{
	return Vec2( Ix * position.x + Jx * position.y + Tx,
		         Iy * position.x + Jy * position.y + Ty );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::TransformPosition3D( const Vec3& position ) const
{
	return Vec3( Ix * position.x + Jx * position.y + Kx * position.z + Tx,
		         Iy * position.x + Jy * position.y + Ky * position.z + Ty,
		         Iz * position.x + Jz * position.y + Kz * position.z + Tz );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Mat44::TransformHomogeneousPoint3D( const Vec4& pointHomo ) const
{
	return Vec4( Ix * pointHomo.x + Jx * pointHomo.y + Kx * pointHomo.z + Tx * pointHomo.w,
		         Iy * pointHomo.x + Jy * pointHomo.y + Ky * pointHomo.z + Ty * pointHomo.w,
		         Iz * pointHomo.x + Jz * pointHomo.y + Kz * pointHomo.z + Tz * pointHomo.w,
		         Iw * pointHomo.x + Jw * pointHomo.y + Kw * pointHomo.z + Tw * pointHomo.w );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Mat44::GetIBasis2D() const
{
	return Vec2( Ix, Iy );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Mat44::GetJBasis2D() const
{
	return Vec2( Jx, Jy );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Mat44::GetTranslation2D() const
{
	return Vec2( Tx, Ty );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::GetIBasis3D() const
{
	return Vec3( Ix, Iy, Iz );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::GetJBasis3D() const
{
	return Vec3( Jx, Jy, Jz );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::GetKBasis3D() const
{
	return Vec3( Kx, Ky, Kz );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Mat44::GetTranslation3D() const
{
	return Vec3( Tx, Ty, Tz );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Mat44::GetIBasis4D() const
{
	return Vec4( Ix, Iy, Iz, Iw );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Mat44::GetJBasis4D() const
{
	return Vec4( Jx, Jy, Jz, Jw );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Mat44::GetKBasis4D() const
{
	return Vec4( Kx, Ky, Kz, Kw );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Mat44::GetTranslation4D() const
{
	return Vec4( Tx, Ty, Tz, Tw );
}

//////////////////////////////////////////////////////////////////////////
void Mat44::Transpose()
{
	float* start = &Ix;
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            int idxA = 4 * i + j;
            int idxB = 4 * j + i;
            float temp = start[idxA];
            start[idxA] = start[idxB];
            start[idxB] = temp;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetTranslation2D( const Vec2& translation2D )
{
	Tx = translation2D.x;
	Ty = translation2D.y;
	Tz = 0;
	Tw = 1;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetTranslation3D( const Vec3& translation3D )
{
	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
	Tw = 1;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;
	Iz = Iw = 0;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;
	Jz = Jw = 0;

	Tx = translation2D.x;
	Ty = translation2D.y;
	Tz = 0;
	Tw = 1;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;
	Iw = 0;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;
	Jw = 0;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;
	Kw = 0;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;
	Iw = 0;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;
	Jw = 0;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;
	Kw = 0;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
	Tw = 1;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D )
{
	Ix = iBasis4D.x;
	Iy = iBasis4D.y;
	Iz = iBasis4D.z;
	Iw = iBasis4D.w;

	Jx = jBasis4D.x;
	Jy = jBasis4D.y;
	Jz = jBasis4D.z;
	Jw = jBasis4D.w;

	Kx = kBasis4D.x;
	Ky = kBasis4D.y;
	Kz = kBasis4D.z;
	Kw = kBasis4D.w;

	Tx = translation4D.x;
	Ty = translation4D.y;
	Tz = translation4D.z;
	Tw = translation4D.w;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::RotateXDegrees( float degreesAboutX )
{
	float cosine = CosDegrees( degreesAboutX );
	float sine = SinDegrees( degreesAboutX );
	Vec4 oldJBasis( Jx, Jy, Jz, Jw );
	Vec4 oldKBasis( Kx, Ky, Kz, Kw );

	Jx = oldJBasis.x * cosine + oldKBasis.x * sine;
	Jy = oldJBasis.y * cosine + oldKBasis.y * sine;
	Jz = oldJBasis.z * cosine + oldKBasis.z * sine;
	Jw = oldJBasis.w * cosine + oldKBasis.w * sine;

	Kx = oldJBasis.x * (-sine) +oldKBasis.x * cosine;	
	Ky = oldJBasis.y * (-sine) +oldKBasis.y * cosine;	
	Kz = oldJBasis.z * (-sine) +oldKBasis.z * cosine;	
	Kw = oldJBasis.w * (-sine) +oldKBasis.w * cosine;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::RotateYDegrees( float degreesAboutY )
{
	float cosine = CosDegrees( degreesAboutY );
	float sine = SinDegrees( degreesAboutY );
	Vec4 oldIBasis( Ix, Iy, Iz, Iw );
	Vec4 oldKBasis( Kx, Ky, Kz, Kw );

	Ix = oldIBasis.x * cosine + oldKBasis.x * (-sine);
	Iy = oldIBasis.y * cosine + oldKBasis.y * (-sine);
	Iz = oldIBasis.z * cosine + oldKBasis.z * (-sine);
	Iw = oldIBasis.w * cosine + oldKBasis.w * (-sine);

	Kx = oldIBasis.x * sine + oldKBasis.x * cosine;
	Ky = oldIBasis.y * sine + oldKBasis.y * cosine;
	Kz = oldIBasis.z * sine + oldKBasis.z * cosine;
	Kw = oldIBasis.w * sine + oldKBasis.w * cosine;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::RotateZDegrees( float degreesAboutZ )
{
	float cosine = CosDegrees( degreesAboutZ );
	float sine = SinDegrees( degreesAboutZ );
	Vec4 oldIBasis( Ix, Iy, Iz, Iw );
	Vec4 oldJBasis( Jx, Jy, Jz, Jw );

	Ix = oldIBasis.x * cosine + oldJBasis.x * sine;
	Iy = oldIBasis.y * cosine + oldJBasis.y * sine;
	Iz = oldIBasis.z * cosine + oldJBasis.z * sine;
	Iw = oldIBasis.w * cosine + oldJBasis.w * sine;

	Jx = oldIBasis.x * (-sine) + oldJBasis.x * cosine;
	Jy = oldIBasis.y * (-sine) + oldJBasis.y * cosine;
	Jz = oldIBasis.z * (-sine) + oldJBasis.z * cosine;
	Jw = oldIBasis.w * (-sine) + oldJBasis.w * cosine;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::Translate2D( const Vec2& translationXY )
{
	Tx = translationXY.x * Ix + translationXY.y * Jx + Tx;
	Ty = translationXY.x * Iy + translationXY.y * Jy + Ty;
	Tz = translationXY.x * Iz + translationXY.y * Jz + Tz;
	Tw = translationXY.x * Iw + translationXY.y * Jw + Tw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::Translate3D( const Vec3& translation3D )
{
	Tx = translation3D.x * Ix + translation3D.y * Jx + translation3D.z * Kx + Tx;
	Ty = translation3D.x * Iy + translation3D.y * Jy + translation3D.z * Ky + Ty;
	Tz = translation3D.x * Iz + translation3D.y * Jz + translation3D.z * Kz + Tz;
	Tw = translation3D.x * Iw + translation3D.y * Jw + translation3D.z * Kw + Tw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	Ix = uniformScaleXY * Ix;
	Iy = uniformScaleXY * Iy;
	Iz = uniformScaleXY * Iz;
	Iw = uniformScaleXY * Iw;
	
	Jx = uniformScaleXY * Jx;
	Jy = uniformScaleXY * Jy;
	Jz = uniformScaleXY * Jz;
	Jw = uniformScaleXY * Jw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::ScaleNonUniform2D( const Vec2& scaleFactorsXY )
{
	Ix = scaleFactorsXY.x * Ix;
	Iy = scaleFactorsXY.x * Iy;
	Iz = scaleFactorsXY.x * Iz;
	Iw = scaleFactorsXY.x * Iw;

	Jx = scaleFactorsXY.y * Jx;
	Jy = scaleFactorsXY.y * Jy;
	Jz = scaleFactorsXY.y * Jz;
	Jw = scaleFactorsXY.y * Jw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	Ix = uniformScaleXYZ * Ix;
	Iy = uniformScaleXYZ * Iy;
	Iz = uniformScaleXYZ * Iz;
	Iw = uniformScaleXYZ * Iw;
					   
	Jx = uniformScaleXYZ * Jx;
	Jy = uniformScaleXYZ * Jy;
	Jz = uniformScaleXYZ * Jz;
	Jw = uniformScaleXYZ * Jw;
	
	Kx = uniformScaleXYZ * Kx;
	Ky = uniformScaleXYZ * Ky;
	Kz = uniformScaleXYZ * Kz;
	Kw = uniformScaleXYZ * Kw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	Ix = scaleFactorsXYZ.x * Ix;
	Iy = scaleFactorsXYZ.x * Iy;
	Iz = scaleFactorsXYZ.x * Iz;
	Iw = scaleFactorsXYZ.x * Iw;

	Jx = scaleFactorsXYZ.y * Jx;
	Jy = scaleFactorsXYZ.y * Jy;
	Jz = scaleFactorsXYZ.y * Jz;
	Jw = scaleFactorsXYZ.y * Jw;
	
	Kx = scaleFactorsXYZ.z * Kx;
	Ky = scaleFactorsXYZ.z * Ky;
	Kz = scaleFactorsXYZ.z * Kz;
	Kw = scaleFactorsXYZ.z * Kw;
}

//////////////////////////////////////////////////////////////////////////
void Mat44::MultiplyRight( const Mat44& newMatrixToAppend )
{
	Mat44 oldMatrix( &Ix );

	Ix = oldMatrix.Ix * newMatrixToAppend.Ix + oldMatrix.Jx * newMatrixToAppend.Iy + oldMatrix.Kx * newMatrixToAppend.Iz + oldMatrix.Tx * newMatrixToAppend.Iw;
	Iy = oldMatrix.Iy * newMatrixToAppend.Ix + oldMatrix.Jy * newMatrixToAppend.Iy + oldMatrix.Ky * newMatrixToAppend.Iz + oldMatrix.Ty * newMatrixToAppend.Iw;
	Iz = oldMatrix.Iz * newMatrixToAppend.Ix + oldMatrix.Jz * newMatrixToAppend.Iy + oldMatrix.Kz * newMatrixToAppend.Iz + oldMatrix.Tz * newMatrixToAppend.Iw;
	Iw = oldMatrix.Iw * newMatrixToAppend.Ix + oldMatrix.Jw * newMatrixToAppend.Iy + oldMatrix.Kw * newMatrixToAppend.Iz + oldMatrix.Tw * newMatrixToAppend.Iw;

	Jx = oldMatrix.Ix * newMatrixToAppend.Jx + oldMatrix.Jx * newMatrixToAppend.Jy + oldMatrix.Kx * newMatrixToAppend.Jz + oldMatrix.Tx * newMatrixToAppend.Jw;
	Jy = oldMatrix.Iy * newMatrixToAppend.Jx + oldMatrix.Jy * newMatrixToAppend.Jy + oldMatrix.Ky * newMatrixToAppend.Jz + oldMatrix.Ty * newMatrixToAppend.Jw;
	Jz = oldMatrix.Iz * newMatrixToAppend.Jx + oldMatrix.Jz * newMatrixToAppend.Jy + oldMatrix.Kz * newMatrixToAppend.Jz + oldMatrix.Tz * newMatrixToAppend.Jw;
	Jw = oldMatrix.Iw * newMatrixToAppend.Jx + oldMatrix.Jw * newMatrixToAppend.Jy + oldMatrix.Kw * newMatrixToAppend.Jz + oldMatrix.Tw * newMatrixToAppend.Jw;
	
	Kx = oldMatrix.Ix * newMatrixToAppend.Kx + oldMatrix.Jx * newMatrixToAppend.Ky + oldMatrix.Kx * newMatrixToAppend.Kz + oldMatrix.Tx * newMatrixToAppend.Kw;
	Ky = oldMatrix.Iy * newMatrixToAppend.Kx + oldMatrix.Jy * newMatrixToAppend.Ky + oldMatrix.Ky * newMatrixToAppend.Kz + oldMatrix.Ty * newMatrixToAppend.Kw;
	Kz = oldMatrix.Iz * newMatrixToAppend.Kx + oldMatrix.Jz * newMatrixToAppend.Ky + oldMatrix.Kz * newMatrixToAppend.Kz + oldMatrix.Tz * newMatrixToAppend.Kw;
	Kw = oldMatrix.Iw * newMatrixToAppend.Kx + oldMatrix.Jw * newMatrixToAppend.Ky + oldMatrix.Kw * newMatrixToAppend.Kz + oldMatrix.Tw * newMatrixToAppend.Kw;

	Tx = oldMatrix.Ix * newMatrixToAppend.Tx + oldMatrix.Jx * newMatrixToAppend.Ty + oldMatrix.Kx * newMatrixToAppend.Tz + oldMatrix.Tx * newMatrixToAppend.Tw;
	Ty = oldMatrix.Iy * newMatrixToAppend.Tx + oldMatrix.Jy * newMatrixToAppend.Ty + oldMatrix.Ky * newMatrixToAppend.Tz + oldMatrix.Ty * newMatrixToAppend.Tw;
	Tz = oldMatrix.Iz * newMatrixToAppend.Tx + oldMatrix.Jz * newMatrixToAppend.Ty + oldMatrix.Kz * newMatrixToAppend.Tz + oldMatrix.Tz * newMatrixToAppend.Tw;
	Tw = oldMatrix.Iw * newMatrixToAppend.Tx + oldMatrix.Jw * newMatrixToAppend.Ty + oldMatrix.Kw * newMatrixToAppend.Tz + oldMatrix.Tw * newMatrixToAppend.Tw;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateXRotationDegrees( float degreesAroundX )
{
	float cosine = CosDegrees( degreesAroundX );
	float sine = SinDegrees( degreesAroundX );

	Mat44 result;	
	result.Jy = cosine;
	result.Jz = sine;
	result.Ky = -sine;
	result.Kz = cosine;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateYRotationDegrees( float degreesAroundY )
{
	float cosine = CosDegrees( degreesAroundY );
	float sine = SinDegrees( degreesAroundY );

	Mat44 result;
	result.Ix = cosine;
	result.Iz = -sine;
	result.Kx = sine;
	result.Kz = cosine;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateZRotationDegrees( float degreesAroundZ )
{
	float cosine = CosDegrees( degreesAroundZ );
	float sine = SinDegrees( degreesAroundZ );

	Mat44 result;
	result.Ix = cosine;
	result.Iy = sine;
	result.Jx = -sine;
	result.Jy = cosine;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateTranslationXY( const Vec2& translationXY )
{
	Mat44 result;
	result.Tx = translationXY.x;
	result.Ty = translationXY.y;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateTranslation3D( const Vec3& translation3D )
{
	Mat44 result;
	result.Tx = translation3D.x;
	result.Ty = translation3D.y;
	result.Tz = translation3D.z;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateUniformScaleXY( float uniformScaleXY )
{
	Mat44 result;
	result.Ix *= uniformScaleXY;
	result.Jy *= uniformScaleXY;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateNonUniformScaleXY( const Vec2& scaleFactorsXY )
{
	Mat44 result;
	result.Ix *= scaleFactorsXY.x;
	result.Jy *= scaleFactorsXY.y;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateUniformScale3D( float uniformScale3D )
{
	Mat44 result;
	result.Ix *= uniformScale3D;
	result.Jy *= uniformScale3D;
	result.Kz *= uniformScale3D;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 result;
	result.Ix *= scaleFactorsXYZ.x;
	result.Jy *= scaleFactorsXYZ.y;
	result.Kz *= scaleFactorsXYZ.z;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreateOrthographicProjection(const Vec3& min, const Vec3& max)
{
	//Range map x,y to -1.f to 1.f, z to 0 to 1.f
	float rangeX = max.x - min.x;
	float rangeY = max.y - min.y;
	float rangeZ = max.z - min.z;
	float mat[] = {
        2.f / rangeX,				0.f,						0.f,						0.f,
        0.f,						2.f / rangeY,				0.f,						0.f,
        0.f,						0.f,						2.f / rangeZ,				0.f,
        -(max.x+min.x) / rangeX,	-(min.y + max.y) / rangeY,	-(min.z + max.z) / rangeZ,	1.f
	};
	return Mat44(mat);
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::CreatePerspectiveProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    float height = 1.f / TanDegrees(fovDegrees * .5f);
    float zRange = farZ - nearZ;
    zRange = zRange <= 0.f ? 1.f : zRange;
    float q = 1.f / zRange;
    aspectRatio = aspectRatio == 0.f ? 1.f : aspectRatio;

    float mat[] = {
        height / aspectRatio, 0,      0,                      0,
        0,                    height, 0,                      0,
        0,                    0,      -farZ * q,              -1.f,
        0,                    0,      nearZ* farZ* q,         0
    };
    return Mat44(mat);
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::FromScaleRotationTranslation(Vec3 const& scale, Vec3 const& eulerRotation, Vec3 const& translation, AxisConvention convention)
{
	Mat44 mat = CreateNonUniformScale3D(scale);
	Mat44 rotateTrans = Mat44::FromRotationTranslation(eulerRotation, translation, convention);
	return MatrixMultiply(rotateTrans,mat);
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::FromRotationTranslation(Vec3 const& eulerRotation, Vec3 const& translation, AxisConvention convention)
{
	Mat44 rotate = Mat44::FromRotationDegrees(eulerRotation, convention);
	Mat44 trans = Mat44::CreateTranslation3D(translation);
	return MatrixMultiply(trans, rotate);
}

//////////////////////////////////////////////////////////////////////////
const Mat44 Mat44::FromRotationDegrees(Vec3 const& eulerRotation, AxisConvention convention)
{
	//Yaw -> Pitch -> Roll
	Mat44 rotation;
	switch (convention)
	{
	case AXIS__YZ_X:
	{
		rotation.RotateZDegrees(eulerRotation.z);
		rotation.RotateYDegrees(eulerRotation.y);
		rotation.RotateXDegrees(eulerRotation.x);
		break;
	}
	case AXIS_XYZ:
	default:
	{
		rotation.RotateYDegrees(eulerRotation.y);
		rotation.RotateXDegrees(eulerRotation.x);
		rotation.RotateZDegrees(eulerRotation.z);
		break;
	}
	}
	return rotation;
}

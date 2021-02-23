#pragma once

#include "Engine/Core/AxisConvention.hpp"

struct Vec2;
struct Vec3;
struct Vec4;

struct Mat44
{
public:
	float Ix = 1.f;
	float Iy = 0.f;
	float Iz = 0.f;
	float Iw = 0.f;

	float Jx = 0.f;
	float Jy = 1.f;
	float Jz = 0.f;
	float Jw = 0.f;

	float Kx = 0.f;
	float Ky = 0.f;
	float Kz = 1.f;
	float Kw = 0.f;

	float Tx = 0.f;
	float Ty = 0.f;
	float Tz = 0.f;
	float Tw = 1.f;
	
	const static Mat44 IDENTITY;

public:
	//constructors
	Mat44()=default; //default constructs to identity matrix
	explicit Mat44( float* sixteenValuesBasis );
	explicit Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D );
	explicit Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D );
	explicit Mat44( const Vec4& iBasisHomo, const Vec4& jBasisHomo, const Vec4& kBasisHomo, const Vec4& translationHomo );

	//transform positions & vectors
	const Vec2 TransformVector2D( const Vec2& vectorToTransform ) const; //z=0, w=0
	const Vec3 TransformVector3D( const Vec3& vectorToTransform ) const; //w=0
	const Vec2 TransformPosition2D( const Vec2& position ) const; //z=0, w=1
	const Vec3 TransformPosition3D( const Vec3& position ) const; //w=1
	const Vec4 TransformHomogeneousPoint3D( const Vec4& pointHomo ) const;	

	//basic accessors
	const float* GetAsFloatArray() const { return &Ix; }
	float*       GetAsFloatArray()       { return &Ix; }
	const Vec2   GetIBasis2D() const;
	const Vec2   GetJBasis2D() const;
	const Vec2   GetTranslation2D() const;
	const Vec3   GetIBasis3D() const;
	const Vec3   GetJBasis3D() const;
	const Vec3   GetKBasis3D() const;
	const Vec3   GetTranslation3D() const;
	const Vec4   GetIBasis4D() const;
	const Vec4   GetJBasis4D() const;
	const Vec4   GetKBasis4D() const;
	const Vec4   GetTranslation4D() const;

	void Transpose();

	//basic mutators: should set matrix from Identity
	void SetTranslation2D( const Vec2& translation2D );
	void SetTranslation3D( const Vec3& translation3D );
	void SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D );
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D );
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D );
	void SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D );
	
	//transformation mutator
	void RotateXDegrees( float degreesAboutX );
	void RotateYDegrees( float degreesAboutY );
	void RotateZDegrees( float degreesAboutZ );
	void Translate2D( const Vec2& translationXY );
	void Translate3D( const Vec3& translation3D );
	void ScaleUniform2D( float uniformScaleXY );
	void ScaleNonUniform2D( const Vec2& scaleFactorsXY );
	void ScaleUniform3D( float uniformScaleXYZ );
	void ScaleNonUniform3D( const Vec3& scaleFactorsXYZ );
	void MultiplyRight( const Mat44& newMatrixToAppend );

	//static construction
	static const Mat44 CreateXRotationDegrees( float degreesAroundX );
	static const Mat44 CreateYRotationDegrees( float degreesAroundY );
	static const Mat44 CreateZRotationDegrees( float degreesAroundZ );
	static const Mat44 CreateTranslationXY( const Vec2& translationXY );
	static const Mat44 CreateTranslation3D( const Vec3& translation3D );
	static const Mat44 CreateUniformScaleXY( float uniformScaleXY );
	static const Mat44 CreateNonUniformScaleXY( const Vec2& scaleFactorsXY );
	static const Mat44 CreateUniformScale3D( float uniformScale3D );
	static const Mat44 CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ );
	//projection
	static const Mat44 CreateOrthographicProjection(const Vec3& min, const Vec3& max);
	static const Mat44 CreatePerspectiveProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	//transform
	static const Mat44 FromScaleRotationTranslation(Vec3 const& scale, Vec3 const& eulerRotation, Vec3 const& translation, AxisConvention convention = AXIS_XYZ);
	static const Mat44 FromRotationTranslation(Vec3 const& eulerRotation, Vec3 const& translation, AxisConvention convention = AXIS_XYZ);
	static const Mat44 FromRotationDegrees(Vec3 const& eulerRotation, AxisConvention convention = AXIS_XYZ);

private:
	const Mat44 operator*( const Mat44& rhs ) const = delete; //so user don't struggle with column/row major rules
};
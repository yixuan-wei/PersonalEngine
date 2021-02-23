#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Transform.hpp"
#include <vector>

struct AABB2;
class Texture;
class RenderContext;
class RenderBuffer;

enum eCameraClearBitFlag : unsigned int
{
	CLEAR_NONE = 0,
	CLEAR_COLOR_BIT   = (1<<0),
	CLEAR_DEPTH_BIT   = (1<<1),
	CLEAR_STENCIL_BIT = (1<<2),
};

typedef unsigned int CameraClearFlags;

struct camera_data_t
{
	Mat44 projection;
	Mat44 view;
	Vec3 position;
	float pading00;
};

class Camera 
{
public:	
	Camera();
	~Camera();

    AABB2    GetBounds() const;
	Vec2     GetViewDimensions() const;
	float    GetAspectRaio() const;

	float	 GetClearDepth() const { return m_depth; }
	unsigned int GetClearMode() const { return m_clearMode; }
	int		 GetColorTargetCount() const { return (int)m_colorTargets.size(); }
    Rgba8    GetClearColor() const { return m_clearColor; }
	Texture* GetColorTarget() const;
	Texture* GetColorTarget(int idx) const;
	Texture* GetDepthStencilTarget() const { return m_depthStencilTarget; }

	bool     IsInsideCameraView( const Vec2& position ) const;
	Vec3     ClientToWorldPosition(Vec2 const& clientPos, float ndcZ= 0) const;
	Vec2	 WorldToClientPosition(Vec3 const& worldPos) const;
	Vec3	 GetForwardVector() const;
	Vec3     GetPosition() const			{ return m_transform.m_position; }
	Vec3	 GetEulerAngleDegrees() const	{ return m_transform.m_rotationAroundXYZDegrees; }
    Vec3	 GetPitchYawRollDegrees() const;

	RenderBuffer* GetCameraUBO() const	{ return m_cameraUBO; }
	void UpdateCameraData(RenderContext* ctx);

	Mat44 GetCameraModel() const		{ return m_camModel; }
	Mat44 GetView() const				{ return m_view; }
	Mat44 GetProjectionMatrix() const	{ return m_projection; }
	void SetViewMatrix(Mat44 const& newView);
	void SetProjectionMatrix(Mat44 const& newProjection);

    void SetClearMode(unsigned int clearFlags, Rgba8 color, float depth = 1.f, unsigned int stencil = 0);
    void SetColorTarget(Texture* texture);
	void SetColorTarget(int idx, Texture* colorTarget);
	void SetDepthStencilTarget(Texture* texture);

	void SetEulerAngleDegreesNoClamp(Vec3 const& newDegrees);
	void SetPitchYawRollDegreesClamp(Vec3 const& pitchYawRoll);
	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight);
	void SetPosition(Vec3 const& position);
	void WorldTranslate(Vec3 const& translation);
	void SetProjectionOrthographic(float height, float nearZ = -1.f, float farZ = 1.f);	
	void SetProjectionPerspective(float fovDegrees, float nearZ = -0.1f, float farZ = -100.f);

	AxisConvention GetAxisConvention() const {return m_axisConvention;}
	void SetAxisConvention(AxisConvention newConvention);

private:
	void UpdateViewMatrix();
    Vec3 GetOrthoBottomLeft() const;
    Vec3 GetOrthoTopRight() const;

private:
	AxisConvention m_axisConvention = AXIS_XYZ;
	Transform m_transform;
	float m_aspectRatio = 1.f;
	float m_viewHeight = 1.f;

	Mat44 m_projection;
	Mat44 m_camModel;
	Mat44 m_view;

	unsigned int m_clearMode = 0;
	float m_depth = 0.f;
	unsigned int m_stencil = 0;
	Rgba8 m_clearColor = Rgba8::BLACK;
	std::vector<Texture*> m_colorTargets;
	Texture* m_depthStencilTarget = nullptr;

	RenderBuffer* m_cameraUBO = nullptr;
};
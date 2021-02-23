#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Camera::Camera()
{
	m_colorTargets.push_back(nullptr);
}

//////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	delete m_depthStencilTarget;
}

//////////////////////////////////////////////////////////////////////////
AABB2 Camera::GetBounds() const
{
	Vec3 mins = GetOrthoBottomLeft();
	Vec3 maxs = GetOrthoTopRight();
	return AABB2( mins.x,mins.y,maxs.x,maxs.y );
}

//////////////////////////////////////////////////////////////////////////
Vec3 Camera::GetOrthoBottomLeft() const
{
	return ClientToWorldPosition(Vec2::ZERO);
}

//////////////////////////////////////////////////////////////////////////
Vec3 Camera::GetOrthoTopRight() const
{
	return ClientToWorldPosition(Vec2::ONE);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetViewDimensions() const
{
	return Vec2(m_viewHeight*m_aspectRatio,m_viewHeight);
}

//////////////////////////////////////////////////////////////////////////
Vec3 Camera::ClientToWorldPosition(const Vec2& clientPos, float ndcZ ) const
{
	//TODO - take into account render target
	//        clientPos being the pixel location on the texture
	
	//TODO - take into account viewport
	
    Vec3 ndc = RangeMapVector3D( Vec3(0, 0, 0), Vec3(1, 1, 1), Vec3(-1, -1, 0), Vec3(1, 1, 1), Vec3(clientPos, ndcZ));
    Mat44 view_projection = MatrixMultiply(m_projection, m_view);  // world to renderable space
    Mat44 clip_to_world = MatrixInverse(view_projection); // renderable space to world
    Vec4 worldPos = clip_to_world.TransformHomogeneousPoint3D(Vec4(ndc, 1.f));
    worldPos /= worldPos.w;
    return worldPos.GetXYZ();
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::WorldToClientPosition(Vec3 const& worldPos) const
{
	Mat44 view_projection = MatrixMultiply(m_projection, m_view);
	Vec3 clipPos = view_projection.TransformPosition3D(worldPos);
	clipPos = GetAxisMatrixFromConvention(m_axisConvention).TransformPosition3D(clipPos);
	Vec3 screenPos = RangeMapVector3D(Vec3(-1, -1, 0), Vec3(1, 1, 1), Vec3(0, 0, 0), Vec3(1, 1, 1), clipPos);
	return screenPos.GetXY();
}

//////////////////////////////////////////////////////////////////////////
Vec3 Camera::GetForwardVector() const
{
	Vec3 forward = GetWorldForwardVector(m_axisConvention);
	return m_camModel.TransformVector3D(forward);
}

//////////////////////////////////////////////////////////////////////////
float Camera::GetAspectRaio() const
{
	return m_aspectRatio;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetViewMatrix(Mat44 const& newView)
{
	m_view = newView;
	m_camModel = MatrixInverseOrthoNormal(m_view);
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetProjectionMatrix(Mat44 const& newProjection)
{
	m_projection = newProjection;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetClearMode( unsigned int clearFlags, Rgba8 color, float depth /*= 1.f*/, unsigned int stencil /*= 0 */ )
{
	m_clearColor = color;
	m_clearMode = clearFlags;
	m_depth = depth;
	m_stencil = stencil;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetColorTarget(Texture* texture)
{
	SetColorTarget(0, texture);
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetColorTarget(int idx, Texture* colorTarget)
{
	for (int i = (int)m_colorTargets.size(); i < idx+1; i++) {
		m_colorTargets.push_back(nullptr);
	}

	m_colorTargets[idx] = colorTarget;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetDepthStencilTarget(Texture* texture)
{
	if (m_depthStencilTarget != nullptr) {
		delete m_depthStencilTarget;
	}
	m_depthStencilTarget = texture;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetEulerAngleDegreesNoClamp(Vec3 const& newDegrees)
{
	m_transform.SetRotationAroundXYZDegrees(newDegrees);
}

void Camera::SetPitchYawRollDegreesClamp(Vec3 const& pitchYawRoll)
{
    float pitch = Clamp(AbsFloat(pitchYawRoll.x), 0.f, 89.9f);
    m_transform.SetRotationPitchYawRollDegrees(SignFloat(pitchYawRoll.x) * pitch, pitchYawRoll.y, pitchYawRoll.z, m_axisConvention);
}

//////////////////////////////////////////////////////////////////////////
Texture* Camera::GetColorTarget() const
{
	return GetColorTarget(0);
}

//////////////////////////////////////////////////////////////////////////
Texture* Camera::GetColorTarget(int idx) const
{
	if (idx < 0 || idx >= m_colorTargets.size()) {
		return nullptr;
	}

	return m_colorTargets[idx];
}

//////////////////////////////////////////////////////////////////////////
bool Camera::IsInsideCameraView( const Vec2& position )const
{
	AABB2 bound = GetBounds();
	if( position.x > bound.mins.x && position.x<bound.maxs.x &&
		position.y>bound.mins.y && position.y < bound.maxs.y )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
Vec3 Camera::GetPitchYawRollDegrees() const
{
	Vec3 const pitchYawRoll = m_transform.m_rotationAroundXYZDegrees;
	switch (m_axisConvention)
	{
	case AXIS_XYZ:		return pitchYawRoll;
	case AXIS__YZ_X:	return Vec3(pitchYawRoll.y, pitchYawRoll.z, pitchYawRoll.x);
	default:			return pitchYawRoll;
	}
}

//////////////////////////////////////////////////////////////////////////
void Camera::UpdateCameraData(RenderContext* ctx)
{
	if (m_cameraUBO == nullptr)
	{
		m_cameraUBO = new RenderBuffer(ctx, eRenderBufferUsageBit::UNIFORM_BUFFER_BIT, eRenderMemoryHint::MEMORY_HINT_DYNAMIC);
	}

	camera_data_t cameraData;
	cameraData.projection = m_projection;
	cameraData.view = m_view;
	cameraData.position = m_transform.m_position;
	
	m_cameraUBO->Update(&cameraData, sizeof(cameraData), sizeof(cameraData));
}

//////////////////////////////////////////////////////////////////////////
void Camera::UpdateViewMatrix()
{
	m_camModel = m_transform.ToMatrix(m_axisConvention);

    // world to local
    m_view = MatrixMultiply(GetAxisMatrixToConvention(m_axisConvention), MatrixInverseOrthoNormal(m_camModel));
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight)
{
	m_transform.SetPosition ((bottomLeft + topRight) * .5f);
	Vec2 size = topRight - bottomLeft;
	size.y = (size.y == 0.f) ? 1.f : size.y;
	m_aspectRatio = size.x / size.y;
	m_viewHeight = size.y;
	m_projection = Mat44::CreateOrthographicProjection(Vec3(bottomLeft, -1.f), Vec3(topRight, 1.f));
	UpdateViewMatrix();
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetPosition(Vec3 const& position)
{
	m_transform.SetPosition( position);
	UpdateViewMatrix();
}

//////////////////////////////////////////////////////////////////////////
void Camera::WorldTranslate(Vec3 const& translation)
{
	m_transform.Translate(translation);
	UpdateViewMatrix();
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetProjectionOrthographic(float height, float nearZ /*= -1.f*/, float farZ /*= 1.f*/)
{
	float aspect = GetAspectRaio();
	m_viewHeight = height;

	Vec2 halfDim(.5f*height*aspect,.5f*height);
    m_projection = Mat44::CreateOrthographicProjection(Vec3(- halfDim.x, - halfDim.y, nearZ),
        Vec3( halfDim.x, halfDim.y, farZ));
	UpdateViewMatrix();
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetProjectionPerspective(float fovDegrees, float nearZ /*= -0.1f*/, float farZ /*= -100.f*/)
{
	float aspect = GetAspectRaio();
	m_projection = MakePerspectiveProjectionMatrix3D(fovDegrees, aspect, nearZ, farZ);
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetAxisConvention(AxisConvention newConvention)
{
	m_axisConvention = newConvention;
	UpdateViewMatrix();
}


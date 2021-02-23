#pragma once

#include <map>
#include <string>
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"

class SpriteAnimDefinition;

class SpriteAnimSet
{
public:
	SpriteAnimSet( RenderContext& renderer, const XmlElement& spriteAnimSetElement);

	void           GetUVsForNameAtTime( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, const std::string& name, float seconds ) const;
	const Texture& GetTexture(const std::string& name) const;
	float          GetAspect(const std::string& name) const;

private:
	std::map<std::string, SpriteAnimDefinition*> m_animations;
};
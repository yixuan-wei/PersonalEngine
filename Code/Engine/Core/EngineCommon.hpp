#pragma once

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
constexpr int MAX_LIGHT_COUNT = 8;
constexpr int APPLY_IMPULSE_ITERATIONS = 7;
constexpr int CIRCLE_FRAGMENT_NUM = 32;
constexpr int VERTEX_DESCRIPTION_NUM = 32;
constexpr int MAX_REGISTERED_EVENTS = 500;
constexpr unsigned int MAX_PARTICLE_PER_EMITTER = 1000;
constexpr int MAX_EMITTERS_IN_SYSTEM = 50000;


//////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////
class DevConsole;
class NamedStrings;
class EventSystem;
class Network;

extern DevConsole* g_theConsole;
extern NamedStrings* g_gameConfigBlackboard;
extern EventSystem* g_theEvents;
extern Network* g_theNetwork;


//////////////////////////////////////////////////////////////////////////
// Alignment
#include "Engine/Math/Vec2.hpp"

const Vec2 ALIGN_CENTERED = Vec2( .5f, .5f );
const Vec2 ALIGN_BOTTOM_LEFT = Vec2( .0f, .0f );
const Vec2 ALIGN_BOTTOM_CENTER = Vec2( .5f, .0f );
const Vec2 ALIGN_BOTTOM_RIGHT = Vec2( 1.f, .0f );
const Vec2 ALIGN_CENTER_LEFT = Vec2( .0f, .5f );
const Vec2 ALIGN_CENTER_RIGHT = Vec2( 1.f, .5f );
const Vec2 ALIGN_TOP_LEFT = Vec2( .0f, 1.f );
const Vec2 ALIGN_TOP_CENTER = Vec2( .5f, 1.f );
const Vec2 ALIGN_TOP_RIGHT = Vec2( 1.f, 1.f );
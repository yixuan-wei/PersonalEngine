//-----------------------------------------------------------------------------------------------
// SmoothNoise.hpp
//
#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////
// Squirrel's Smooth Noise utilities (version 3)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following smoothed noise functions require supporting raw bit-noise functions, such as
//	those provided by the accompanying source file "RawNoise.hpp", #included by SmoothNoise.cpp.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in out-of-order (or or-demand) procedural
//	content generation (i.e. that mountain village is the same whether you generated it
//	first or last, ahead of time or just now).
//
// My implementations of fractal and Perlin noise include a few improvements over the stock
//	versions I've encountered:
//	* Functions can take seeds (independent of index/position) with unique-but-consistent results
//	* Each octave is offset (translation/bias) to dramatically reduce octaval harmony/feedback.
//	* Vector gradients are in power-of-two sets, to avoid modulus ops (bitwise masks instead)
//	* Octave persistence and scale are exposed and adjustable (not necessarily 0.5 and 2.0)
//	* Multi-octave noise can be "normalized" to be eased back to within [-1,1] (or not)
//
// Note: these functions assume the presence of a few simple math utility functions and classes;
//	class or struct Vec2, Vec3, Vec4					// public x,y,z,w floats & basic operators
//	float DotProduct2D( const Vec2& A, const Vec2& B )	// (Ax*Bx) + (Ay*By)
//	float DotProduct3D( const Vec3& A, const Vec3& B )	// (Ax*Bx) + (Ay*By) + (Az*Bz)
//	float DotProduct4D( const Vec4& A, const Vec4& B )	// (Ax*Bx) + (Ay*By) + (Az*Bz) + (Aw*Bw)
//	float SmoothStep3( float inputZeroToOne )			// 3t^2 - 2t^3  where "^" means "to the power of"
//
// Feel free to modify SmoothNoise.cpp, SmoothNoise.hpp, and/or RawNoise.hpp to suit your math
//	library class, function, and variable names.
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Smooth/fractal pseudorandom noise functions (random-access / deterministic)
//
// These are less "organic" (and more axial) than Perlin's functions, but simpler and faster.
// 
// <numOctaves>			Number of layers of noise added together
// <octavePersistence>	Amplitude multiplier for each subsequent octave (each octave is quieter)
// <octaveScale>		Frequency multiplier for each subsequent octave (each octave is busier)
// <renormalize>		If true, uses nonlinear (SmoothStep3) renormalization to within [-1,1]
//
float Compute1dFractalNoise( float position, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute2dFractalNoise( float posX, float posY, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute3dFractalNoise( float posX, float posY, float posZ, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute4dFractalNoise( float posX, float posY, float posZ, float posT, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );


//-----------------------------------------------------------------------------------------------
// Perlin noise functions (random-access / deterministic)
//
// Perlin noise is slightly more expensive, but more organic-looking (less axial) than regular
//	square fractal noise, through the use of blended dot products vs. randomized gradient vectors.
//
// <numOctaves>			Number of layers of noise added together
// <octavePersistence>	Amplitude multiplier for each subsequent octave (each octave is quieter)
// <octaveScale>		Frequency multiplier for each subsequent octave (each octave is busier)
// <renormalize>		If true, uses nonlinear (SmoothStep3) renormalization to within [-1,1]
//
float Compute1dPerlinNoise( float position, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute2dPerlinNoise( float posX, float posY, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute3dPerlinNoise( float posX, float posY, float posZ, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );
float Compute4dPerlinNoise( float posX, float posY, float posZ, float posT, float scale=1.f, unsigned int numOctaves=1, float octavePersistence=0.5f, float octaveScale=2.f, bool renormalize=true, unsigned int seed=0 );


//-----------------------------------------------------------------------------------------------
// Simplex noise functions (random-access / deterministic)
//
// Simplex Noise (also by Ken Perlin) is theoretically faster than - and supposedly superior to - 
//	Perlin noise, in that it is more organic-looking.  I'm not sure I like the look of it better,
//	however; cross-sections of 4D simplex noise definitely look worse to me than 4D Perlin does!
//
// Also, higher-dimensional cross-sections of Simplex noise look different than their lower-
//	dimensional counterparts, which I dislike.
//
// I'm also not empirically certain firsthand (or entirely convinced) that it's actually
//	that much faster (if at all) in 2D and 3D (my primary use-cases).  Need to test.
//
// #TODO: Implement simplex noise in 2D, 3D, 4D (1D simplex is identical to 1D Perlin, I think?)
// #TODO: Test comparative simplex noise implementation in 2D/3D to compare speeds.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
// WARNING: 3D+ Simplex Noise for texture/image synthesis is protected by U.S. Patent 6,867,776!
/////////////////////////////////////////////////////////////////////////////////////////////////



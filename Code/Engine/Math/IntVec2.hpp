#pragma once


//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

	static const IntVec2 ZERO;
	static const IntVec2 ONE;

public:
	// Construction/Destruction
	~IntVec2() = default;								// destructor (do nothing)
	IntVec2() = default;								// default constructor (do nothing)
	IntVec2( const IntVec2& copyFrom );					// copy constructor (from another IntVec2)
	explicit IntVec2( int initialX, int initialY );		// explicit constructor (from x, y)

	void SetFromText( const char* text );
	
	// Accessors const methods
	float		    GetLength() const;
	int		        GetLengthSquared() const;
	int             GetTaxicabLength() const;
	float	        GetOrientationRadians() const;
	float	        GetOrientationDegrees() const;
	const IntVec2	GetRotated90Degrees() const;
	const IntVec2	GetRotatedMinus90Degrees() const;

	void		    Rotate90Degrees();
	void		    RotateMinus90Degrees();

	// Operators (const)
	bool		    operator==( const IntVec2& compare ) const;		// IntVec2 == IntVec2
	bool		    operator!=( const IntVec2& compare ) const;		// IntVec2 != IntVec2
	const IntVec2	operator+( const IntVec2& vecToAdd ) const;		// IntVec2 + IntVec2
	const IntVec2	operator-( const IntVec2& vecToSubtract ) const;	// IntVec2 - IntVec2
	const IntVec2	operator-() const;								// -IntVec2, i.e. "unary negation"
	const IntVec2	operator*( int uniformScale ) const;			// IntVec2 * int
	const IntVec2	operator*( const IntVec2& vecToMultiply ) const;	// IntVec2 * IntVec2

	// Operators (self-mutating / non-const)
	void		operator+=( const IntVec2& vecToAdd );				// IntVec2 += IntVec2
	void		operator-=( const IntVec2& vecToSubtract );		// IntVec2 -= IntVec2
	void		operator*=( const int uniformScale );			// IntVec2 *= int
	void		operator=( const IntVec2& copyFrom );				// IntVec2 = IntVec2

	// Standalone "friend" functions that are conceptually, but not actually, part of IntVec2::
	friend const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale );	// int * IntVec2
};



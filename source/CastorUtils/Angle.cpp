#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Angle.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;
using namespace Castor::Utils;

//*************************************************************************************************

real Angle :: Pi = real( 3.1415926535897932384626433832795028841968);
double Angle :: PiDouble = 3.1415926535897932384626433832795028841968;
float Angle :: PiFloat = real( 3.1415926535897932384626433832795028841968);
real Angle :: PiDiv2 = Angle::Pi / real( 2.0);
real Angle :: PiMult2 = Angle::Pi * real( 2.0);

real Angle :: RadianToDegree = real( 180.0) / real( Angle::Pi);
real Angle :: RadianToGrad = real( 200.0) / real( Angle::Pi);
real Angle :: DegreeToRadian = real( Angle::Pi) / real( 180.0);
real Angle :: DegreeToGrad = real( 200) / real( 180.0);
real Angle :: GradToRadian = real( Angle::Pi) / real( 200.0);
real Angle :: GradToDegree = real( 180) / real( 200.0);

Angle Angle :: FromDegrees( real p_degrees)
{
	return Angle( p_degrees * DegreeToRadian);
}

Angle Angle :: FromRadians( real p_radians)
{
	return Angle( p_radians);
}

Angle Angle :: FromGrads( real p_grads)
{
	return Angle( p_grads * GradToRadian);
}

Angle :: Angle()
	:	m_rAngle( 0)
{
}

Angle :: Angle( real p_radians)
	:	m_rAngle( p_radians)
{
}

Angle :: Angle( Angle const & p_angle)
	:	m_rAngle( p_angle.m_rAngle)
{
}

Angle :: ~Angle()
{
}

real Angle :: Degrees()const
{
	return m_rAngle * RadianToDegree;
}

real Angle :: Radians()const
{
	return m_rAngle;
}

real Angle :: Grads()const
{
	return m_rAngle * RadianToGrad;
}

Angle & Angle :: Degrees( real p_rAngle)
{
	m_rAngle = p_rAngle * DegreeToRadian;
	return * this;
}

Angle & Angle :: Radians( real p_rAngle)
{
	m_rAngle = p_rAngle;
	return * this;
}

Angle & Angle :: Grads( real p_rAngle)
{
	m_rAngle = p_rAngle * GradToRadian;
	return * this;
}

real Angle :: Cos()const
{
	return cos( m_rAngle);
}

real Angle :: Sin()const
{
	return sin( m_rAngle);
}

real Angle :: Tan()const
{
	return tan( m_rAngle);
}

real Angle :: Cosh()const
{
	return cosh( m_rAngle);
}

real Angle :: Sinh()const
{
	return sinh( m_rAngle);
}

real Angle :: Tanh()const
{
	return tanh( m_rAngle);
}

Angle & Angle :: ACos( real p_rValue)
{
	m_rAngle = acos( p_rValue);
	return * this;
}

Angle & Angle :: ASin( real p_rValue)
{
	m_rAngle = asin( p_rValue);
	return * this;
}

Angle & Angle :: ATan( real p_rValue)
{
	m_rAngle = atan( p_rValue);
	return * this;
}

Angle & Angle :: operator =( Angle const & p_angle)
{
	m_rAngle = p_angle.m_rAngle;
	return * this;
}

Angle & Angle :: operator +=( Angle const & p_angle)
{
	m_rAngle += p_angle.m_rAngle;
	return * this;
}

Angle & Angle :: operator -=( Angle const & p_angle)
{
	m_rAngle -= p_angle.m_rAngle;
	return * this;
}

Angle & Angle :: operator *=( Angle const & p_angle)
{
	m_rAngle *= p_angle.m_rAngle;
	return * this;
}

Angle & Angle :: operator /=( Angle const & p_angle)
{
	m_rAngle /= p_angle.m_rAngle;
	return * this;
}

BEGIN_SERIALISE_MAP( Angle, Serialisable)
	ADD_ELEMENT( m_rAngle)
END_SERIALISE_MAP()

//*************************************************************************************************

Angle Castor::Math :: operator +( Angle const & p_angleA, Angle const & p_angleB)
{
	Angle l_return( p_angleA);
	l_return += p_angleB;
	return l_return;
}

Angle Castor::Math :: operator -( Angle const & p_angleA, Angle const & p_angleB)
{
	Angle l_return( p_angleA);
	l_return -= p_angleB;
	return l_return;
}

Angle Castor::Math :: operator *( Angle const & p_angleA, Angle const & p_angleB)
{
	Angle l_return( p_angleA);
	l_return *= p_angleB;
	return l_return;
}

Angle Castor::Math :: operator /( Angle const & p_angleA, Angle const & p_angleB)
{
	Angle l_return( p_angleA);
	l_return /= p_angleB;
	return l_return;
}

//*************************************************************************************************
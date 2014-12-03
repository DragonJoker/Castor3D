#include "Angle.hpp"

namespace Castor
{
	const double	Angle::PiDouble		= 3.1415926535897932384626433832795028841968;
	const real		Angle::Pi				= real( PiDouble );
	const float		Angle::PiFloat		= float( PiDouble );
	const real		Angle::PiDiv2			= Angle::Pi / 2;
	const real		Angle::PiMult2		= Angle::Pi * 2;
	const double	Angle::RadianToDegree	= 180 / Angle::PiDouble;
	const double	Angle::RadianToGrad	= 200 / Angle::PiDouble;
	const double	Angle::DegreeToRadian	= Angle::PiDouble / 180;
	const double	Angle::DegreeToGrad	= 200.0 / 180.0;
	const double	Angle::GradToRadian	= Angle::PiDouble / 200;
	const double	Angle::GradToDegree	= 180.0 / 200.0;

	Angle::Angle()
		:	m_rAngle( 0	)
	{
	}

	Angle::Angle( Angle const & p_angle )
		:	m_rAngle( p_angle.m_rAngle	)
	{
	}

	Angle::Angle( Angle && p_angle )
		:	m_rAngle( 0	)
	{
		m_rAngle = std::move( p_angle.m_rAngle );
		p_angle.m_rAngle = 0;
	}

	Angle::~Angle()
	{
	}

	real Angle::Degrees()const
	{
		return m_rAngle * real( RadianToDegree );
	}

	real Angle::Radians()const
	{
		return m_rAngle;
	}

	real Angle::Grads()const
	{
		return m_rAngle * real( RadianToGrad );
	}

	void Angle::Degrees( real p_rAngle )
	{
		m_rAngle = p_rAngle * real( DegreeToRadian );
	}

	void Angle::Radians( real p_rAngle )
	{
		m_rAngle = p_rAngle;
	}

	void Angle::Grads( real p_rAngle )
	{
		m_rAngle = p_rAngle * real( GradToRadian );
	}

	real Angle::Cos()const
	{
		return cos( m_rAngle );
	}

	real Angle::Sin()const
	{
		return sin( m_rAngle );
	}

	real Angle::Tan()const
	{
		return tan( m_rAngle );
	}

	real Angle::Cosh()const
	{
		return cosh( m_rAngle );
	}

	real Angle::Sinh()const
	{
		return sinh( m_rAngle );
	}

	real Angle::Tanh()const
	{
		return tanh( m_rAngle );
	}

	void Angle::ACos( real p_rValue )
	{
		m_rAngle = acos( p_rValue );
	}

	void Angle::ASin( real p_rValue )
	{
		m_rAngle = asin( p_rValue );
	}

	void Angle::ATan( real p_rValue )
	{
		m_rAngle = atan( p_rValue );
	}

	Angle & Angle::operator =( Angle const & p_angle )
	{
		m_rAngle = p_angle.m_rAngle;
		return *this;
	}

	Angle & Angle::operator =( Angle && p_angle )
	{
		if ( this != &p_angle )
		{
			m_rAngle = p_angle.m_rAngle;
			p_angle.m_rAngle = 0;
		}

		return *this;
	}

	Angle & Angle::operator +=( Angle const & p_angle )
	{
		m_rAngle += p_angle.m_rAngle;
		return *this;
	}

	Angle & Angle::operator -=( Angle const & p_angle )
	{
		m_rAngle -= p_angle.m_rAngle;
		return *this;
	}

	Angle & Angle::operator *=( Angle const & p_angle )
	{
		m_rAngle *= p_angle.m_rAngle;
		return *this;
	}

	Angle & Angle::operator /=( Angle const & p_angle )
	{
		m_rAngle /= p_angle.m_rAngle;
		return *this;
	}

	Angle & Angle::operator *=( real p_scalar )
	{
		m_rAngle *= p_scalar;
		return *this;
	}

	Angle & Angle::operator /=( real p_scalar )
	{
		m_rAngle /= p_scalar;
		return *this;
	}

	bool operator ==( Angle const & p_angleA, Angle const & p_angleB )
	{
		return std::abs( p_angleA.m_rAngle - p_angleB.m_rAngle ) < std::numeric_limits< real >::epsilon();
	}

	bool operator !=( Angle const & p_angleA, Angle const & p_angleB )
	{
		return !( p_angleA == p_angleB );
	}

	Angle operator +( Angle const & p_angleA, Angle const & p_angleB )
	{
		Angle l_return( p_angleA );
		l_return += p_angleB;
		return l_return;
	}

	Angle operator -( Angle const & p_angleA, Angle const & p_angleB )
	{
		Angle l_return( p_angleA );
		l_return -= p_angleB;
		return l_return;
	}

	Angle operator *( Angle const & p_angleA, Angle const & p_angleB )
	{
		Angle l_return( p_angleA );
		l_return *= p_angleB;
		return l_return;
	}

	Angle operator /( Angle const & p_angleA, Angle const & p_angleB )
	{
		Angle l_return( p_angleA );
		l_return /= p_angleB;
		return l_return;
	}

	Angle operator *( Angle const & p_angle, real p_scalar )
	{
		Angle l_return( p_angle );
		l_return *= p_scalar;
		return l_return;
	}

	Angle operator /( Angle const & p_angle, real p_scalar )
	{
		Angle l_return( p_angle );
		l_return /= p_scalar;
		return l_return;
	}
}


#include "Angle.hpp"

namespace Castor
{
	const double	Angle::Pi				= 3.1415926535897932384626433832795028841968;
	const double	Angle::PiDiv2			= Angle::Pi / 2;
	const double	Angle::PiMult2			= Angle::Pi * 2;
	const double	Angle::RadianToDegree	= 57.295779513082320876798154814105;
	const double	Angle::RadianToGrad		= 200 / Angle::Pi;
	const double	Angle::DegreeToRadian	= 0.01745329251994329576923690768489;
	const double	Angle::DegreeToGrad		= 200.0 / 180.0;
	const double	Angle::GradToRadian		= Angle::Pi / 200;
	const double	Angle::GradToDegree		= 180.0 / 200.0;

	Angle::Angle()
		:	m_radians( 0	)
	{
	}

	Angle::Angle( Angle const & p_angle )
		:	m_radians( p_angle.m_radians	)
	{
	}

	Angle::Angle( Angle && p_angle )
		:	m_radians( 0	)
	{
		m_radians = std::move( p_angle.m_radians );
		p_angle.m_radians = 0;
	}

	Angle::~Angle()
	{
	}

	double Angle::Degrees()const
	{
		return m_radians * RadianToDegree;
	}

	double Angle::Radians()const
	{
		return m_radians;
	}

	double Angle::Grads()const
	{
		return m_radians * RadianToGrad;
	}

	void Angle::Degrees( double p_rAngle )
	{
		m_radians = p_rAngle * DegreeToRadian;
	}

	void Angle::Radians( double p_rAngle )
	{
		m_radians = p_rAngle;
	}

	void Angle::Grads( double p_rAngle )
	{
		m_radians = p_rAngle * GradToRadian;
	}

	double Angle::Cos()const
	{
		return cos( m_radians );
	}

	double Angle::Sin()const
	{
		return sin( m_radians );
	}

	double Angle::Tan()const
	{
		return tan( m_radians );
	}

	double Angle::Cosh()const
	{
		return cosh( m_radians );
	}

	double Angle::Sinh()const
	{
		return sinh( m_radians );
	}

	double Angle::Tanh()const
	{
		return tanh( m_radians );
	}

	void Angle::ACos( double p_rValue )
	{
		m_radians = acos( p_rValue );
	}

	void Angle::ASin( double p_rValue )
	{
		m_radians = asin( p_rValue );
	}

	void Angle::ATan( double p_rValue )
	{
		m_radians = atan( p_rValue );
	}

	Angle & Angle::operator =( Angle const & p_angle )
	{
		m_radians = p_angle.m_radians;
		return *this;
	}

	Angle & Angle::operator =( Angle && p_angle )
	{
		if ( this != &p_angle )
		{
			m_radians = p_angle.m_radians;
			p_angle.m_radians = 0;
		}

		return *this;
	}

	Angle & Angle::operator +=( Angle const & p_angle )
	{
		m_radians += p_angle.m_radians;
		return *this;
	}

	Angle & Angle::operator -=( Angle const & p_angle )
	{
		m_radians -= p_angle.m_radians;
		return *this;
	}

	Angle & Angle::operator *=( Angle const & p_angle )
	{
		m_radians *= p_angle.m_radians;
		return *this;
	}

	Angle & Angle::operator /=( Angle const & p_angle )
	{
		m_radians /= p_angle.m_radians;
		return *this;
	}

	Angle & Angle::operator *=( double p_scalar )
	{
		m_radians *= p_scalar;
		return *this;
	}

	Angle & Angle::operator /=( double p_scalar )
	{
		m_radians /= p_scalar;
		return *this;
	}

	bool operator ==( Angle const & p_angleA, Angle const & p_angleB )
	{
		return std::abs( p_angleA.m_radians - p_angleB.m_radians ) < std::numeric_limits< double >::epsilon();
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

	Angle operator *( Angle const & p_angle, double p_scalar )
	{
		Angle l_return( p_angle );
		l_return *= p_scalar;
		return l_return;
	}

	Angle operator /( Angle const & p_angle, double p_scalar )
	{
		Angle l_return( p_angle );
		l_return /= p_scalar;
		return l_return;
	}
}


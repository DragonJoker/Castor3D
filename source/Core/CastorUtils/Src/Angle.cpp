#include "Angle.hpp"

namespace Castor
{
	const double Angle::Pi = 3.1415926535897932384626433832795028841968;
	const double Angle::PiDiv2 = Angle::Pi / 2;
	const double Angle::PiMult2 = Angle::Pi * 2;
	const double Angle::RadianToDegree = 57.295779513082320876798154814105;
	const double Angle::RadianToGrad = 200 / Angle::Pi;
	const double Angle::DegreeToRadian = 0.01745329251994329576923690768489;
	const double Angle::DegreeToGrad = 200.0 / 180.0;
	const double Angle::GradToRadian = Angle::Pi / 200;
	const double Angle::GradToDegree = 180.0 / 200.0;

	Angle::Angle()
		: Angle( 0 )
	{
	}

	Angle::Angle( Angle const & p_angle )
		: Angle( p_angle.m_radians )
	{
	}

	Angle::Angle( Angle && p_angle )
		: m_radians( 0 )
	{
		m_radians = std::move( p_angle.m_radians );
		p_angle.m_radians = 0;
	}

	Angle::~Angle()
	{
	}

	double Angle::degrees()const
	{
		return m_radians * RadianToDegree;
	}

	double Angle::radians()const
	{
		return m_radians;
	}

	double Angle::grads()const
	{
		return m_radians * RadianToGrad;
	}

	void Angle::degrees( double p_rAngle )
	{
		m_radians = p_rAngle * DegreeToRadian;
	}

	void Angle::radians( double p_rAngle )
	{
		m_radians = p_rAngle;
	}

	void Angle::grads( double p_rAngle )
	{
		m_radians = p_rAngle * GradToRadian;
	}

	double Angle::cos()const
	{
		return ::cos( m_radians );
	}

	double Angle::sin()const
	{
		return ::sin( m_radians );
	}

	double Angle::tan()const
	{
		return ::tan( m_radians );
	}

	double Angle::cosh()const
	{
		return ::cosh( m_radians );
	}

	double Angle::sinh()const
	{
		return ::sinh( m_radians );
	}

	double Angle::tanh()const
	{
		return ::tanh( m_radians );
	}

	void Angle::acos( double p_rValue )
	{
		m_radians = ::acos( p_rValue );
	}

	void Angle::asin( double p_rValue )
	{
		m_radians = ::asin( p_rValue );
	}

	void Angle::atan( double p_rValue )
	{
		m_radians = ::atan( p_rValue );
	}

	Angle & Angle::operator=( Angle const & p_rhs )
	{
		m_radians = p_rhs.m_radians;
		return *this;
	}

	Angle & Angle::operator=( Angle && p_rhs )
	{
		if ( this != &p_rhs )
		{
			m_radians = p_rhs.m_radians;
			p_rhs.m_radians = 0;
		}

		return *this;
	}

	Angle & Angle::operator+=( Angle const & p_rhs )
	{
		m_radians += p_rhs.m_radians;
		return *this;
	}

	Angle & Angle::operator-=( Angle const & p_rhs )
	{
		m_radians -= p_rhs.m_radians;
		return *this;
	}

	Angle & Angle::operator*=( Angle const & p_rhs )
	{
		m_radians *= p_rhs.m_radians;
		return *this;
	}

	Angle & Angle::operator/=( Angle const & p_rhs )
	{
		m_radians /= p_rhs.m_radians;
		return *this;
	}

	Angle & Angle::operator*=( double p_rhs )
	{
		m_radians *= p_rhs;
		return *this;
	}

	Angle & Angle::operator/=( double p_rhs )
	{
		m_radians /= p_rhs;
		return *this;
	}

	bool operator==( Angle const & p_lhs, Angle const & p_rhs )
	{
		return std::abs( p_lhs.m_radians - p_rhs.m_radians ) < std::numeric_limits< double >::epsilon();
	}

	bool operator!=( Angle const & p_lhs, Angle const & p_rhs )
	{
		return !( p_lhs == p_rhs );
	}

	Angle operator+( Angle const & p_lhs, Angle const & p_rhs )
	{
		Angle l_return( p_lhs );
		l_return += p_rhs;
		return l_return;
	}

	Angle operator-( Angle const & p_lhs, Angle const & p_rhs )
	{
		Angle l_return( p_lhs );
		l_return -= p_rhs;
		return l_return;
	}

	Angle operator*( Angle const & p_lhs, Angle const & p_rhs )
	{
		Angle l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	Angle operator/( Angle const & p_lhs, Angle const & p_rhs )
	{
		Angle l_return( p_lhs );
		l_return /= p_rhs;
		return l_return;
	}

	Angle operator*( Angle const & p_lhs, double p_rhs )
	{
		Angle l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	Angle operator/( Angle const & p_lhs, double p_rhs )
	{
		Angle l_return( p_lhs );
		l_return /= p_rhs;
		return l_return;
	}
}

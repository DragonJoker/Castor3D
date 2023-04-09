/*
See LICENSE file in root folder
*/
#ifndef ___CU_MathModule_HPP___
#define ___CU_MathModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

#include "Math.hpp"

namespace castor
{
	/**@name Math */
	//@{
	enum class LengthUnit
	{
		eKilometre,
		eMetre,
		eCentimetre,
		eMillimetre,
		eYard,
		eFoot,
		eInch,
		CU_ScopedEnumBounds( eKilometre )
	};
	CU_API String getName( LengthUnit value );

	template< typename Type >
	static constexpr Type Pi{ Type( 3.1415926535897932384626433832795028841968 ) };
	template< typename Type >
	static constexpr Type PiMult2{ Type( 3.1415926535897932384626433832795028841968 * 2.0 ) };
	template< typename Type >
	static constexpr Type Tau{ Type( 3.1415926535897932384626433832795028841968 * 2.0 ) };
	template< typename Type >
	static constexpr Type PiDiv2{ Type( 3.1415926535897932384626433832795028841968 / 2.0 ) };
	/**
	\~english
	\brief		Angle helper class.
	\remark		Handles angle units conversions.
	\~french
	\brief		Classe d'angle.
	\remark		Gère les conversions d'unités d'angles.
	*/
	template< typename TypeT >
	class AngleT;
	/**
	\~english
	\brief		Templated static dimensions point representation
	\remark		Can hold any type which has a defined Policy
	\~french
	\brief		Classe de points à dimensions statiques
	\remark		Peut recevoir les types de données qui ont une Policy créée
	*/
	template< typename T, uint32_t Count >
	class Coords;
	/**
	\~english
	\brief		Length helper class.
	\remark		Handles length units conversions.
	\~french
	\brief		Classe de longueur.
	\remark		Gère les conversions d'unités de longueurs.
	*/
	template< typename TypeT >
	class LengthT;
	/**
	\~english
	\brief		2D line equation
	\remark		do you remember y = ax + b ?
	\~french
	\brief		Equation d'une ligne 2D
	\remark		Vous connaissez y = ax + b ?
	*/
	template< typename T >
	class Line2D;
	/**
	\~english
	\brief		3D line equation
	\remark		A slope and an origin
				<br />x = m_slope[0] * t + m_origin[0]
				<br />y = m_slope[1] * t + m_origin[1]
				<br />z = m_slope[2] * t + m_origin[2]
	\~french
	\brief		Equation d'une droite 3D
	\remark		Une pente et une origine
				<br />x = m_slope[0] * t + m_origin[0]
				<br />y = m_slope[1] * t + m_origin[1]
				<br />z = m_slope[2] * t + m_origin[2]
	*/
	template< typename T >
	class Line3D;
	/**
	\~english
	\brief		Templated column major matrix representation.
	\remark		Can hold any type which has a defined Policy.
	\~french
	\brief		Représentation d'une matrice column major, le type des éléments et les dimensions de la matrice sont en template.
	\remark		Peut contenir n'importe quel élément qui a une castor::Policy.
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	class Matrix;
	/**
	\~english
	\brief		Plane equation implementation (ax + by + cz + d = 0).
	\~french
	\brief		Implémentation d'une équation de plan (ax + by + cz + d = 0).
	*/
	class PlaneEquation;
	/**
	\~english
	\brief		Templated static dimensions point representation
	\remark		Can hold any type which has a defined Policy
	\~french
	\brief		Classe de points à dimensions statiques
	\remark		Peut recevoir les types de données qui ont une Policy créée
	*/
	template< typename T, uint32_t TCount >
	class Point;
	/**
	\~english
	\brief		Data holder for a point.
	\~french
	\brief		Conteneur des données d'un point.
	*/
	template< typename T, uint32_t TCount >
	struct PointData;
	/**
	\~english
	\brief		Point operators.
	\~french
	\brief		Opérateurs de Point.
	*/
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtOperators;
	/**
	\~english
	\brief		Point assignment operators.
	\~french
	\brief		Opérateurs d'affectation de Point.
	*/
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtAssignOperators;
	/**
	\~english
	\brief		Quaternion representation class
	\remark		A quaternion can be seen as an axis and an angle.
	\~french
	\brief		Représentation d'un quaternion
	\remark		Un quaternion peut se voir comme un axe et un angle.
	*/
	template< typename T >
	class QuaternionT;
	/**
	\~english
	\brief		A range class.
	\~french
	\brief		Une classe d'intervalle.
	*/
	template< typename T >
	class Range;
	/**
	\~english
	\brief		A value inside a range.
	\remarks	The value can't get out of its range.
	\~french
	\brief		Représente une valeur dans un intervalle.
	\remarks	La valeur ne peut pas sortir de son intervalle.
	*/
	template< typename T >
	class RangedValue;
	/**
	\~english
	\brief		A range inside a sequence.
	\~french
	\brief		Un intervalle dans une séquence d'intervalles.
	*/
	template< typename T >
	class IndexedRangeT;
	/**
	\~english
	\brief		A contiguous ranges sequence.
	\~french
	\brief		Une séquence continue d'intervalles.
	*/
	template< typename T >
	class RangeSequenceT;
	/**
	\~english
	\brief		Vertex, expressed in rho theta and phi (Euler angles)
	\~french
	\brief		Vertex, exprimé en rho theta et phi (angles d'Euler)
	*/
	class SphericalVertex;
	/**
	\~english
	\brief		The traits for aspeed.
	\~french
	\brief		Les traits pour une vitesse.
	*/
	template< typename ValueT >
	struct SpeedTraitsT;
	/**
	\~english
	\brief		A speed.
	\~french
	\brief		Une vitesse.
	*/
	template< typename ValueT, typename DurationT, typename TraitsT = SpeedTraitsT< ValueT > >
	class SpeedT;
	/**
	\~english
	\brief		Templated column major square matrix representation
	\~french
	\brief		Représentation d'une matrice carrée column major
	*/
	template< typename T, uint32_t Count >
	class SquareMatrix;

	template< typename T > using Point2 = Point< T, 2 >;
	template< typename T > using Point3 = Point< T, 3 >;
	template< typename T > using Point4 = Point< T, 4 >;

	template< typename T > using Coords2 = Coords< T, 2 >;
	template< typename T > using Coords3 = Coords< T, 3 >;
	template< typename T > using Coords4 = Coords< T, 4 >;

	template< typename T > using Matrix2x2 = SquareMatrix< T, 2 >;
	template< typename T > using Matrix3x3 = SquareMatrix< T, 3 >;
	template< typename T > using Matrix4x4 = SquareMatrix< T, 4 >;

	using Angle = AngleT< float >;
	using Length = LengthT< float >;
	using Quaternion = QuaternionT< float >;

	CU_DeclarePoint( bool, 4, b );
	CU_DeclarePoint( bool, 3, b );
	CU_DeclarePoint( bool, 2, b );
	CU_DeclarePoint( int8_t, 4, c );
	CU_DeclarePoint( int8_t, 3, c );
	CU_DeclarePoint( int8_t, 2, c );
	CU_DeclarePoint( uint8_t, 4, ub );
	CU_DeclarePoint( uint8_t, 3, ub );
	CU_DeclarePoint( uint8_t, 2, ub );
	CU_DeclarePoint( int16_t, 4, s );
	CU_DeclarePoint( int16_t, 3, s );
	CU_DeclarePoint( int16_t, 2, s );
	CU_DeclarePoint( uint16_t, 4, us );
	CU_DeclarePoint( uint16_t, 3, us );
	CU_DeclarePoint( uint16_t, 2, us );
	CU_DeclarePoint( int32_t, 4, i );
	CU_DeclarePoint( int32_t, 3, i );
	CU_DeclarePoint( int32_t, 2, i );
	CU_DeclarePoint( uint32_t, 4, ui );
	CU_DeclarePoint( uint32_t, 3, ui );
	CU_DeclarePoint( uint32_t, 2, ui );
	CU_DeclarePoint( float, 4, f );
	CU_DeclarePoint( float, 3, f );
	CU_DeclarePoint( float, 2, f );
	CU_DeclarePoint( double, 4, d );
	CU_DeclarePoint( double, 3, d );
	CU_DeclarePoint( double, 2, d );

	using Vector2f = LengthT< Point2f >;
	using Vector3f = LengthT< Point3f >;
	using Vector4f = LengthT< Point4f >;
	using Vector2d = LengthT< Point2d >;
	using Vector3d = LengthT< Point3d >;
	using Vector4d = LengthT< Point4d >;

	CU_DeclareCoord( bool, 4, b );
	CU_DeclareCoord( bool, 3, b );
	CU_DeclareCoord( bool, 2, b );
	CU_DeclareCoord( int8_t, 4, c );
	CU_DeclareCoord( int8_t, 3, c );
	CU_DeclareCoord( int8_t, 2, c );
	CU_DeclareCoord( uint8_t, 4, ub );
	CU_DeclareCoord( uint8_t, 3, ub );
	CU_DeclareCoord( uint8_t, 2, ub );
	CU_DeclareCoord( int16_t, 4, s );
	CU_DeclareCoord( int16_t, 3, s );
	CU_DeclareCoord( int16_t, 2, s );
	CU_DeclareCoord( uint16_t, 4, us );
	CU_DeclareCoord( uint16_t, 3, us );
	CU_DeclareCoord( uint16_t, 2, us );
	CU_DeclareCoord( int32_t, 4, i );
	CU_DeclareCoord( int32_t, 3, i );
	CU_DeclareCoord( int32_t, 2, i );
	CU_DeclareCoord( uint32_t, 4, ui );
	CU_DeclareCoord( uint32_t, 3, ui );
	CU_DeclareCoord( uint32_t, 2, ui );
	CU_DeclareCoord( int32_t, 4, i );
	CU_DeclareCoord( int32_t, 3, i );
	CU_DeclareCoord( int32_t, 2, i );
	CU_DeclareCoord( float, 4, f );
	CU_DeclareCoord( float, 3, f );
	CU_DeclareCoord( float, 2, f );
	CU_DeclareCoord( double, 4, d );
	CU_DeclareCoord( double, 3, d );
	CU_DeclareCoord( double, 2, d );
	CU_DeclareCoord( uint8_t, 4, ub );
	CU_DeclareCoord( uint8_t, 3, ub );
	CU_DeclareCoord( uint8_t, 2, ub );

	CU_DeclareConstCoord( bool, 4, b );
	CU_DeclareConstCoord( bool, 3, b );
	CU_DeclareConstCoord( bool, 2, b );
	CU_DeclareConstCoord( int8_t, 4, c );
	CU_DeclareConstCoord( int8_t, 3, c );
	CU_DeclareConstCoord( int8_t, 2, c );
	CU_DeclareConstCoord( uint8_t, 4, ub );
	CU_DeclareConstCoord( uint8_t, 3, ub );
	CU_DeclareConstCoord( uint8_t, 2, ub );
	CU_DeclareConstCoord( int16_t, 4, s );
	CU_DeclareConstCoord( int16_t, 3, s );
	CU_DeclareConstCoord( int16_t, 2, s );
	CU_DeclareConstCoord( uint16_t, 4, us );
	CU_DeclareConstCoord( uint16_t, 3, us );
	CU_DeclareConstCoord( uint16_t, 2, us );
	CU_DeclareConstCoord( int32_t, 4, i );
	CU_DeclareConstCoord( int32_t, 3, i );
	CU_DeclareConstCoord( int32_t, 2, i );
	CU_DeclareConstCoord( uint32_t, 4, ui );
	CU_DeclareConstCoord( uint32_t, 3, ui );
	CU_DeclareConstCoord( uint32_t, 2, ui );
	CU_DeclareConstCoord( int32_t, 4, i );
	CU_DeclareConstCoord( int32_t, 3, i );
	CU_DeclareConstCoord( int32_t, 2, i );
	CU_DeclareConstCoord( float, 4, f );
	CU_DeclareConstCoord( float, 3, f );
	CU_DeclareConstCoord( float, 2, f );
	CU_DeclareConstCoord( double, 4, d );
	CU_DeclareConstCoord( double, 3, d );
	CU_DeclareConstCoord( double, 2, d );
	CU_DeclareConstCoord( uint8_t, 4, ub );
	CU_DeclareConstCoord( uint8_t, 3, ub );
	CU_DeclareConstCoord( uint8_t, 2, ub );

	CU_DeclareSqMtx( bool, 4, b );
	CU_DeclareSqMtx( bool, 3, b );
	CU_DeclareSqMtx( bool, 2, b );
	CU_DeclareSqMtx( int32_t, 4, i );
	CU_DeclareSqMtx( int32_t, 3, i );
	CU_DeclareSqMtx( int32_t, 2, i );
	CU_DeclareSqMtx( uint32_t, 4, ui );
	CU_DeclareSqMtx( uint32_t, 3, ui );
	CU_DeclareSqMtx( uint32_t, 2, ui );
	CU_DeclareSqMtx( float, 4, f );
	CU_DeclareSqMtx( float, 3, f );
	CU_DeclareSqMtx( float, 2, f );
	CU_DeclareSqMtx( double, 4, d );
	CU_DeclareSqMtx( double, 3, d );
	CU_DeclareSqMtx( double, 2, d );

	CU_DeclareMtx( bool, 2, 3, b );
	CU_DeclareMtx( bool, 2, 4, b );
	CU_DeclareMtx( bool, 3, 2, b );
	CU_DeclareMtx( bool, 3, 4, b );
	CU_DeclareMtx( bool, 4, 2, b );
	CU_DeclareMtx( bool, 4, 3, b );
	CU_DeclareMtx( int32_t, 2, 3, i );
	CU_DeclareMtx( int32_t, 2, 4, i );
	CU_DeclareMtx( int32_t, 3, 2, i );
	CU_DeclareMtx( int32_t, 3, 4, i );
	CU_DeclareMtx( int32_t, 4, 2, i );
	CU_DeclareMtx( int32_t, 4, 3, i );
	CU_DeclareMtx( uint32_t, 2, 3, ui );
	CU_DeclareMtx( uint32_t, 2, 4, ui );
	CU_DeclareMtx( uint32_t, 3, 2, ui );
	CU_DeclareMtx( uint32_t, 3, 4, ui );
	CU_DeclareMtx( uint32_t, 4, 2, ui );
	CU_DeclareMtx( uint32_t, 4, 3, ui );
	CU_DeclareMtx( float, 2, 3, f );
	CU_DeclareMtx( float, 2, 4, f );
	CU_DeclareMtx( float, 3, 2, f );
	CU_DeclareMtx( float, 3, 4, f );
	CU_DeclareMtx( float, 4, 2, f );
	CU_DeclareMtx( float, 4, 3, f );
	CU_DeclareMtx( double, 2, 3, d );
	CU_DeclareMtx( double, 2, 4, d );
	CU_DeclareMtx( double, 3, 2, d );
	CU_DeclareMtx( double, 3, 4, d );
	CU_DeclareMtx( double, 4, 2, d );
	CU_DeclareMtx( double, 4, 3, d );

	CU_API Point3f operator*( Matrix4x4f const & lhs, Point3f const & rhs );
	CU_API Point3f operator*( Point3f const & lhs, Matrix4x4f const & rhs );
	CU_API Point4f operator*( Matrix4x4f const & lhs, Point4f const & rhs );
	CU_API Point4f operator*( Point4f const & lhs, Matrix4x4f const & rhs );
	//@}

	template< typename SrcT, typename DstCompT >
	struct PointTyperT
	{
		using Type = DstCompT;
	};

	template< typename SrcCompT, uint32_t CountT, typename DstCompT >
	struct PointTyperT< Point< SrcCompT, CountT >, DstCompT >
	{
		using Type = Point< DstCompT, CountT >;
	};

	template< typename SrcT, typename DstCompT >
	struct PointTyperT< LengthT< SrcT >, DstCompT >
	{
		using Type = LengthT< typename PointTyperT< SrcT, DstCompT >::Type >;
	};

	template< typename SrcT, typename DstCompT >
	using PointTypeT = typename PointTyperT< SrcT, DstCompT >::Type;

	template< typename TypeT >
	struct PointComponentGetT
	{
		using Type = TypeT;
	};

	template< typename TypeT >
	struct PointComponentGetT< Point< TypeT, 2u > >
	{
		using Type = TypeT;
	};

	template< typename TypeT >
	struct PointComponentGetT< Point< TypeT, 3u > >
	{
		using Type = TypeT;
	};

	template< typename TypeT >
	struct PointComponentGetT< Point< TypeT, 4u > >
	{
		using Type = TypeT;
	};

	template< typename TypeT >
	struct PointComponentGetT< LengthT< TypeT > >
	{
		using Type = typename PointComponentGetT< TypeT >::Type;
	};

	template< typename TypeT >
	using PointComponentT = typename PointComponentGetT< TypeT >::Type;

	template< typename TypeT >
	struct IsLengthT : std::false_type
	{
	};

	template< typename TypeT >
	struct IsLengthT< LengthT< TypeT > > : std::true_type
	{
	};

	template< typename TypeT >
	static constexpr bool isLengthV = IsLengthT< TypeT >::value;

	template< typename TypeT >
	static constexpr bool isFloatingV = std::is_same_v< float, TypeT >
		|| std::is_same_v< double, TypeT >;
	template< typename TypeT >
	concept FloatingT = ( isFloatingV< TypeT > );

	template< typename TypeT, typename CoordT, uint32_t CountT >
	static constexpr bool isPointV = std::is_same_v< Point< CoordT, CountT >, TypeT >
		|| std::is_same_v< LengthT< Point< CoordT, CountT > >, TypeT >;

	template< typename TypeT, typename CoordT >
	static constexpr bool isPoint2V = isPointV< TypeT, CoordT, 2u >;
	template< typename TypeT, typename CoordT >
	static constexpr bool isPoint3V = isPointV< TypeT, CoordT, 3u >;
	template< typename TypeT, typename CoordT >
	static constexpr bool isPoint4V = isPointV< TypeT, CoordT, 4u >;

	template< typename TypeT >
	static constexpr bool isPoint2f = isPoint2V< TypeT, float >;
	template< typename TypeT >
	static constexpr bool isPoint3f = isPoint3V< TypeT, float >;
	template< typename TypeT >
	static constexpr bool isPoint4f = isPoint4V< TypeT, float >;

	template< typename TypeT >
	static constexpr bool isPoint2d = isPoint2V< TypeT, double >;
	template< typename TypeT >
	static constexpr bool isPoint3d = isPoint3V< TypeT, double >;
	template< typename TypeT >
	static constexpr bool isPoint4d = isPoint4V< TypeT, double >;

	template< typename TypeT >
	concept Vector2T = ( isPoint2f< TypeT > || isPoint2d< TypeT > );
	template< typename TypeT >
	concept Vector3T = ( isPoint3f< TypeT > || isPoint3d< TypeT > );
	template< typename TypeT >
	concept Vector4T = ( isPoint4f< TypeT > || isPoint4d< TypeT > );

	template< typename TypeT >
	concept Vector2fT = ( isPoint2f< TypeT > );
	template< typename TypeT >
	concept Vector3fT = ( isPoint3f< TypeT > );
	template< typename TypeT >
	concept Vector4fT = ( isPoint4f< TypeT > );

	template< typename TypeT >
	concept Vector2dT = ( isPoint2d< TypeT > );
	template< typename TypeT >
	concept Vector3dT = ( isPoint3d< TypeT > );
	template< typename TypeT >
	concept Vector4dT = ( isPoint4d< TypeT > );
}

#endif

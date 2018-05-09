/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CHUNK_DATA_H___
#define ___C3D_CHUNK_DATA_H___

#include "Mesh/VertexGroup.hpp"
#include "Mesh/SubmeshComponent/FaceIndices.hpp"
#include "Mesh/SubmeshComponent/LineIndices.hpp"
#include "Mesh/Skeleton/VertexBoneData.hpp"

#include <Data/Endianness.hpp>
#include <Graphics/Colour.hpp>
#include <Math/Coords.hpp>
#include <Math/Matrix.hpp>
#include <Math/Point.hpp>
#include <Graphics/Position.hpp>
#include <Math/Quaternion.hpp>
#include <Graphics/Size.hpp>
#include <Math/SquareMatrix.hpp>

namespace castor3d
{
	// Version 1.3 and before
	template< typename T >
	struct OldInterleavedVertexT
	{
		castor::Point3< T > pos;
		castor::Point3< T > nml;
		castor::Point3< T > tan;
		castor::Point3< T > bin;
		castor::Point3< T > tex;
	};
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( bool const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( bool & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int8_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int8_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint8_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint8_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int16_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int16_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint16_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint16_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int32_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int32_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint32_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint32_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int64_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int64_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint64_t const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint64_t & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( float const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( float & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( double const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( double & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( long double const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( long double & p_value )
	{
		return reinterpret_cast< uint8_t * >( &p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( T const & p_value )
	{
		return sizeof( p_value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	template< typename T >
	static inline uint8_t const * getBuffer( std::vector< T > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	template< typename T >
	static inline uint8_t * getBuffer( std::vector< T > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( std::vector< T > const & p_value )
	{
		return p_value.size() * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline uint8_t const * getBuffer( std::array< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer (&p_value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon (&p_value pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline uint8_t * getBuffer( std::array< T, Count > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline size_t getDataSize( std::array< T, Count > const & p_value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::Point< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::Point< T, Count > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::Point< T, Count > const & p_value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::Coords< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::Coords< T, Count > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::Coords< T, Count > const & p_value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline uint8_t const * getBuffer( castor::Matrix< T, Columns, Rows > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline uint8_t * getBuffer( castor::Matrix< T, Columns, Rows > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline size_t getDataSize( castor::Matrix< T, Columns, Rows > const & p_value )
	{
		return Columns * Rows * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::SquareMatrix< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::SquareMatrix< T, Count > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::SquareMatrix< T, Count > const & p_value )
	{
		return Count * Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::RgbColour const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::RgbColour & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::RgbColour const & p_value )
	{
		return 3u * sizeof( float );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::RgbaColour const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::RgbaColour & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::RgbaColour const & p_value )
	{
		return 4u * sizeof( float );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::Size const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::Size & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::Size const & p_value )
	{
		return 2 * sizeof( uint32_t );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::Position const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::Position & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::Position const & p_value )
	{
		return 2 * sizeof( int32_t );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T >
	static inline uint8_t const * getBuffer( castor::QuaternionT< T > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	p_value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T >
	static inline uint8_t * getBuffer( castor::QuaternionT< T > & p_value )
	{
		return reinterpret_cast< uint8_t * >( p_value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	p_value	The value.
	 *\return		The size (sizeof( p_value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	p_value	La valeur.
	 *\return		La taille (sizeof( p_value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( castor::QuaternionT< T > const & p_value )
	{
		return 4 * sizeof( T );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( bool & p_value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( int8_t & p_value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( uint8_t & p_value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( int16_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( uint16_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( int32_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( uint32_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( int64_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( uint64_t & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( float & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( double & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( long double & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( std::vector< T > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T, size_t Count >
	static inline void prepareChunkData( std::array< T, Count > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::SquareMatrix< T, Count > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto i = 0u; i < Count * Count; ++i )
			{
				castor::switchEndianness( p_value.ptr()[i] );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::Point< T, Count > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::Coords< T, Count > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( castor::QuaternionT< T > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( castor::ColourComponent & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.value() );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( castor::RgbColour & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( castor::RgbaColour & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			for ( auto & value : p_value )
			{
				castor::switchEndianness( value );
			}
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( OldInterleavedVertexT< T > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.pos[0] );
			castor::switchEndianness( p_value.pos[1] );
			castor::switchEndianness( p_value.pos[2] );
			castor::switchEndianness( p_value.nml[0] );
			castor::switchEndianness( p_value.nml[1] );
			castor::switchEndianness( p_value.nml[2] );
			castor::switchEndianness( p_value.tan[0] );
			castor::switchEndianness( p_value.tan[1] );
			castor::switchEndianness( p_value.tan[2] );
			castor::switchEndianness( p_value.tex[0] );
			castor::switchEndianness( p_value.tex[1] );
			castor::switchEndianness( p_value.tex[2] );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( InterleavedVertexT< T > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.pos[0] );
			castor::switchEndianness( p_value.pos[1] );
			castor::switchEndianness( p_value.pos[2] );
			castor::switchEndianness( p_value.nml[0] );
			castor::switchEndianness( p_value.nml[1] );
			castor::switchEndianness( p_value.nml[2] );
			castor::switchEndianness( p_value.tan[0] );
			castor::switchEndianness( p_value.tan[1] );
			castor::switchEndianness( p_value.tan[2] );
			castor::switchEndianness( p_value.tex[0] );
			castor::switchEndianness( p_value.tex[1] );
			castor::switchEndianness( p_value.tex[2] );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( SubmeshAnimationBufferT< T > & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			prepareChunkData( p_value.m_timeIndex );
			castor::switchEndianness( p_value.m_buffer );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( FaceIndices & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.m_index[0] );
			castor::switchEndianness( p_value.m_index[1] );
			castor::switchEndianness( p_value.m_index[2] );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( LineIndices & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.m_index[0] );
			castor::switchEndianness( p_value.m_index[1] );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( VertexBoneData & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value.m_ids.data[0] );
			castor::switchEndianness( p_value.m_ids.data[1] );
			castor::switchEndianness( p_value.m_ids.data[2] );
			castor::switchEndianness( p_value.m_ids.data[3] );
			castor::switchEndianness( p_value.m_ids.data[4] );
			castor::switchEndianness( p_value.m_ids.data[5] );
			castor::switchEndianness( p_value.m_ids.data[6] );
			castor::switchEndianness( p_value.m_ids.data[7] );
			castor::switchEndianness( p_value.m_weights.data[0] );
			castor::switchEndianness( p_value.m_weights.data[1] );
			castor::switchEndianness( p_value.m_weights.data[2] );
			castor::switchEndianness( p_value.m_weights.data[3] );
			castor::switchEndianness( p_value.m_weights.data[4] );
			castor::switchEndianness( p_value.m_weights.data[5] );
			castor::switchEndianness( p_value.m_weights.data[6] );
			castor::switchEndianness( p_value.m_weights.data[7] );
		}
	}
}

#endif

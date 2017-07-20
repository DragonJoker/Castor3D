/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_CHUNK_DATA_H___
#define ___C3D_CHUNK_DATA_H___


#include "Mesh/VertexGroup.hpp"
#include "Mesh/FaceIndices.hpp"
#include "Mesh/Skeleton/VertexBoneData.hpp"
#include "Animation/KeyFrame.hpp"

#include <Data/Endianness.hpp>
#include <Graphics/Colour.hpp>
#include <Math/Coords.hpp>
#include <Math/Matrix.hpp>
#include <Math/Point.hpp>
#include <Graphics/Position.hpp>
#include <Math/Quaternion.hpp>
#include <Graphics/Size.hpp>
#include <Math/SquareMatrix.hpp>

namespace Castor3D
{
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
	static inline uint8_t const * GetBuffer( bool const & p_value )
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
	static inline uint8_t * GetBuffer( bool & p_value )
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
	static inline uint8_t const * GetBuffer( int8_t const & p_value )
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
	static inline uint8_t * GetBuffer( int8_t & p_value )
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
	static inline uint8_t const * GetBuffer( uint8_t const & p_value )
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
	static inline uint8_t * GetBuffer( uint8_t & p_value )
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
	static inline uint8_t const * GetBuffer( int16_t const & p_value )
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
	static inline uint8_t * GetBuffer( int16_t & p_value )
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
	static inline uint8_t const * GetBuffer( uint16_t const & p_value )
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
	static inline uint8_t * GetBuffer( uint16_t & p_value )
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
	static inline uint8_t const * GetBuffer( int32_t const & p_value )
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
	static inline uint8_t * GetBuffer( int32_t & p_value )
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
	static inline uint8_t const * GetBuffer( uint32_t const & p_value )
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
	static inline uint8_t * GetBuffer( uint32_t & p_value )
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
	static inline uint8_t const * GetBuffer( int64_t const & p_value )
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
	static inline uint8_t * GetBuffer( int64_t & p_value )
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
	static inline uint8_t const * GetBuffer( uint64_t const & p_value )
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
	static inline uint8_t * GetBuffer( uint64_t & p_value )
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
	static inline uint8_t const * GetBuffer( float const & p_value )
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
	static inline uint8_t * GetBuffer( float & p_value )
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
	static inline uint8_t const * GetBuffer( double const & p_value )
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
	static inline uint8_t * GetBuffer( double & p_value )
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
	static inline uint8_t const * GetBuffer( long double const & p_value )
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
	static inline uint8_t * GetBuffer( long double & p_value )
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
	static inline size_t GetDataSize( T const & p_value )
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
	static inline uint8_t const * GetBuffer( std::vector< T > const & p_value )
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
	static inline uint8_t * GetBuffer( std::vector< T > & p_value )
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
	static inline size_t GetDataSize( std::vector< T > const & p_value )
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
	static inline uint8_t const * GetBuffer( std::array< T, Count > const & p_value )
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
	static inline uint8_t * GetBuffer( std::array< T, Count > & p_value )
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
	static inline size_t GetDataSize( std::array< T, Count > const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::Point< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Point< T, Count > & p_value )
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
	static inline size_t GetDataSize( Castor::Point< T, Count > const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::Coords< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Coords< T, Count > & p_value )
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
	static inline size_t GetDataSize( Castor::Coords< T, Count > const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::Matrix< T, Columns, Rows > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Matrix< T, Columns, Rows > & p_value )
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
	static inline size_t GetDataSize( Castor::Matrix< T, Columns, Rows > const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::SquareMatrix< T, Count > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::SquareMatrix< T, Count > & p_value )
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
	static inline size_t GetDataSize( Castor::SquareMatrix< T, Count > const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::Colour const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Colour & p_value )
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
	static inline size_t GetDataSize( Castor::Colour const & p_value )
	{
		return size_t( Castor::Component::eCount ) * sizeof( float );
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
	static inline uint8_t const * GetBuffer( Castor::Size const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Size & p_value )
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
	static inline size_t GetDataSize( Castor::Size const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::Position const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::Position & p_value )
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
	static inline size_t GetDataSize( Castor::Position const & p_value )
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
	static inline uint8_t const * GetBuffer( Castor::QuaternionT< T > const & p_value )
	{
		return reinterpret_cast< uint8_t const * >( p_value.const_ptr() );
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
	static inline uint8_t * GetBuffer( Castor::QuaternionT< T > & p_value )
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
	static inline size_t GetDataSize( Castor::QuaternionT< T > const & p_value )
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
	static inline void PrepareChunkData( bool & p_value )
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
	static inline void PrepareChunkData( int8_t & p_value )
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
	static inline void PrepareChunkData( uint8_t & p_value )
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
	static inline void PrepareChunkData( int16_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( uint16_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( int32_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( uint32_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( int64_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( uint64_t & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( float & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( double & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( long double & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value );
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
	static inline void PrepareChunkData( std::vector< T > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( std::array< T, Count > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( Castor::SquareMatrix< T, Count > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto i = 0u; i < Count * Count; ++i )
			{
				Castor::SwitchEndianness( p_value.ptr()[i] );
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
	static inline void PrepareChunkData( Castor::Point< T, Count > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( Castor::Coords< T, Count > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( Castor::QuaternionT< T > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( Castor::ColourComponent & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value.value() );
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
	static inline void PrepareChunkData( Castor::Colour & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			for ( auto & value : p_value )
			{
				Castor::SwitchEndianness( value );
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
	static inline void PrepareChunkData( InterleavedVertexT< T > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value.m_pos[0] );
			Castor::SwitchEndianness( p_value.m_pos[1] );
			Castor::SwitchEndianness( p_value.m_pos[2] );
			Castor::SwitchEndianness( p_value.m_nml[0] );
			Castor::SwitchEndianness( p_value.m_nml[1] );
			Castor::SwitchEndianness( p_value.m_nml[2] );
			Castor::SwitchEndianness( p_value.m_tan[0] );
			Castor::SwitchEndianness( p_value.m_tan[1] );
			Castor::SwitchEndianness( p_value.m_tan[2] );
			Castor::SwitchEndianness( p_value.m_bin[0] );
			Castor::SwitchEndianness( p_value.m_bin[1] );
			Castor::SwitchEndianness( p_value.m_bin[2] );
			Castor::SwitchEndianness( p_value.m_tex[0] );
			Castor::SwitchEndianness( p_value.m_tex[1] );
			Castor::SwitchEndianness( p_value.m_tex[2] );
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
	static inline void PrepareChunkData( SubmeshAnimationBufferT< T > & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			PrepareChunkData( p_value.m_timeIndex );
			Castor::SwitchEndianness( p_value.m_buffer );
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
	static inline void PrepareChunkData( FaceIndices & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value.m_index[0] );
			Castor::SwitchEndianness( p_value.m_index[1] );
			Castor::SwitchEndianness( p_value.m_index[2] );
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
	static inline void PrepareChunkData( VertexBoneData & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			Castor::SwitchEndianness( p_value.m_ids[0] );
			Castor::SwitchEndianness( p_value.m_ids[1] );
			Castor::SwitchEndianness( p_value.m_ids[2] );
			Castor::SwitchEndianness( p_value.m_ids[3] );
			Castor::SwitchEndianness( p_value.m_ids[4] );
			Castor::SwitchEndianness( p_value.m_ids[5] );
			Castor::SwitchEndianness( p_value.m_ids[6] );
			Castor::SwitchEndianness( p_value.m_ids[7] );
			Castor::SwitchEndianness( p_value.m_weights[0] );
			Castor::SwitchEndianness( p_value.m_weights[1] );
			Castor::SwitchEndianness( p_value.m_weights[2] );
			Castor::SwitchEndianness( p_value.m_weights[3] );
			Castor::SwitchEndianness( p_value.m_weights[4] );
			Castor::SwitchEndianness( p_value.m_weights[5] );
			Castor::SwitchEndianness( p_value.m_weights[6] );
			Castor::SwitchEndianness( p_value.m_weights[7] );
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
	static inline void PrepareChunkData( KeyFrame & p_value )
	{
		if ( !Castor::IsBigEndian() )
		{
			p_value = KeyFrame
			{
				Castor::SwitchEndianness( p_value.GetTimeIndex() ),
				Castor::SwitchEndianness( p_value.GetTransform() )
			};
		}
	}
}

#endif

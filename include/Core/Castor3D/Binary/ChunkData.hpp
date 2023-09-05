/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ChunkData_H___
#define ___C3D_ChunkData_H___

#include "BinaryModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/FaceIndices.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LineIndices.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

#include <CastorUtils/Data/Endianness.hpp>
#include <CastorUtils/Graphics/Colour.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Matrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	template< typename T >
	struct OldInterleavedVertexT
	{
		castor::Point3< T > pos;
		castor::Point3< T > nml;
		castor::Point3< T > tan;
		castor::Point3< T > bin;
		castor::Point3< T > tex;
	};
	struct InterleavedVertexNoMikk
	{
		castor::Point3f pos;
		castor::Point3f nml;
		castor::Point3f tan;
		castor::Point3f tex;
	};
	using InterleavedVertexNoMikkArray = std::vector< InterleavedVertexNoMikk >;
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( bool const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( bool & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int8_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int8_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint8_t const & value )
	{
		return &value;
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint8_t & value )
	{
		return &value;
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int16_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int16_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint16_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint16_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int32_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int32_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint32_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint32_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( int64_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( int64_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( uint64_t const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( uint64_t & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( float const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( float & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( double const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( double & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t const * getBuffer( long double const & value )
	{
		return reinterpret_cast< uint8_t const * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	static inline uint8_t * getBuffer( long double & value )
	{
		return reinterpret_cast< uint8_t * >( &value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( T const & value )
	{
		return sizeof( value );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	template< typename T >
	static inline uint8_t const * getBuffer( std::vector< T > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	template< typename T >
	static inline uint8_t * getBuffer( std::vector< T > & value )
	{
		return reinterpret_cast< uint8_t * >( value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( std::vector< T > const & value )
	{
		return value.size() * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline uint8_t const * getBuffer( std::array< T, Count > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer (&value for basic types).
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon (&value pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline uint8_t * getBuffer( std::array< T, Count > & value )
	{
		return reinterpret_cast< uint8_t * >( value.data() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T, size_t Count >
	static inline size_t getDataSize( std::array< T, Count > const & value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::Point< T, Count > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::Point< T, Count > & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::Point< T, Count > const & value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::Coords< T, Count > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::Coords< T, Count > & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::Coords< T, Count > const & value )
	{
		return Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline uint8_t const * getBuffer( castor::Matrix< T, Columns, Rows > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline uint8_t * getBuffer( castor::Matrix< T, Columns, Rows > & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T, uint32_t Columns, uint32_t Rows >
	static inline size_t getDataSize( castor::Matrix< T, Columns, Rows > const & value )
	{
		return Columns * Rows * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t const * getBuffer( castor::SquareMatrix< T, Count > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T, uint32_t Count >
	static inline uint8_t * getBuffer( castor::SquareMatrix< T, Count > & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T, uint32_t Count >
	static inline size_t getDataSize( castor::SquareMatrix< T, Count > const & value )
	{
		return Count * Count * sizeof( T );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::RgbColour const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::RgbColour & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::RgbColour const & value )
	{
		return 3u * sizeof( float );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::RgbaColour const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::RgbaColour & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::RgbaColour const & value )
	{
		return 4u * sizeof( float );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::Size const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::Size & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::Size const & value )
	{
		return 2 * sizeof( uint32_t );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t const * getBuffer( castor::Position const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	static inline uint8_t * getBuffer( castor::Position & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	static inline size_t getDataSize( castor::Position const & value )
	{
		return 2 * sizeof( int32_t );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T >
	static inline uint8_t const * getBuffer( castor::QuaternionT< T > const & value )
	{
		return reinterpret_cast< uint8_t const * >( value.constPtr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the beginning of the buffer for given value.
	 *\param[in]	value	The value.
	 *\return		The buffer.
	 *\~french
	 *\brief		Récupère le début du tampon représentant la valeur.
	 *\param[in]	value	La valeur.
	 *\return		Le tampon.
	 */
	template< typename T >
	static inline uint8_t * getBuffer( castor::QuaternionT< T > & value )
	{
		return reinterpret_cast< uint8_t * >( value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Retrieves the value binary size.
	 *\param[in]	value	The value.
	 *\return		The size (sizeof( value ) for basic types).
	 *\~french
	 *\brief		Récupère la taille en octets de la valeur.
	 *\param[in]	value	La valeur.
	 *\return		La taille (sizeof( value ) pour les types basiques).
	 */
	template< typename T >
	static inline size_t getDataSize( castor::QuaternionT< T > const & value )
	{
		return 4 * sizeof( T );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( bool & value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( int8_t & value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( uint8_t & value )
	{
		// noop
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( int16_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( uint16_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( int32_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( uint32_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( int64_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( uint64_t & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( float & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( double & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( long double & value )
	{
		castor::switchEndianness( value );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	values	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	values	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( std::vector< T > & values )
	{
		for ( auto & value : values )
		{
			castor::switchEndianness( value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	values	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	values	La valeur.
	 */
	template< typename T, size_t Count >
	static inline void prepareChunkData( std::array< T, Count > & values )
	{
		for ( auto & value : values )
		{
			castor::switchEndianness( value );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::SquareMatrix< T, Count > & value )
	{
		for ( auto i = 0u; i < Count * Count; ++i )
		{
			castor::switchEndianness( value.ptr()[i] );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::Point< T, Count > & value )
	{
		for ( auto & component : value )
		{
			castor::switchEndianness( component );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename T, uint32_t Count >
	static inline void prepareChunkData( castor::Coords< T, Count > & value )
	{
		for ( auto & component : value )
		{
			castor::switchEndianness( component );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( castor::QuaternionT< T > & value )
	{
		for ( auto & component : value )
		{
			castor::switchEndianness( component );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( castor::ColourComponent & value )
	{
		castor::switchEndianness( value.value() );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( castor::RgbColour & value )
	{
		for ( auto & component : value )
		{
			castor::switchEndianness( component );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( castor::RgbaColour & value )
	{
		for ( auto & component : value )
		{
			castor::switchEndianness( component );
		}
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename T >
	static inline void prepareChunkData( OldInterleavedVertexT< T > & value )
	{
		castor::switchEndianness( value.pos[0] );
		castor::switchEndianness( value.pos[1] );
		castor::switchEndianness( value.pos[2] );
		castor::switchEndianness( value.nml[0] );
		castor::switchEndianness( value.nml[1] );
		castor::switchEndianness( value.nml[2] );
		castor::switchEndianness( value.tan[0] );
		castor::switchEndianness( value.tan[1] );
		castor::switchEndianness( value.tan[2] );
		castor::switchEndianness( value.tex[0] );
		castor::switchEndianness( value.tex[1] );
		castor::switchEndianness( value.tex[2] );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( InterleavedVertexNoMikk & value )
	{
		castor::switchEndianness( value.pos[0] );
		castor::switchEndianness( value.pos[1] );
		castor::switchEndianness( value.pos[2] );
		castor::switchEndianness( value.nml[0] );
		castor::switchEndianness( value.nml[1] );
		castor::switchEndianness( value.nml[2] );
		castor::switchEndianness( value.tan[0] );
		castor::switchEndianness( value.tan[1] );
		castor::switchEndianness( value.tan[2] );
		castor::switchEndianness( value.tex[0] );
		castor::switchEndianness( value.tex[1] );
		castor::switchEndianness( value.tex[2] );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( InterleavedVertex & value )
	{
		castor::switchEndianness( value.pos[0] );
		castor::switchEndianness( value.pos[1] );
		castor::switchEndianness( value.pos[2] );
		castor::switchEndianness( value.nml[0] );
		castor::switchEndianness( value.nml[1] );
		castor::switchEndianness( value.nml[2] );
		castor::switchEndianness( value.tan[0] );
		castor::switchEndianness( value.tan[1] );
		castor::switchEndianness( value.tan[2] );
		castor::switchEndianness( value.tan[3] );
		castor::switchEndianness( value.tex[0] );
		castor::switchEndianness( value.tex[1] );
		castor::switchEndianness( value.tex[2] );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( SubmeshAnimationBuffer & value )
	{
		castor::switchEndianness( value.positions );
		castor::switchEndianness( value.normals );
		castor::switchEndianness( value.tangents );
		castor::switchEndianness( value.texcoords0 );
		castor::switchEndianness( value.texcoords1 );
		castor::switchEndianness( value.texcoords2 );
		castor::switchEndianness( value.texcoords3 );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( FaceIndices & value )
	{
		castor::switchEndianness( value.m_index[0] );
		castor::switchEndianness( value.m_index[1] );
		castor::switchEndianness( value.m_index[2] );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( LineIndices & value )
	{
		castor::switchEndianness( value.m_index[0] );
		castor::switchEndianness( value.m_index[1] );
	}
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( VertexBoneData & value )
	{
		castor::switchEndianness( value.m_ids.data[0] );
		castor::switchEndianness( value.m_ids.data[1] );
		castor::switchEndianness( value.m_ids.data[2] );
		castor::switchEndianness( value.m_ids.data[3] );
		castor::switchEndianness( value.m_ids.data[4] );
		castor::switchEndianness( value.m_ids.data[5] );
		castor::switchEndianness( value.m_ids.data[6] );
		castor::switchEndianness( value.m_ids.data[7] );
		castor::switchEndianness( value.m_weights.data[0] );
		castor::switchEndianness( value.m_weights.data[1] );
		castor::switchEndianness( value.m_weights.data[2] );
		castor::switchEndianness( value.m_weights.data[3] );
		castor::switchEndianness( value.m_weights.data[4] );
		castor::switchEndianness( value.m_weights.data[5] );
		castor::switchEndianness( value.m_weights.data[6] );
		castor::switchEndianness( value.m_weights.data[7] );
	}
	/**
	 *\~english
	 *\brief			Prepares chunk data for read/write.
	 *\param[in,out]	chunk	The chunk, for endianness check.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Prépare les données du chunk à l'écriture/lecture.
	 *\param[in,out]	chunk	Le chunk pour le boutisme.
	 *\param[in,out]	value	La valeur.
	 */
	template< typename DataT >
	static inline void prepareChunkDataT( BinaryChunk const * chunk
		, DataT & value )
	{
		if ( chunk )
		{
			// Read from chunk.
			if ( isLittleEndian( *chunk ) == castor::isLittleEndian() )
			{
				// chunk and system endiannesses are identical, nothing to do
				return;
			}

			prepareChunkData( value );
		}
		else
		{
			// Write to chunk.
			if constexpr ( !castor::isLittleEndian() )
			{
				prepareChunkData( value );
			}
		}
	}

	template< typename T >
	T & chunkEndianToSystemEndian( BinaryChunk const & chunk
		, T & value )
	{
		if constexpr ( !castor::isLittleEndian() )
		{
			if ( isLittleEndian( chunk ) )
			{
				return castor::switchEndianness( value );
			}

			return value;
		}
		else
		{
			if ( !isLittleEndian( chunk ) )
			{
				return castor::switchEndianness( value );
			}

			return value;
		}
	}
}

#endif

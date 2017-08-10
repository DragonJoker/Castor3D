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
#ifndef ___C3D_VERTEX_H___
#define ___C3D_VERTEX_H___

#include "Castor3DPrerequisites.hpp"

#include "Mesh/Buffer/BufferElementGroup.hpp"

#include <Data/TextLoader.hpp>
#include <Math/Coords.hpp>
#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		27/06/2013
	\~english
	\brief		Representation of a vertex
	\remark		Wraps BufferElementGroup to easily retrieve vertex's position, normal, tangent and texture coordinates, an index
	\~fench
	\brief		Représentation d'un sommet
	\remark		Encadre un BufferElementGroup pour récupérer facilement la position d'un sommet, sa normale, sa tangente, ses coordonnées de texture, un indice
	*/
	class Vertex
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_group		The wrapped group
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_group		Le groupe encadré
		 */
		C3D_API explicit Vertex( BufferElementGroup & p_group );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_source		The source object
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_source		L'objet source
		 */
		C3D_API Vertex( Vertex const & p_source );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_source		The source object
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_source		L'objet source
		 *\return		Une référence sur cet objet
		 */
		C3D_API Vertex & operator=( Vertex const & p_source );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Vertex();
		/**
		 *\~english
		 *\brief		sets the vertex position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position du sommet
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void setPosition( castor::Coords3r const & val )
		{
			setPosition( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position du sommet
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void setPosition( castor::Point3r const & val )
		{
			setPosition( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex position
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la position du sommet
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		inline void setPosition( real x, real y, real z )
		{
			setPosition( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the vertex position
		 *\param[in]	p_coords	The new value
		 *\~french
		 *\brief		Définit la position du sommet
		 *\param[in]	p_coords	La nouvelle valeur
		 */
		inline void setPosition( real const * p_coords )
		{
			setPosition( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la normale du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setNormal( castor::Coords3r const & val )
		{
			setNormal( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la normale du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setNormal( castor::Point3r const & val )
		{
			setNormal( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la normale du sommet
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void setNormal( real x, real y, real z )
		{
			setNormal( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la normale du sommet
		 *\param[in]	p_coords	Le tampon de données
		 */
		inline void setNormal( real const * p_coords )
		{
			setNormal( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la tangente du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setTangent( castor::Coords3r const & val )
		{
			setTangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la tangente du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setTangent( castor::Point3r const & val )
		{
			setTangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la tangente du sommet
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void setTangent( real x, real y, real z )
		{
			setTangent( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la tangente du sommet
		 *\param[in]	p_coords	Le tampon de données
		 */
		inline void setTangent( real const * p_coords )
		{
			setTangent( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la bitangente du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setBitangent( castor::Coords3r const & val )
		{
			setBitangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la bitangente du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setBitangent( castor::Point3r const & val )
		{
			setBitangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la bitangente du sommet
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void setBitangent( real x, real y, real z )
		{
			setBitangent( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la bitangente du sommet
		 *\param[in]	p_coords	Le tampon de données
		 */
		inline void setBitangent( real const * p_coords )
		{
			setBitangent( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setTexCoord( castor::Coords3r const & val )
		{
			setTexCoord( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet
		 *\param[in]	val	La valeur
		 */
		inline void setTexCoord( castor::Point3r const & val )
		{
			setTexCoord( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void setTexCoord( real x, real y, real z = 0 )
		{
			setTexCoord( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet
		 *\param[in]	p_coords	Le tampon de données
		 */
		inline void setTexCoord( real const * p_coords )
		{
			setTexCoord( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du sommet
		 *\return		La valeur
		 */
		inline castor::Coords3r & getPosition( castor::Coords3r & p_ptPos )
		{
			return getPosition( m_group.ptr(), p_ptPos );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du sommet
		 *\return		La valeur
		 */
		inline castor::Point3r & getPosition( castor::Point3r & p_ptPos )const
		{
			return getPosition( m_group.constPtr(), p_ptPos );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du sommet
		 *\return		La valeur
		 */
		inline castor::Coords3r & getNormal( castor::Coords3r & p_ptNml )
		{
			return getNormal( m_group.ptr(), p_ptNml );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du sommet
		 *\return		La valeur
		 */
		inline castor::Point3r & getNormal( castor::Point3r & p_ptNml )const
		{
			return getNormal( m_group.constPtr(), p_ptNml );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangent du sommet
		 *\return		La valeur
		 */
		inline castor::Coords3r & getTangent( castor::Coords3r & p_ptTan )
		{
			return getTangent( m_group.ptr(), p_ptTan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangent du sommet
		 *\return		La valeur
		 */
		inline castor::Point3r & getTangent( castor::Point3r & p_ptTan )const
		{
			return getTangent( m_group.constPtr(), p_ptTan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du sommet
		 *\return		La valeur
		 */
		inline castor::Coords3r & getBitangent( castor::Coords3r & p_ptBitan )
		{
			return getBitangent( m_group.ptr(), p_ptBitan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du sommet
		 *\return		La valeur
		 */
		inline castor::Point3r & getBitangent( castor::Point3r & p_ptBitan )const
		{
			return getBitangent( m_group.constPtr(), p_ptBitan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du sommet
		 *\return		La valeur
		 */
		inline castor::Coords3r & getTexCoord( castor::Coords3r & p_ptTex )
		{
			return getTexCoord( m_group.ptr(), p_ptTex );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du sommet
		 *\return		La valeur
		 */
		inline castor::Point3r & getTexCoord( castor::Point3r & p_ptTex )const
		{
			return getTexCoord( m_group.constPtr(), p_ptTex );
		}
		/**
		 *\~english
		 *\brief		Retrieves position elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments de la position
		 *\return		La valeur
		 */
		static inline const uint32_t getCountPos()
		{
			return sm_uiCountPos;
		}
		/**
		 *\~english
		 *\brief		Retrieves normal elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments de la normale
		 *\return		La valeur
		 */
		static inline const uint32_t getCountNml()
		{
			return sm_uiCountNml;
		}
		/**
		 *\~english
		 *\brief		Retrieves tangent elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments de la tangent
		 *\return		La valeur
		 */
		static inline const uint32_t getCountTan()
		{
			return sm_uiCountTan;
		}
		/**
		 *\~english
		 *\brief		Retrieves bitangent elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments de la bitangente
		 *\return		La valeur
		 */
		static inline const uint32_t getCountBin()
		{
			return sm_uiCountBin;
		}
		/**
		 *\~english
		 *\brief		Retrieves texture coordinates elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments des coordonnées de texture
		 *\return		La valeur
		 */
		static inline const uint32_t getCountTex()
		{
			return sm_uiCountTex;
		}
		/**
		 *\~english
		 *\brief		Retrieves position's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset de la position dans le tampon
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetPos()
		{
			return sm_uiOffsetPos;
		}
		/**
		 *\~english
		 *\brief		Retrieves normal's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset de la normale dans le tampon
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetNml()
		{
			return sm_uiOffsetNml;
		}
		/**
		 *\~english
		 *\brief		Retrieves tangent's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset de la tangente dans le tampon
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetTan()
		{
			return sm_uiOffsetTan;
		}
		/**
		 *\~english
		 *\brief		Retrieves bitangent's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset de la bitangente dans le tampon
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetBin()
		{
			return sm_uiOffsetBin;
		}
		/**
		 *\~english
		 *\brief		Retrieves texture coordinates' buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset des coordonnées de texture dans le tampon
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetTex()
		{
			return sm_uiOffsetTex;
		}
		/**
		 *\~english
		 *\brief		Retrieves position binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets de la position
		 *\return		La valeur
		 */
		static inline const uint32_t getSizePos()
		{
			return sm_uiSizePos;
		}
		/**
		 *\~english
		 *\brief		Retrieves normal binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets de la normale
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeNml()
		{
			return sm_uiSizeNml;
		}
		/**
		 *\~english
		 *\brief		Retrieves tangent binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets de la tangent
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeTan()
		{
			return sm_uiSizeTan;
		}
		/**
		 *\~english
		 *\brief		Retrieves bitangent binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets de la bitangente
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeBin()
		{
			return sm_uiSizeBin;
		}
		/**
		 *\~english
		 *\brief		Retrieves texture coordinates binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets des coordonnées de texture
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeTex()
		{
			return sm_uiSizeTex;
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Coords3r getPosition( void * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Point3r getPosition( void const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getPosition( void * p_buffer, castor::Coords3r & p_coord )
		{
			return p_coord = getPosition( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Point3r & getPosition( void const * p_buffer, castor::Point3r & p_coord )
		{
			return p_coord = getPosition( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\~french
		 *\brief		Récupère la position d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 */
		static void getPosition( void const * p_buffer, std::array< real, 3 > & p_coord )
		{
			auto position = getPosition( p_buffer );
			p_coord[0] = position[0];
			p_coord[1] = position[1];
			p_coord[2] = position[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Coords3r getNormal( void * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Point3r getNormal( void const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Coords3r & getNormal( void * p_buffer, castor::Coords3r & p_coord )
		{
			return p_coord = getNormal( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Point3r & getNormal( void const * p_buffer, castor::Point3r & p_coord )
		{
			return p_coord = getNormal( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static void getNormal( void const * p_buffer, std::array< real, 3 > & p_coord )
		{
			auto normal = getNormal( p_buffer );
			p_coord[0] = normal[0];
			p_coord[1] = normal[1];
			p_coord[2] = normal[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Coords3r getTangent( void * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Point3r getTangent( void const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Coords3r & getTangent( void * p_buffer, castor::Coords3r & p_coord )
		{
			return p_coord = getTangent( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static void getTangent( void const * p_buffer, std::array< real, 3 > & p_coord )
		{
			auto tangent = getTangent( p_buffer );
			p_coord[0] = tangent[0];
			p_coord[1] = tangent[1];
			p_coord[2] = tangent[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Point3r & getTangent( void const * p_buffer, castor::Point3r & p_coord )
		{
			return p_coord = getTangent( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Coords3r getBitangent( void * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Point3r getBitangent( void const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Coords3r & getBitangent( void * p_buffer, castor::Coords3r & p_coord )
		{
			return p_coord = getBitangent( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Point3r & getBitangent( void const * p_buffer, castor::Point3r & p_coord )
		{
			return p_coord = getBitangent( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static void getBitangent( void const * p_buffer, std::array< real, 3 > & p_coord )
		{
			auto bitangent = getBitangent( p_buffer );
			p_coord[0] = bitangent[0];
			p_coord[1] = bitangent[1];
			p_coord[2] = bitangent[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Coords3r getTexCoord( void * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\return		La valeur
		 */
		C3D_API static castor::Point3r getTexCoord( void const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Coords3r & getTexCoord( void * p_buffer, castor::Coords3r & p_coord )
		{
			return p_coord = getTexCoord( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static castor::Point3r & getTexCoord( void const * p_buffer, castor::Point3r & p_coord )
		{
			return p_coord = getTexCoord( p_buffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un sommet à partir d'un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static void getTexCoord( void const * p_buffer, std::array< real, 3 > & p_coord )
		{
			auto texcoord = getTexCoord( p_buffer );
			p_coord[0] = texcoord[0];
			p_coord[1] = texcoord[1];
			p_coord[2] = texcoord[2];
		}
		/**
		 *\~english
		 *\brief		sets the vertex position in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setPosition( void * p_buffer, castor::Coords3r const & p_coord )
		{
			setPosition( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex position in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setPosition( void * p_buffer, castor::Point3r const & p_coord )
		{
			setPosition( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex position in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void setPosition( void * p_buffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		sets the vertex position in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la position du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void setPosition( void * p_buffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		sets the vertex normal in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setNormal( void * p_buffer, castor::Coords3r const & p_coord )
		{
			setNormal( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setNormal( void * p_buffer, castor::Point3r const & p_coord )
		{
			setNormal( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex normal in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void setNormal( void * p_buffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		sets the vertex normal in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la normale du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void setNormal( void * p_buffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		sets the vertex tangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTangent( void * p_buffer, castor::Coords3r const & p_coord )
		{
			setTangent( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTangent( void * p_buffer, castor::Point3r const & p_coord )
		{
			setTangent( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex tangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void setTangent( void * p_buffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		sets the vertex tangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la tangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void setTangent( void * p_buffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		sets the vertex bitangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setBitangent( void * p_buffer, castor::Coords3r const & p_coord )
		{
			setBitangent( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setBitangent( void * p_buffer, castor::Point3r const & p_coord )
		{
			setBitangent( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex bitangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void setBitangent( void * p_buffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		sets the vertex bitangent in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la bitangente du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void setBitangent( void * p_buffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTexCoord( void * p_buffer, castor::Coords3r const & p_coord )
		{
			setTexCoord( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTexCoord( void * p_buffer, castor::Point3r const & p_coord )
		{
			setTexCoord( p_buffer, p_coord.constPtr() );
		}
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void setTexCoord( void * p_buffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		sets the vertex texture coordinates in a buffer
		 *\param[in]	p_buffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du sommet dans un tampon
		 *\param[in]	p_buffer	Le tampon
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void setTexCoord( void * p_buffer, real x, real y, real z = 0 );
		/**
		 *\~english
		 *\brief		Retrieves the group position
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_coord		Receives the coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_coord		Reçoit les coordonnées
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getPosition( BufferElementGroupSPtr p_pElement, castor::Coords3r & p_coord )
		{
			return getPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group position
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_point		Receives the coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées
		 *\return		La valeur
		 */
		static inline castor::Point3r & getPosition( BufferElementGroupSPtr p_pElement, castor::Point3r & p_point )
		{
			return getPosition( p_pElement->constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group normal
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_coord		Receives the normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_coord		Reçoit la normale
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getNormal( BufferElementGroupSPtr p_pElement, castor::Coords3r & p_coord )
		{
			return getNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group normal
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_point		Receives the normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_point		Reçoit la normale
		 *\return		La valeur
		 */
		static inline castor::Point3r & getNormal( BufferElementGroupSPtr p_pElement, castor::Point3r & p_point )
		{
			return getNormal( p_pElement->constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_coord		Receives the tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_coord		Reçoit la tangente
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getTangent( BufferElementGroupSPtr p_pElement, castor::Coords3r & p_coord )
		{
			return getTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_point		Receives the tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_point		Reçoit la tangente
		 *\return		La valeur
		 */
		static inline castor::Point3r & getTangent( BufferElementGroupSPtr p_pElement, castor::Point3r & p_point )
		{
			return getTangent( p_pElement->constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_coord		Receives the bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_coord		Reçoit la bitangente
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getBitangent( BufferElementGroupSPtr p_pElement, castor::Coords3r & p_coord )
		{
			return getBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_point		Receives the bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_point		Reçoit la bitangente
		 *\return		La valeur
		 */
		static inline castor::Point3r & getBitangent( BufferElementGroupSPtr p_pElement, castor::Point3r & p_point )
		{
			return getBitangent( p_pElement->constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_coord		Receives the texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_coord		Reçoit les coordonnées de texture
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getTexCoord( BufferElementGroupSPtr p_pElement, castor::Coords3r & p_coord )
		{
			return getTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[out]	p_point		Receives the texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées de texture
		 *\return		La valeur
		 */
		static inline castor::Point3r & getTexCoord( BufferElementGroupSPtr p_pElement, castor::Point3r & p_point )
		{
			return getTexCoord( p_pElement->constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroupSPtr p_pElement, castor::Coords3r const & p_coord )
		{
			setPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroupSPtr p_pElement, castor::Point3r const & p_coord )
		{
			setPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			setPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			setPosition( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroupSPtr p_pElement, castor::Coords3r const & p_coord )
		{
			setNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroupSPtr p_pElement, castor::Point3r const & p_coord )
		{
			setNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			setNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			setNormal( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroupSPtr p_pElement, castor::Coords3r const & p_coord )
		{
			setTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroupSPtr p_pElement, castor::Point3r const & p_coord )
		{
			setTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			setTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			setTangent( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroupSPtr p_pElement, castor::Coords3r const & p_coord )
		{
			setBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroupSPtr p_pElement, castor::Point3r const & p_coord )
		{
			setBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			setBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			setBitangent( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroupSPtr p_pElement, castor::Coords3r const & p_coord )
		{
			setTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroupSPtr p_pElement, castor::Point3r const & p_coord )
		{
			setTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			setTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroupSPtr p_pElement, real x, real y, real z = 0 )
		{
			setTexCoord( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group position
		 *\param[in]	p_element	The group
		 *\param[out]	p_coord		Receives the coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_coord		Reçoit les coordonnées
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getPosition( BufferElementGroup & p_element, castor::Coords3r & p_coord )
		{
			return getPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group position
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées
		 *\return		La valeur
		 */
		static inline castor::Point3r & getPosition( BufferElementGroup const & p_element, castor::Point3r & p_point )
		{
			return getPosition( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group position
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées
		 *\return		La valeur
		 */
		static inline void getPosition( BufferElementGroup const & p_element, std::array< real, 3 > & p_point )
		{
			getPosition( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group normal
		 *\param[in]	p_element	The group
		 *\param[out]	p_coord		Receives the normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_coord		Reçoit la normale
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getNormal( BufferElementGroup & p_element, castor::Coords3r & p_coord )
		{
			return getNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group normal
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la normale
		 *\return		La valeur
		 */
		static inline castor::Point3r & getNormal( BufferElementGroup const & p_element, castor::Point3r & p_point )
		{
			return getNormal( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group normal
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la normale
		 *\return		La valeur
		 */
		static inline void getNormal( BufferElementGroup const & p_element, std::array< real, 3 > & p_point )
		{
			getNormal( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group tangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_coord		Receives the tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_coord		Reçoit la tangente
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getTangent( BufferElementGroup & p_element, castor::Coords3r & p_coord )
		{
			return getTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group tangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la tangente
		 *\return		La valeur
		 */
		static inline castor::Point3r & getTangent( BufferElementGroup const & p_element, castor::Point3r & p_point )
		{
			return getTangent( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group tangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la tangente
		 *\return		La valeur
		 */
		static inline void getTangent( BufferElementGroup const & p_element, std::array< real, 3 > & p_point )
		{
			getTangent( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bitangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_coord		Receives the bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_coord		Reçoit la bitangente
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getBitangent( BufferElementGroup & p_element, castor::Coords3r & p_coord )
		{
			return getBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bitangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la bitangente
		 *\return		La valeur
		 */
		static inline castor::Point3r & getBitangent( BufferElementGroup const & p_element, castor::Point3r & p_point )
		{
			return getBitangent( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bitangent
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit la bitangente
		 *\return		La valeur
		 */
		static inline void getBitangent( BufferElementGroup const & p_element, std::array< real, 3 > & p_point )
		{
			getBitangent( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group texture coordinates
		 *\param[in]	p_element	The group
		 *\param[out]	p_coord		Receives the texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_coord		Reçoit les coordonnées de texture
		 *\return		La valeur
		 */
		static inline castor::Coords3r & getTexCoord( BufferElementGroup & p_element, castor::Coords3r & p_coord )
		{
			return getTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group texture coordinates
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées de texture
		 *\return		La valeur
		 */
		static inline castor::Point3r & getTexCoord( BufferElementGroup const & p_element, castor::Point3r & p_point )
		{
			return getTexCoord( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Retrieves the group texture coordinates
		 *\param[in]	p_element	The group
		 *\param[out]	p_point		Receives the texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du groupe
		 *\param[in]	p_element	Le groupe
		 *\param[out]	p_point		Reçoit les coordonnées de texture
		 *\return		La valeur
		 */
		static inline void getTexCoord( BufferElementGroup const & p_element, std::array< real, 3 > & p_point )
		{
			getTexCoord( p_element.constPtr(), p_point );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroup & p_element, castor::Coords3r const & p_coord )
		{
			setPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroup & p_element, castor::Point3r const & p_coord )
		{
			setPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroup & p_element, real const * p_coord )
		{
			setPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void setPosition( BufferElementGroup & p_element, real x, real y, real z )
		{
			setPosition( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroup & p_element, castor::Coords3r const & p_coord )
		{
			setNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroup & p_element, castor::Point3r const & p_coord )
		{
			setNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroup & p_element, real const * p_coord )
		{
			setNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void setNormal( BufferElementGroup & p_element, real x, real y, real z )
		{
			setNormal( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroup & p_element, castor::Coords3r const & p_coord )
		{
			setTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroup & p_element, castor::Point3r const & p_coord )
		{
			setTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroup & p_element, real const * p_coord )
		{
			setTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void setTangent( BufferElementGroup & p_element, real x, real y, real z )
		{
			setTangent( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroup & p_element, castor::Coords3r const & p_coord )
		{
			setBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroup & p_element, castor::Point3r const & p_coord )
		{
			setBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroup & p_element, real const * p_coord )
		{
			setBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void setBitangent( BufferElementGroup & p_element, real x, real y, real z )
		{
			setBitangent( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroup & p_element, castor::Coords3r const & p_coord )
		{
			setTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroup & p_element, castor::Point3r const & p_coord )
		{
			setTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroup & p_element, real const * p_coord )
		{
			setTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void setTexCoord( BufferElementGroup & p_element, real x, real y, real z = 0 )
		{
			setTexCoord( p_element.ptr(), x, y, z );
		}

	private:
		//!\~english	Vertex position elements count.
		//!\~french		Compte des élément de la position du sommet.
		static const uint32_t sm_uiCountPos = 3;
		//!\~english	Vertex normal elements count.
		//!\~french		Compte des élément de la normale du sommet.
		static const uint32_t sm_uiCountNml = 3;
		//!\~english	Vertex tangent elements count.
		//!\~french		Compte des élément de la tangente du sommet.
		static const uint32_t sm_uiCountTan = 3;
		//!\~english	Vertex bitangent elements count.
		//!\~french		Compte des élément de la bitangente du sommet.
		static const uint32_t sm_uiCountBin = 3;
		//!\~english	Vertex texture coordinates elements count.
		//!\~french		Compte des élément des coordonnées de texture du sommet.
		static const uint32_t sm_uiCountTex = 3;
		//!\~english	Vertex position binary size.
		//!\~french		Taille en octets de la position du sommet.
		static const uint32_t sm_uiSizePos = uint32_t( sm_uiCountPos * sizeof( real ) );
		//!\~english	Vertex normal binary size.
		//!\~french		Taille en octets de la normale du sommet.
		static const uint32_t sm_uiSizeNml = uint32_t( sm_uiCountNml * sizeof( real ) );
		//!\~english	Vertex tangent binary size.
		//!\~french		Taille en octets de la tangente du sommet.
		static const uint32_t sm_uiSizeTan = uint32_t( sm_uiCountTan * sizeof( real ) );
		//!\~english	Vertex bitangent binary size.
		//!\~french		Taille en octets de la bitangente du sommet.
		static const uint32_t sm_uiSizeBin = uint32_t( sm_uiCountBin * sizeof( real ) );
		//!\~english	Vertex texture coordinates binary size.
		//!\~french		Taille en octets des coordonnées de texture du sommet.
		static const uint32_t sm_uiSizeTex = uint32_t( sm_uiCountTex * sizeof( real ) );
		//!\~english	Vertex position's buffer offset.
		//!\~french		Offset dans le tampon de la position du sommet.
		static const uint32_t sm_uiOffsetPos = 0;
		//!\~english	Vertex normal's buffer offset.
		//!\~french		Offset dans le tampon de la normale du sommet.
		static const uint32_t sm_uiOffsetNml = sm_uiOffsetPos + sm_uiSizePos;
		//!\~english	Vertex tangent's buffer offset.
		//!\~french		Offset dans le tampon de la tangente du sommet.
		static const uint32_t sm_uiOffsetTan = sm_uiOffsetNml + sm_uiSizeNml;
		//!\~english	Vertex bitangent's buffer offset.
		//!\~french		Offset dans le tampon de la bitangente du sommet.
		static const uint32_t sm_uiOffsetBin = sm_uiOffsetTan + sm_uiSizeTan;
		//!\~english	Vertex texture coordinates' buffer offset.
		//!\~french		Offset dans le tampon des coordonnées de texture du sommet.
		static const uint32_t sm_uiOffsetTex = sm_uiOffsetBin + sm_uiSizeBin;

	private:
		//!\~english	The wrapped group.
		//!\~french		Le groupe encadré.
		BufferElementGroup & m_group;
	};
}

#endif

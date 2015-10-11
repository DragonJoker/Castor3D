/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_VERTEX_H___
#define ___C3D_VERTEX_H___

#include "Castor3DPrerequisites.hpp"
#include "BufferElementGroup.hpp"

#include <TextLoader.hpp>
#include <Coords.hpp>
#include <Point.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		27/06/2013
	\~english
	\brief		Representation of a vertex
	\remark		Wraps BufferElementGroup to easily retrieve vertex's position, normal, tangent and texture coordinates, an index
	\~fench
	\brief		Représentation d'un vertex
	\remark		Encadre un BufferElementGroup pour récupérer facilement la position d'un vertex, sa normale, sa tangente, ses coordonnées de texture, un indice
	*/
	class Vertex
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Vertex loader
		\~french
		\brief Loader de Vertex
		*/
		class TextLoader
			: public Castor::Loader< Vertex, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Loads a Vertex object from a text file
			 *\param[out]	p_object	The Vertex object to read
			 *\param[in]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet Vertex à partir d'un fichier texte
			 *\param[out]	p_object	L'objet Vertex
			 *\param[in]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			C3D_API virtual bool operator()( Castor3D::Vertex & p_object, Castor::TextFile & p_file );
			/**
			 *\~english
			 *\brief		Writes a Vertex object into a text file
			 *\param[in]	p_object	The Vertex object to write
			 *\param[out]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet Vertex dans un fichier texte
			 *\param[in]	p_object	L'objet Vertex
			 *\param[out]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			C3D_API virtual bool operator()( Castor3D::Vertex const & p_object, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_group		The wrapped group
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_group		Le groupe encadré
		 */
		C3D_API Vertex( BufferElementGroup & p_group );
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
		 *\brief		Sets the vertex position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position du vertice
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPosition( Castor::Coords3r const & val )
		{
			SetPosition( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position du vertice
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPosition( Castor::Point3r const & val )
		{
			SetPosition( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la position du vertice
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		inline void SetPosition( real x, real y, real z )
		{
			SetPosition( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position
		 *\param[in]	p_coords	The new value
		 *\~french
		 *\brief		Définit la position du vertice
		 *\param[in]	p_coords	La nouvelle valeur
		 */
		inline void SetPosition( real const * p_coords )
		{
			SetPosition( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la normale du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetNormal( Castor::Coords3r const & val )
		{
			SetNormal( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la normale du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetNormal( Castor::Point3r const & val )
		{
			SetNormal( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la normale du vertex
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void SetNormal( real x, real y, real z )
		{
			SetNormal( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la normale du vertex
		 *\param[in]	p_coords	Le buffer de données
		 */
		inline void SetNormal( real const * p_coords )
		{
			SetNormal( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la tangente du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetTangent( Castor::Coords3r const & val )
		{
			SetTangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la tangente du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetTangent( Castor::Point3r const & val )
		{
			SetTangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la tangente du vertex
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void SetTangent( real x, real y, real z )
		{
			SetTangent( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la tangente du vertex
		 *\param[in]	p_coords	Le buffer de données
		 */
		inline void SetTangent( real const * p_coords )
		{
			SetTangent( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la bitangente du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetBitangent( Castor::Coords3r const & val )
		{
			SetBitangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit la bitangente du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetBitangent( Castor::Point3r const & val )
		{
			SetBitangent( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit la bitangente du vertex
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void SetBitangent( real x, real y, real z )
		{
			SetBitangent( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit la bitangente du vertex
		 *\param[in]	p_coords	Le buffer de données
		 */
		inline void SetBitangent( real const * p_coords )
		{
			SetBitangent( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetTexCoord( Castor::Coords3r const & val )
		{
			SetTexCoord( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates
		 *\param[in]	val	The value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex
		 *\param[in]	val	La valeur
		 */
		inline void SetTexCoord( Castor::Point3r const & val )
		{
			SetTexCoord( m_group.ptr(), val );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates
		 *\param[in]	x, y, z	The values
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex
		 *\param[in]	x, y, z	Les valeurs
		 */
		inline void SetTexCoord( real x, real y, real z = 0 )
		{
			SetTexCoord( m_group.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates
		 *\param[in]	p_coords	The data buffer
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex
		 *\param[in]	p_coords	Le buffer de données
		 */
		inline void SetTexCoord( real const * p_coords )
		{
			SetTexCoord( m_group.ptr(), p_coords );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du vertice
		 *\return		La valeur
		 */
		inline Castor::Coords3r & GetPosition( Castor::Coords3r & p_ptPos )
		{
			return GetPosition( m_group.ptr(), p_ptPos );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du vertice
		 *\return		La valeur
		 */
		inline Castor::Point3r & GetPosition( Castor::Point3r & p_ptPos )const
		{
			return GetPosition( m_group.const_ptr(), p_ptPos );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du vertice
		 *\return		La valeur
		 */
		inline Castor::Coords3r & GetNormal( Castor::Coords3r & p_ptNml )
		{
			return GetNormal( m_group.ptr(), p_ptNml );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du vertice
		 *\return		La valeur
		 */
		inline Castor::Point3r & GetNormal( Castor::Point3r & p_ptNml )const
		{
			return GetNormal( m_group.const_ptr(), p_ptNml );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangent du vertice
		 *\return		La valeur
		 */
		inline Castor::Coords3r & GetTangent( Castor::Coords3r & p_ptTan )
		{
			return GetTangent( m_group.ptr(), p_ptTan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex tangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangent du vertice
		 *\return		La valeur
		 */
		inline Castor::Point3r & GetTangent( Castor::Point3r & p_ptTan )const
		{
			return GetTangent( m_group.const_ptr(), p_ptTan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du vertice
		 *\return		La valeur
		 */
		inline Castor::Coords3r & GetBitangent( Castor::Coords3r & p_ptBitan )
		{
			return GetBitangent( m_group.ptr(), p_ptBitan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex bitangent
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente du vertice
		 *\return		La valeur
		 */
		inline Castor::Point3r & GetBitangent( Castor::Point3r & p_ptBitan )const
		{
			return GetBitangent( m_group.const_ptr(), p_ptBitan );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du vertice
		 *\return		La valeur
		 */
		inline Castor::Coords3r & GetTexCoord( Castor::Coords3r & p_ptTex )
		{
			return GetTexCoord( m_group.ptr(), p_ptTex );
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex texture coordinates
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture du vertice
		 *\return		La valeur
		 */
		inline Castor::Point3r & GetTexCoord( Castor::Point3r & p_ptTex )const
		{
			return GetTexCoord( m_group.const_ptr(), p_ptTex );
		}
		/**
		 *\~english
		 *\brief		Retrieves position elements count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le compte des éléments de la position
		 *\return		La valeur
		 */
		static inline const uint32_t GetCountPos()
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
		static inline const uint32_t GetCountNml()
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
		static inline const uint32_t GetCountTan()
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
		static inline const uint32_t GetCountBin()
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
		static inline const uint32_t GetCountTex()
		{
			return sm_uiCountTex;
		}
		/**
		 *\~english
		 *\brief		Retrieves position's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer de la position
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetPos()
		{
			return sm_uiOffsetPos;
		}
		/**
		 *\~english
		 *\brief		Retrieves normal's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer de la normale
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetNml()
		{
			return sm_uiOffsetNml;
		}
		/**
		 *\~english
		 *\brief		Retrieves tangent's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer de la tangent
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetTan()
		{
			return sm_uiOffsetTan;
		}
		/**
		 *\~english
		 *\brief		Retrieves bitangent's buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer de la bitangente
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetBin()
		{
			return sm_uiOffsetBin;
		}
		/**
		 *\~english
		 *\brief		Retrieves texture coordinates' buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer des coordonnées de texture
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetTex()
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
		static inline const uint32_t GetSizePos()
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
		static inline const uint32_t GetSizeNml()
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
		static inline const uint32_t GetSizeTan()
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
		static inline const uint32_t GetSizeBin()
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
		static inline const uint32_t GetSizeTex()
		{
			return sm_uiSizeTex;
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Coords3r GetPosition( void * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Point3r GetPosition( void const * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static inline Castor::Coords3r & GetPosition( void * p_pBuffer, Castor::Coords3r & p_coord )
		{
			return p_coord = GetPosition( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex position from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Point3r & GetPosition( void const * p_pBuffer, Castor::Point3r & p_coord )
		{
			return p_coord = GetPosition( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Coords3r GetNormal( void * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Point3r GetNormal( void const * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Coords3r & GetNormal( void * p_pBuffer, Castor::Coords3r & p_coord )
		{
			return p_coord = GetNormal( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex normal from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Point3r & GetNormal( void const * p_pBuffer, Castor::Point3r & p_coord )
		{
			return p_coord = GetNormal( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Coords3r GetTangent( void * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Point3r GetTangent( void const * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Coords3r & GetTangent( void * p_pBuffer, Castor::Coords3r & p_coord )
		{
			return p_coord = GetTangent( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex tangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la tangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Point3r & GetTangent( void const * p_pBuffer, Castor::Point3r & p_coord )
		{
			return p_coord = GetTangent( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Coords3r GetBitangent( void * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Point3r GetBitangent( void const * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Coords3r & GetBitangent( void * p_pBuffer, Castor::Coords3r & p_coord )
		{
			return p_coord = GetBitangent( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex bitangent from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bitangente d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Point3r & GetBitangent( void const * p_pBuffer, Castor::Point3r & p_coord )
		{
			return p_coord = GetBitangent( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Coords3r GetTexCoord( void * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\return		La valeur
		 */
		C3D_API static Castor::Point3r GetTexCoord( void const * p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Coords3r & GetTexCoord( void * p_pBuffer, Castor::Coords3r & p_coord )
		{
			return p_coord = GetTexCoord( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Retrieves a vertex texture coordinates from a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[out]	p_coord		Receives the value
		 *\return		The value
		 *\~french
		 *\brief		Récupère les coordonnées de texture d'un vertex à partir d'un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[out]	p_coord		Reçoit la valeur
		 *\return		La valeur
		 */
		static Castor::Point3r & GetTexCoord( void const * p_pBuffer, Castor::Point3r & p_coord )
		{
			return p_coord = GetTexCoord( p_pBuffer );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetPosition( void * p_pBuffer, Castor::Coords3r const & p_coord )
		{
			SetPosition( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetPosition( void * p_pBuffer, Castor::Point3r const & p_coord )
		{
			SetPosition( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex position in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void SetPosition( void * p_pBuffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		Sets the vertex position in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la position du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void SetPosition( void * p_pBuffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		Sets the vertex normal in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetNormal( void * p_pBuffer, Castor::Coords3r const & p_coord )
		{
			SetNormal( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetNormal( void * p_pBuffer, Castor::Point3r const & p_coord )
		{
			SetNormal( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex normal in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void SetNormal( void * p_pBuffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		Sets the vertex normal in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la normale du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void SetNormal( void * p_pBuffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		Sets the vertex tangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTangent( void * p_pBuffer, Castor::Coords3r const & p_coord )
		{
			SetTangent( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTangent( void * p_pBuffer, Castor::Point3r const & p_coord )
		{
			SetTangent( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex tangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void SetTangent( void * p_pBuffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		Sets the vertex tangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la tangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void SetTangent( void * p_pBuffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetBitangent( void * p_pBuffer, Castor::Coords3r const & p_coord )
		{
			SetBitangent( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetBitangent( void * p_pBuffer, Castor::Point3r const & p_coord )
		{
			SetBitangent( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void SetBitangent( void * p_pBuffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		Sets the vertex bitangent in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit la bitangente du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void SetBitangent( void * p_pBuffer, real x, real y, real z );
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTexCoord( void * p_pBuffer, Castor::Coords3r const & p_coord )
		{
			SetTexCoord( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTexCoord( void * p_pBuffer, Castor::Point3r const & p_coord )
		{
			SetTexCoord( p_pBuffer, p_coord.const_ptr() );
		}
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		C3D_API static void SetTexCoord( void * p_pBuffer, real const * p_coord );
		/**
		 *\~english
		 *\brief		Sets the vertex texture coordinates in a buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\param[in]	x,y,z		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du vertex dans un buffer
		 *\param[in]	p_pBuffer	Le buffer
		 *\param[in]	x,y,z		La nouvelle valeur
		 */
		C3D_API static void SetTexCoord( void * p_pBuffer, real x, real y, real z = 0 );
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
		static inline Castor::Coords3r & GetPosition( BufferElementGroupSPtr p_pElement, Castor::Coords3r & p_coord )
		{
			return GetPosition( p_pElement->ptr(), p_coord );
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
		static inline Castor::Point3r & GetPosition( BufferElementGroupSPtr p_pElement, Castor::Point3r & p_point )
		{
			return GetPosition( p_pElement->const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetNormal( BufferElementGroupSPtr p_pElement, Castor::Coords3r & p_coord )
		{
			return GetNormal( p_pElement->ptr(), p_coord );
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
		static inline Castor::Point3r & GetNormal( BufferElementGroupSPtr p_pElement, Castor::Point3r & p_point )
		{
			return GetNormal( p_pElement->const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetTangent( BufferElementGroupSPtr p_pElement, Castor::Coords3r & p_coord )
		{
			return GetTangent( p_pElement->ptr(), p_coord );
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
		static inline Castor::Point3r & GetTangent( BufferElementGroupSPtr p_pElement, Castor::Point3r & p_point )
		{
			return GetTangent( p_pElement->const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetBitangent( BufferElementGroupSPtr p_pElement, Castor::Coords3r & p_coord )
		{
			return GetBitangent( p_pElement->ptr(), p_coord );
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
		static inline Castor::Point3r & GetBitangent( BufferElementGroupSPtr p_pElement, Castor::Point3r & p_point )
		{
			return GetBitangent( p_pElement->const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetTexCoord( BufferElementGroupSPtr p_pElement, Castor::Coords3r & p_coord )
		{
			return GetTexCoord( p_pElement->ptr(), p_coord );
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
		static inline Castor::Point3r & GetTexCoord( BufferElementGroupSPtr p_pElement, Castor::Point3r & p_point )
		{
			return GetTexCoord( p_pElement->const_ptr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroupSPtr p_pElement, Castor::Coords3r const & p_coord )
		{
			SetPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroupSPtr p_pElement, Castor::Point3r const & p_coord )
		{
			SetPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			SetPosition( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			SetPosition( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroupSPtr p_pElement, Castor::Coords3r const & p_coord )
		{
			SetNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroupSPtr p_pElement, Castor::Point3r const & p_coord )
		{
			SetNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			SetNormal( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			SetNormal( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroupSPtr p_pElement, Castor::Coords3r const & p_coord )
		{
			SetTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroupSPtr p_pElement, Castor::Point3r const & p_coord )
		{
			SetTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			SetTangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			SetTangent( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroupSPtr p_pElement, Castor::Coords3r const & p_coord )
		{
			SetBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroupSPtr p_pElement, Castor::Point3r const & p_coord )
		{
			SetBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			SetBitangent( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroupSPtr p_pElement, real x, real y, real z )
		{
			SetBitangent( p_pElement->ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroupSPtr p_pElement, Castor::Coords3r const & p_coord )
		{
			SetTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroupSPtr p_pElement, Castor::Point3r const & p_coord )
		{
			SetTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	p_coord		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	p_coord		La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroupSPtr p_pElement, real const * p_coord )
		{
			SetTexCoord( p_pElement->ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[in]	p_pElement	The group
		 *\param[in]	x, y, z		The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\param[in]	x, y, z		La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroupSPtr p_pElement, real x, real y, real z = 0 )
		{
			SetTexCoord( p_pElement->ptr(), x, y, z );
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
		static inline Castor::Coords3r & GetPosition( BufferElementGroup & p_element, Castor::Coords3r & p_coord )
		{
			return GetPosition( p_element.ptr(), p_coord );
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
		static inline Castor::Point3r & GetPosition( BufferElementGroup const & p_element, Castor::Point3r & p_point )
		{
			return GetPosition( p_element.const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetNormal( BufferElementGroup & p_element, Castor::Coords3r & p_coord )
		{
			return GetNormal( p_element.ptr(), p_coord );
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
		static inline Castor::Point3r & GetNormal( BufferElementGroup const & p_element, Castor::Point3r & p_point )
		{
			return GetNormal( p_element.const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetTangent( BufferElementGroup & p_element, Castor::Coords3r & p_coord )
		{
			return GetTangent( p_element.ptr(), p_coord );
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
		static inline Castor::Point3r & GetTangent( BufferElementGroup const & p_element, Castor::Point3r & p_point )
		{
			return GetTangent( p_element.const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetBitangent( BufferElementGroup & p_element, Castor::Coords3r & p_coord )
		{
			return GetBitangent( p_element.ptr(), p_coord );
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
		static inline Castor::Point3r & GetBitangent( BufferElementGroup const & p_element, Castor::Point3r & p_point )
		{
			return GetBitangent( p_element.const_ptr(), p_point );
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
		static inline Castor::Coords3r & GetTexCoord( BufferElementGroup & p_element, Castor::Coords3r & p_coord )
		{
			return GetTexCoord( p_element.ptr(), p_coord );
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
		static inline Castor::Point3r & GetTexCoord( BufferElementGroup const & p_element, Castor::Point3r & p_point )
		{
			return GetTexCoord( p_element.const_ptr(), p_point );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroup & p_element, Castor::Coords3r const & p_coord )
		{
			SetPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroup & p_element, Castor::Point3r const & p_coord )
		{
			SetPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroup & p_element, real const * p_coord )
		{
			SetPosition( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group position
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la position du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void SetPosition( BufferElementGroup & p_element, real x, real y, real z )
		{
			SetPosition( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroup & p_element, Castor::Coords3r const & p_coord )
		{
			SetNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroup & p_element, Castor::Point3r const & p_coord )
		{
			SetNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroup & p_element, real const * p_coord )
		{
			SetNormal( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group normal
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la normale du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void SetNormal( BufferElementGroup & p_element, real x, real y, real z )
		{
			SetNormal( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroup & p_element, Castor::Coords3r const & p_coord )
		{
			SetTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroup & p_element, Castor::Point3r const & p_coord )
		{
			SetTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroup & p_element, real const * p_coord )
		{
			SetTangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group tangent
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la tangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void SetTangent( BufferElementGroup & p_element, real x, real y, real z )
		{
			SetTangent( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroup & p_element, Castor::Coords3r const & p_coord )
		{
			SetBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroup & p_element, Castor::Point3r const & p_coord )
		{
			SetBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroup & p_element, real const * p_coord )
		{
			SetBitangent( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group bitangent
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit la bitangente du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void SetBitangent( BufferElementGroup & p_element, real x, real y, real z )
		{
			SetBitangent( p_element.ptr(), x, y, z );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroup & p_element, Castor::Coords3r const & p_coord )
		{
			SetTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroup & p_element, Castor::Point3r const & p_coord )
		{
			SetTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	p_coord	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_coord	La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroup & p_element, real const * p_coord )
		{
			SetTexCoord( p_element.ptr(), p_coord );
		}
		/**
		 *\~english
		 *\brief		Sets the group texture coordinates
		 *\param[out]	p_element	The group
		 *\param[in]	x, y, z	The new value
		 *\~french
		 *\brief		Définit les coordonnées de texture du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	x, y, z	La nouvelle valeur
		 */
		static inline void SetTexCoord( BufferElementGroup & p_element, real x, real y, real z = 0 )
		{
			SetTexCoord( p_element.ptr(), x, y, z );
		}

	protected:
		//!\~english Vertex position elements count	\~french Compte des élément de la position du vertex
		static const uint32_t sm_uiCountPos = 3;
		//!\~english Vertex normal elements count	\~french Compte des élément de la normale du vertex
		static const uint32_t sm_uiCountNml = 3;
		//!\~english Vertex tangent elements count	\~french Compte des élément de la tangente du vertex
		static const uint32_t sm_uiCountTan = 3;
		//!\~english Vertex bitangent elements count	\~french Compte des élément de la bitangente du vertex
		static const uint32_t sm_uiCountBin = 3;
		//!\~english Vertex texture coordinates elements count	\~french Compte des élément des coordonnées de texture du vertex
		static const uint32_t sm_uiCountTex = 3;
		//!\~english Vertex position binary size	\~french Taille en octets de la position du vertex
		static const uint32_t sm_uiSizePos = uint32_t( sm_uiCountPos * sizeof( real ) );
		//!\~english Vertex normal binary size	\~french Taille en octets de la normale du vertex
		static const uint32_t sm_uiSizeNml = uint32_t( sm_uiCountNml * sizeof( real ) );
		//!\~english Vertex tangent binary size	\~french Taille en octets de la tangente du vertex
		static const uint32_t sm_uiSizeTan = uint32_t( sm_uiCountTan * sizeof( real ) );
		//!\~english Vertex bitangent binary size	\~french Taille en octets de la bitangente du vertex
		static const uint32_t sm_uiSizeBin = uint32_t( sm_uiCountBin * sizeof( real ) );
		//!\~english Vertex texture coordinates binary size	\~french Taille en octets des coordonnées de texture du vertex
		static const uint32_t sm_uiSizeTex = uint32_t( sm_uiCountTex * sizeof( real ) );
		//!\~english Vertex position's buffer offset	\~french Offset dans le buffer de la position du vertex
		static const uint32_t sm_uiOffsetPos = 0;
		//!\~english Vertex normal's buffer offset	\~french Offset dans le buffer de la normale du vertex
		static const uint32_t sm_uiOffsetNml = sm_uiOffsetPos + sm_uiSizePos;
		//!\~english Vertex tangent's buffer offset	\~french Offset dans le buffer de la tangente du vertex
		static const uint32_t sm_uiOffsetTan = sm_uiOffsetNml + sm_uiSizeNml;
		//!\~english Vertex bitangent's buffer offset	\~french Offset dans le buffer de la bitangente du vertex
		static const uint32_t sm_uiOffsetBin = sm_uiOffsetTan + sm_uiSizeTan;
		//!\~english Vertex texture coordinates' buffer offset	\~french Offset dans le buffer des coordonnées de texture du vertex
		static const uint32_t sm_uiOffsetTex = sm_uiOffsetBin + sm_uiSizeBin;

		//!\~english The wrapped group	\~french Le groupe encadré
		BufferElementGroup & m_group;
	};
}

#endif

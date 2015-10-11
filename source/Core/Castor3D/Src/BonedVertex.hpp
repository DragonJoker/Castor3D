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
#ifndef ___C3D_BONED_VERTEX_H___
#define ___C3D_BONED_VERTEX_H___

#include "Vertex.hpp"
#include "VertexBoneData.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		27/06/2013
	\~english
	\brief		Vertex with bones infos representation
	\remark		Vertex Specialisation adding bone infos retrieval.
	\~fench
	\brief		Représentation d'un vertex avec des informations des bones
	\remark		Spécialisation de Vertex ajoutant la récupération des informations des bones
	*/
	class BonedVertex
		: public Vertex
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
		C3D_API BonedVertex( BufferElementGroup & p_group );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_source		The source object
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_source		L'objet source
		 */
		C3D_API BonedVertex( BonedVertex const & p_source );
		/**
		 *\~english
		 *\brief		Constructor from Vertex
		 *\param[in]	p_source		The source object
		 *\~french
		 *\brief		Constructeur à partir d'un Vertex
		 *\param[in]	p_source		L'objet source
		 */
		C3D_API BonedVertex( Vertex const & p_source );
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
		C3D_API BonedVertex & operator=( BonedVertex const & p_source );
		/**
		 *\~english
		 *\brief		Assignment operator from a Vertex
		 *\param[in]	p_source		The source object
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation à partir d'un Vertex
		 *\param[in]	p_source		L'objet source
		 *\return		Une référence sur cet objet
		 */
		C3D_API BonedVertex & operator=( Vertex const & p_source );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BonedVertex();
		/**
		 *\~english
		 *\brief		Cast operator to Vertex
		 *\return		A Vertex
		 *\~french
		 *\brief		Opérateur de cast vers Vertex
		 *\return		Un Vertex
		 */
		operator Vertex()
		{
			return *this;
		}
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\return		La valeur
		 */
		C3D_API stVERTEX_BONE_DATA GetBones()const;
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[in]	p_pData		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[in]	p_pData		La nouvelle valeur
		 */
		C3D_API void SetBones( stVERTEX_BONE_DATA * p_pData );
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[in]	p_data		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[in]	p_data		La nouvelle valeur
		 */
		C3D_API void SetBones( stVERTEX_BONE_DATA const & p_data );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_pElement	The group
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_pElement	Le groupe
		 *\return		La valeur
		 */
		C3D_API static stVERTEX_BONE_DATA GetBones( BufferElementGroupSPtr p_pElement );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_element	The group
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_element	Le groupe
		 *\return		La valeur
		 */
		C3D_API static stVERTEX_BONE_DATA GetBones( BufferElementGroup const & p_element );
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[out]	p_pElement	The group
		 *\param[in]	p_pData		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_pElement	Le groupe
		 *\param[in]	p_pData		La nouvelle valeur
		 */
		C3D_API static void SetBones( BufferElementGroupSPtr p_pElement, stVERTEX_BONE_DATA * p_pData );
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[out]	p_pElement	The group
		 *\param[in]	p_pData		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_pElement	Le groupe
		 *\param[in]	p_pData		La nouvelle valeur
		 */
		C3D_API static void SetBones( BufferElementGroupSPtr p_pElement, stVERTEX_BONE_DATA const & p_pData );
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[out]	p_element	The group
		 *\param[in]	p_pData		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_pData		La nouvelle valeur
		 */
		C3D_API static void SetBones( BufferElementGroup & p_element, stVERTEX_BONE_DATA * p_pData );
		/**
		 *\~english
		 *\brief		Sets the group bones datas
		 *\param[out]	p_element	The group
		 *\param[in]	p_pData		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_element	Le groupe
		 *\param[in]	p_pData		La nouvelle valeur
		 */
		C3D_API static void SetBones( BufferElementGroup & p_element, stVERTEX_BONE_DATA const & p_pData );
		/**
		 *\~english
		 *\brief		Retrieves bones informations' buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t GetOffsetBon()
		{
			return sm_uiOffsetBon;
		}
		/**
		 *\~english
		 *\brief		Retrieves bones informations binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t GetSizeBon()
		{
			return sm_uiSizeBon;
		}

	protected:
		//!\~english Bones informations binary size	\~french Taille en octets des informations des bones
		static const uint32_t sm_uiSizeBon = uint32_t( 4 * ( sizeof( real ) + sizeof( uint32_t ) ) );
		//!\~english Bones informations buffer offset	\~french Offset dans le buffer des informations des bones
		static const uint32_t sm_uiOffsetBon = sm_uiOffsetTex + sm_uiSizeTex;
	};
}

#endif

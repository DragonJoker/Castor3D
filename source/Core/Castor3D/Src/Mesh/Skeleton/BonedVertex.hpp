/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BONED_VERTEX_H___
#define ___C3D_BONED_VERTEX_H___

#include "Mesh/Buffer/BufferElementGroup.hpp"

#include "VertexBoneData.hpp"

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
	\brief		Vertex with bones infos representation
	\remark		Vertex Specialisation adding bone infos retrieval.
	\~fench
	\brief		Représentation d'un vertex avec des informations des bones
	\remark		Spécialisation de Vertex ajoutant la récupération des informations des bones
	*/
	class BonedVertex
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
		C3D_API explicit BonedVertex( BufferElementGroup & p_group );
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
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BonedVertex();
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\return		La valeur
		 */
		C3D_API VertexBoneData getBones()const;
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[in]	p_data		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[in]	p_data		La nouvelle valeur
		 */
		C3D_API void setBones( VertexBoneData * p_data );
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[in]	p_data		The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[in]	p_data		La nouvelle valeur
		 */
		C3D_API void setBones( VertexBoneData const & p_data );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_group	The group
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_group	Le groupe
		 *\return		La valeur
		 */
		C3D_API static VertexBoneData getBones( BufferElementGroupSPtr p_group );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_group	The group
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_group	Le groupe
		 *\return		La valeur
		 */
		C3D_API static VertexBoneData const & getBones( BufferElementGroup const & p_group );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_group	The group
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_group	Le groupe
		 *\return		La valeur
		 */
		C3D_API static VertexBoneData & getBones( BufferElementGroup & p_group );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_data	The datas
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_data	Les données
		 *\return		La valeur
		 */
		C3D_API static VertexBoneData const & getBones( void const * p_data );
		/**
		 *\~english
		 *\brief		Retrieves the group bones data
		 *\param[in]	p_data	The datas
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données de bones du groupe
		 *\param[in]	p_data	Les données
		 *\return		La valeur
		 */
		C3D_API static VertexBoneData & getBones( void * p_data );
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[out]	p_group	The group
		 *\param[in]	p_pData	The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_group	Le groupe
		 *\param[in]	p_pData	La nouvelle valeur
		 */
		C3D_API static void setBones( BufferElementGroupSPtr p_group, VertexBoneData * p_pData );
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[out]	p_group	The group
		 *\param[in]	p_data	The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_group	Le groupe
		 *\param[in]	p_data	La nouvelle valeur
		 */
		C3D_API static void setBones( BufferElementGroupSPtr p_group, VertexBoneData const & p_data );
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[out]	p_group	The group
		 *\param[in]	p_data	The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_group	Le groupe
		 *\param[in]	p_data	La nouvelle valeur
		 */
		C3D_API static void setBones( BufferElementGroup & p_group, VertexBoneData * p_data );
		/**
		 *\~english
		 *\brief		sets the group bones datas
		 *\param[out]	p_group	The group
		 *\param[in]	p_data	The new value
		 *\~french
		 *\brief		Définit les données de bones du groupe
		 *\param[out]	p_group	Le groupe
		 *\param[in]	p_data	La nouvelle valeur
		 */
		C3D_API static void setBones( BufferElementGroup & p_group, VertexBoneData const & p_data );
		/**
		 *\~english
		 *\brief		Retrieves bones informations' buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetIds()
		{
			return sm_uiOffsetIds;
		}
		/**
		 *\~english
		 *\brief		Retrieves bones informations binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeIds()
		{
			return sm_uiSizeIds;
		}
		/**
		 *\~english
		 *\brief		Retrieves bones informations' buffer offset
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'offset dans le buffer des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t getOffsetWeights()
		{
			return sm_uiOffsetWeights;
		}
		/**
		 *\~english
		 *\brief		Retrieves bones informations binary size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille en octets des informations des bones
		 *\return		La valeur
		 */
		static inline const uint32_t getSizeWeights()
		{
			return sm_uiSizeWeights;
		}

	private:
		//!\~english	Bones informations binary size.
		//!\~french		Taille en octets des informations des bones.
		static const uint32_t sm_uiSizeIds = uint32_t( C3D_MAX_BONES_PER_VERTEX * sizeof( uint32_t ) );
		//!\~english	Bones informations buffer offset.
		//!\~french		Offset dans le buffer des informations des bones.
		static const uint32_t sm_uiOffsetIds = 0;
		//!\~english	Bones informations binary size.
		//!\~french		Taille en octets des informations des bones.
		static const uint32_t sm_uiSizeWeights = uint32_t( C3D_MAX_BONES_PER_VERTEX * sizeof( real ) );
		//!\~english	Bones informations buffer offset.
		//!\~french		Offset dans le buffer des informations des bones.
		static const uint32_t sm_uiOffsetWeights = sm_uiOffsetIds + sm_uiSizeIds;
		//!\~english	The wrapped group.
		//!\~french		Le groupe encapsulé.
		BufferElementGroup & m_group;

	public:
		//!\~english	The vertex bones data size.
		//!\~french		La taille des données de bones pour un sommet.
		static const uint32_t Stride = sm_uiOffsetWeights + sm_uiSizeWeights;
	};
}

#endif

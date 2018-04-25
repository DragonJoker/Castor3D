/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERTEX_BONE_DATA_H___
#define ___C3D_VERTEX_BONE_DATA_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	static const uint32_t C3D_MAX_BONES_PER_VERTEX = 8;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		26/06/2013
	\~english
	\brief		Holds bone data for one vertex
	\~french
	\brief		Contient les données de bones pour un vertice
	*/
	struct VertexBoneData
	{
		C3D_API VertexBoneData();
		C3D_API VertexBoneData( VertexBoneData const & rhs );
		C3D_API VertexBoneData( VertexBoneData && rhs );
		C3D_API ~VertexBoneData();
		C3D_API VertexBoneData & operator=( VertexBoneData const & rhs );
		C3D_API VertexBoneData & operator=( VertexBoneData && rhs );
		/**
		 *\~english
		 *\brief		adds bone informations to the vertex
		 *\param[in]	p_boneId	The bone ID
		 *\param[in]	p_weight	The bone weight
		 *\~french
		 *\brief		Ajoute des informations de bone au vertice
		 *\param[in]	p_boneId	L'ID du bone
		 *\param[in]	p_weight	Le poids du bone
		 */
		C3D_API void addBoneData( uint32_t p_boneId, real p_weight );

		//!\~english	The bones ID.
		//!\~french		L'ID des bones.
		union Ids
		{
			struct
			{
				castor::Point4ui id0;
				castor::Point4ui id1;
			};
			std::array< uint32_t, C3D_MAX_BONES_PER_VERTEX > data;
			Ids()
				: data{}
			{
			}
			Ids( Ids const & rhs )
				: data{ rhs.data }
			{
			}
			Ids( Ids && rhs )
				: data{ std::move( rhs.data ) }
			{
			}
			~Ids()
			{
			}
			Ids & operator=( Ids const & rhs )
			{
				data = rhs.data;
				return *this;
			}
			Ids & operator=( Ids && rhs )
			{
				if ( this != &rhs )
				{
					data = std::move( rhs.data );
				}

				return *this;
			}
			uint32_t & operator[]( size_t index )
			{
				return data[index];
			}
			uint32_t const & operator[]( size_t index )const
			{
				return data[index];
			}
			size_t size()const
			{
				return data.size();
			}
		};
		Ids m_ids;
		//!\~english	The bones weights.
		//!\~french		Les poids des bones.
		union Weights
		{
			struct
			{
				castor::Point4f weight0;
				castor::Point4f weight1;
			};
			std::array< float, C3D_MAX_BONES_PER_VERTEX > data;
			Weights()
				: data{}
			{
			}
			Weights( Weights const & rhs )
				: data{ rhs.data }
			{
			}
			Weights( Weights && rhs )
				: data{ std::move( rhs.data ) }
			{
			}
			~Weights()
			{
			}
			Weights & operator=( Weights const & rhs )
			{
				data = rhs.data;
				return *this;
			}
			Weights & operator=( Weights && rhs )
			{
				if ( this != &rhs )
				{
					data = std::move( rhs.data );
				}

				return *this;
			}
			float & operator[]( size_t index )
			{
				return data[index];
			}
			float const & operator[]( size_t index )const
			{
				return data[index];
			}
			size_t size()const
			{
				return data.size();
			}
		};
		Weights m_weights;
	};
	using VertexBoneDataArray = std::vector< VertexBoneData >;
}

#endif

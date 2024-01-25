/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VertexBoneData_H___
#define ___C3D_VertexBoneData_H___

#include "SkeletonModule.hpp"

#include "Castor3D/Limits.hpp"

namespace castor3d
{
	struct VertexBoneData
	{
		C3D_API VertexBoneData() = default;
		/**
		 *\~english
		 *\brief		adds bone informations to the vertex
		 *\param[in]	boneId	The bone ID
		 *\param[in]	weight	The bone weight
		 *\~french
		 *\brief		Ajoute des informations de bone au vertice
		 *\param[in]	boneId	L'ID du bone
		 *\param[in]	weight	Le poids du bone
		 */
		C3D_API void addBoneData( uint32_t boneId
			, float weight );

		//!\~english	The bones ID.
		//!\~french		L'ID des bones.
		union Ids
		{
			struct ids
			{
				castor::Point4ui id0;
				castor::Point4ui id1;
			};
			castor::Array< uint32_t, MaxBonesPerVertex > data{};

			Ids() = default;

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
			auto begin()const
			{
				return data.begin();
			}
			auto begin()
			{
				return data.begin();
			}
			auto end()const
			{
				return data.end();
			}
			auto end()
			{
				return data.end();
			}
		};
		Ids m_ids;
		//!\~english	The bones weights.
		//!\~french		Les poids des bones.
		union Weights
		{
			struct weights
			{
				castor::Point4f weight0;
				castor::Point4f weight1;
			};
			castor::Array< float, MaxBonesPerVertex > data{};

			Weights() = default;

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
			auto begin()const
			{
				return data.begin();
			}
			auto begin()
			{
				return data.begin();
			}
			auto end()const
			{
				return data.end();
			}
			auto end()
			{
				return data.end();
			}
		};
		Weights m_weights;
	};
}

#endif

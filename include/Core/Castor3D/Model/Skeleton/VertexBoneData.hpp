/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VertexBoneData_H___
#define ___C3D_VertexBoneData_H___

#include "SkeletonModule.hpp"

namespace castor3d
{
	struct VertexBoneData
	{
		C3D_API VertexBoneData();
		C3D_API VertexBoneData( VertexBoneData const & rhs );
		C3D_API VertexBoneData( VertexBoneData && rhs )noexcept;
		C3D_API ~VertexBoneData();
		C3D_API VertexBoneData & operator=( VertexBoneData const & rhs );
		C3D_API VertexBoneData & operator=( VertexBoneData && rhs )noexcept;
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
			std::array< uint32_t, MaxBonesPerVertex > data;
			Ids()
				: data{}
			{
			}
			Ids( Ids const & rhs )
				: data{ rhs.data }
			{
			}
			Ids( Ids && rhs )noexcept
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
			Ids & operator=( Ids && rhs )noexcept
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
			std::array< float, MaxBonesPerVertex > data;
			Weights()
				: data{}
			{
			}
			Weights( Weights const & rhs )
				: data{ rhs.data }
			{
			}
			Weights( Weights && rhs )noexcept
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
			Weights & operator=( Weights && rhs )noexcept
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

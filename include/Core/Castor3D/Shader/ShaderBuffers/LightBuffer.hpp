/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightBuffer_H___
#define ___C3D_LightBuffer_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

namespace castor3d
{
	class LightBuffer
	{
	public:
		struct Float4
		{
			Float4 & operator=( float rhs )
			{
				x = rhs;
				return *this;
			}

			Float4 & operator=( castor::Point2f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			Float4 & operator=( castor::Point3f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				return *this;
			}

			Float4 & operator=( castor::Point4f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				w = rhs->w;
				return *this;
			}

			float x{};
			float y{};
			float z{};
			float w{};
		};
		struct Float4x4
		{
			Float4x4 & operator=( castor::Matrix4x4f const & rhs )
			{
				c0 = rhs[0];
				c1 = rhs[1];
				c2 = rhs[2];
				c3 = rhs[3];
				return *this;
			}

			Float4 c0{};
			Float4 c1{};
			Float4 c2{};
			Float4 c3{};
		};

		union SpecLight
		{
			struct
			{
#	if C3D_UseTiledDirectionalShadowMap
				// DirectionalLight => BaseLightComponentsCount + 30 => BaseLightComponentsCount + 1(directionCount) + 1(tiles) + 2(splitDepths) + 2(splitScales) + (6 * 4)(transforms)
				Float4 directionCount;
				Float4 tiles;
				std::array< Float4, 2u > splitDepths;
				std::array< Float4, 2u > splitScales;
				std::array< Float4x4, 4u > transforms;
#	else
				Float4 directionCount;
				Float4 splitDepths;
				Float4 splitScales;
				std::array< Float4x4, 4u > transforms;
#	endif
			} directional;
			struct
			{
				Float4 position;
				Float4 attenuation;
			} point;
			struct
			{
				Float4 position;
				Float4 attenuation;
				Float4 direction;
				Float4 exponentCutOff;
				Float4x4 transform;
			} spot;
		};

		struct LightData
		{
			Float4 colourIndex;
			Float4 intensityFarPlane;
			Float4 volumetric;
			Float4 shadowsOffsets;
			Float4 shadowsVariances;
			SpecLight specific;
		};
		using LightsData = castor::ArrayView< LightData >;

		static constexpr uint32_t DataSize = sizeof( LightData );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max passes count.
		 *\param[in]	size	The size of a pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de passes.
		 *\param[in]	size	La taille d'une passe.
		 */
		C3D_API LightBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a light source to the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse au tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void addLight( Light & light );
		/**
		 *\~english
		 *\brief		Removes a light source from the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Supprime une source lumineuse du tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void removeLight( Light & light );
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates a frame pass binding.
		 *\~french
		 *\brief		Crée une attache de frame pass.
		 */
		C3D_API void createPassBinding( crg::FramePass & pass, uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for this buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for this buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding
			, VkDeviceSize offset
			, VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set binding at given point.
		 *\param[in]	descriptorSet	The descriptor set that receives the binding.
		 *\param[in]	binding			The descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de set de descripteurs au point donné.
		 *\param[in]	descriptorSet	Le set de descripteurs recevant l'attache.
		 *\param[in]	binding			L'attache de layout de set de descripteurs.
		 */
		C3D_API void createBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & binding )const;
		/**
		 *\~english
		 *\return		The pointer to the data for given light source.
		 *\~french
		 *\brief		Le pointeur sur les données pour la source lumineuse donnée.
		 */
		C3D_API LightData & getData( Light const & light );
		/**
		 *\~english
		 *\return		The light source buffer index.
		 *\~french
		 *\brief		l'index de la source lumineuse dans le buffer.
		 */
		C3D_API uint32_t getIndex( Light const & light )const;

		uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

		VkDescriptorType getType()const
		{
			return m_buffer.getType();
		}

		LightsArray getLights( LightType type )const
		{
			return m_typeSortedLights[size_t( type )];
		}

		LightData & operator[]( size_t index )
		{
			return m_data[index];
		}

		LightData const & operator[]( size_t index )const
		{
			return m_data[index];
		}

	private:
		uint32_t doGetIndex( Light const & light )const;
		void doUpdateLightsIndices();

	private:
		ShaderBuffer m_buffer;
		LightsMap m_typeSortedLights;
		std::vector< Light * > m_dirty;
		std::map< Light *, OnLightChangedConnection > m_connections;
		LightsData m_data;
		mutable std::mutex m_mutex;
		bool m_wasDirty{};
	};
}

#endif

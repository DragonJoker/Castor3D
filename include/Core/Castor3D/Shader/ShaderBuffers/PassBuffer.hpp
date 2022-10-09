/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassBuffer_H___
#define ___C3D_PassBuffer_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"

#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

namespace castor3d
{
	class PassBuffer
	{
	public:
		struct PassDataPtr
		{
			C3D_API explicit PassDataPtr( castor::ArrayView< uint8_t > data )
				: m_data{ data }
			{
			}

			C3D_API VkDeviceSize write( MemChunk const & chunk
				, uint32_t v
				, VkDeviceSize offset );
			C3D_API VkDeviceSize write( MemChunk const & chunk
				, int32_t v
				, VkDeviceSize offset );
			C3D_API VkDeviceSize write( MemChunk const & chunk
				, float v
				, VkDeviceSize offset );

			template< typename DataA, typename DataB >
			VkDeviceSize write( MemChunk const & chunk
				, DataA a
				, DataB b
				, VkDeviceSize offset )
			{
				auto base = offset;
				offset += write( chunk, a, offset );
				offset += write( chunk, b, offset );
				return offset - base;
			}

			template< typename DataA, typename DataB, typename DataC >
			VkDeviceSize write( MemChunk const & chunk
				, DataA a
				, DataB b
				, DataC c
				, VkDeviceSize offset )
			{
				auto base = offset;
				offset += write( chunk, a, offset );
				offset += write( chunk, b, offset );
				offset += write( chunk, c, offset );
				return offset - base;
			}

			template< typename DataA, typename DataB, typename DataC, typename DataD >
			VkDeviceSize write( MemChunk const & chunk
				, DataA a
				, DataB b
				, DataC c
				, DataD d
				, VkDeviceSize offset )
			{
				auto base = offset;
				offset += write( chunk, a, offset );
				offset += write( chunk, b, offset );
				offset += write( chunk, c, offset );
				offset += write( chunk, d, offset );
				return offset - base;
			}

			template< typename DataT, size_t SizeT >
			VkDeviceSize write( MemChunk const & chunk
				, std::array< DataT, SizeT > const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( size_t i = 0u; i < SizeT; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

			template< typename DataT, uint32_t CountT >
			VkDeviceSize write( MemChunk const & chunk
				, castor::Point< DataT, CountT > const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( uint32_t i = 0u; i < CountT; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

			VkDeviceSize write( MemChunk const & chunk
				, castor::RgbColour const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( size_t i = 0u; i < 3u; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

			VkDeviceSize write( MemChunk const & chunk
				, castor::HdrRgbColour const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( size_t i = 0u; i < 3u; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

			VkDeviceSize write( MemChunk const & chunk
				, castor::RgbaColour const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( size_t i = 0u; i < 4u; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

			VkDeviceSize write( MemChunk const & chunk
				, castor::HdrRgbaColour const & v
				, VkDeviceSize offset )
			{
				auto base = offset;

				for ( size_t i = 0u; i < 4u; ++i )
				{
					offset += write( chunk, v[i], offset );
				}

				return offset - base;
			}

		private:
			castor::ArrayView< uint8_t > m_data;
		};
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API PassBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API uint32_t addPass( Pass & pass );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( Pass & pass );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\param[in]	commandBuffer	Receives the update commands.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 *\param[in]	commandBuffer	Reçoit les commandes de mise à jour.
		 */
		C3D_API void update( SpecificsBuffers const & specifics
			, ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const;
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
		 *\return		The pointer to the data for given pass ID.
		 *\~french
		 *\brief		Le pointeur sur les données pour l'ID de passe donné.
		 */
		C3D_API PassDataPtr getData( uint32_t passID );
		/**
		 *\~english
		 *\return		The maximum pass types count (for visibility buffer use).
		 *\~french
		 *\brief		Le nombre maximum de types de passes (pour l'utilisation de vibility buffer).
		 */
		C3D_API uint32_t getMaxPassTypeCount()const;
		/**
		 *\~english
		 *\return		The pass type index (for visibility buffer use).
		 *\~french
		 *\brief		L'indice de type de passe (pour l'utilisation de vibility buffer).
		 */
		C3D_API uint32_t getPassTypeIndex( PassTypeID passType
			, PassFlags passFlags
			, TextureFlags textureFlags )const;
		/**
		 *\~english
		 *\return		The pass type details for given pass type index (for visibility buffer use).
		 *\~french
		 *\brief		Les détails du type de passe pour l'index donné (pour l'utilisation de vibility buffer).
		 */
		C3D_API std::tuple< PassTypeID, PassFlags, TextureFlags > getPassTypeDetails( uint32_t passTypeIndex )const;
		/**
		 *\~english
		 *\return		The pointer to the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

		uint32_t getCurrentPassTypeCount()const
		{
			return uint32_t( m_passTypeIndices.size() );
		}

	private:
		uint32_t m_stride;
		ShaderBuffer m_buffer;
		std::vector< Pass * > m_passes;
		std::vector< Pass const * > m_dirty;
		std::vector< OnPassChangedConnection > m_connections;
		uint32_t m_passID{ 1u };
		std::unordered_map< uint32_t, uint16_t > m_passTypeIndices;
		castor::ArrayView< uint8_t > m_data;
		std::mutex m_mutex;
	};
}

#endif

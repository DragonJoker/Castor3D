/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureCache_H___
#define ___C3D_TextureCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#include <mutex>
#include <unordered_set>
#pragma warning( pop )

namespace castor3d
{
	C3D_API void mergeConfigs( TextureConfiguration const & lhs
		, TextureConfiguration & rhs );
	C3D_API castor::PixelFormat getSRGBFormat( castor::PixelFormat pformat );
	C3D_API castor::PixelFormat getNonSRGBFormat( castor::PixelFormat pformat );

	class TextureUnitCache
		: public castor::OwnedBy< Engine >
	{
	public:
		struct ThreadData
		{
			ThreadData( TextureSourceInfo psourceInfo
				, PassTextureConfig pconfig
				, TextureUnit & punit )
				: sourceInfo{ psourceInfo }
				, config{ pconfig }
				, unit{ &punit }
			{
			}

			TextureSourceInfo sourceInfo;
			PassTextureConfig config;
			castor::PxBufferBaseUPtr buffer{};
			TextureLayoutSPtr layout{};
			TextureUnit * unit{};
			uint32_t tiles{ 1u };
		};

	public:
		C3D_API explicit TextureUnitCache( Engine & engine );
		C3D_API ~TextureUnitCache();

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();

		C3D_API void update( GpuUpdater & updater );

		C3D_API void notifyPassChange( Pass & pass );
		C3D_API void preparePass( Pass & pass );

		C3D_API void clear();

		C3D_API TextureUnitSPtr getTexture( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config );
		C3D_API TextureUnitSPtr mergeImages( TextureSourceInfo const & lhsSourceInfo
			, TextureConfiguration const & lhsConfig
			, uint32_t lhsSrcMask
			, uint32_t lhsDstMask
			, TextureSourceInfo const & rhsSourceInfo
			, TextureConfiguration const & rhsConfig
			, uint32_t rhsSrcMask
			, uint32_t rhsDstMask
			, castor::String const & name
			, TextureSourceInfo resultSourceInfo
			, TextureConfiguration resultConfig );

		bool hasBindless()const
		{
			return m_bindlessTexLayout && m_bindlessTexSet;
		}

		ashes::DescriptorSetLayout * getDescriptorLayout()const
		{
			return m_bindlessTexLayout.get();
		}

		ashes::DescriptorPool * getDescriptorPool()const
		{
			return m_bindlessTexPool.get();
		}

		ashes::DescriptorSet * getDescriptorSet()const
		{
			return m_bindlessTexSet.get();
		}

	private:
		void doCreateLayout( ThreadData & data
			, castor::String const & name );
		void doLoadSource( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config
			, TextureUnit & unit );
		void doMergeSources( TextureSourceInfo const & lhsSourceInfo
			, TextureConfiguration const & lhsConfig
			, uint32_t lhsSrcMask
			, uint32_t lhsDstMask
			, TextureSourceInfo const & rhsSourceInfo
			, TextureConfiguration const & rhsConfig
			, uint32_t rhsSrcMask
			, uint32_t rhsDstMask
			, TextureSourceInfo const & resultSourceInfo
			, PassTextureConfig const & resultConfig
			, castor::String const & name
			, TextureUnit & unit );
		void doUpload( ThreadData & data );
		void doUpdateWrite( TextureUnit & unit );
		ThreadData & doCreateThreadData( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config
			, TextureUnit & unit );
		void doDestroyThreadData( ThreadData & data );

	private:
		castor::CheckedMutex m_dirtyMtx;
		std::unordered_set< Pass * > m_dirty;
		castor::CheckedMutex m_loadMtx;
		std::vector< std::unique_ptr< ThreadData > > m_loading;
		std::unordered_map< size_t, TextureUnitSPtr > m_loaded;
		ashes::DescriptorSetLayoutPtr m_bindlessTexLayout;
		ashes::DescriptorPoolPtr m_bindlessTexPool;
		ashes::DescriptorSetPtr m_bindlessTexSet;
		std::mutex m_dirtyWritesMtx;
		std::vector< ashes::WriteDescriptorSet > m_dirtyWrites;
	};
}

#endif

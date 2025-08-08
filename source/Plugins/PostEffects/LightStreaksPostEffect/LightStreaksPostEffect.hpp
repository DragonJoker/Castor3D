/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaksPostEffect___
#define ___C3D_LightStreaksPostEffect___

#include "CombinePass.hpp"
#include "HiPass.hpp"
#include "KawasePass.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace light_streaks
{
	class PostEffect
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
		/**
		 *\copydoc		c3d::PostEffect::accept
		 */
		void accept( c3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		c3d::PostEffect::setParameters
		 */
		void setParameters( c3d::Parameters parameters )override;

	private:
		/**
		*\copydoc		c3d::PostEffect::doInitialise
		*/
		bool doInitialise( c3d::RenderDevice const & device
			, c3d::Texture const & source
			, c3d::Texture & target )override;
		/**
		*\copydoc		c3d::PostEffect::doCleanup
		*/
		void doCleanup( c3d::RenderDevice const & device )override;
		/**
		 *\copydoc		c3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( c3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		c3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( c3d::StringStream & file, c3d::String const & tabs )override;

	public:
		static c3d::String const Type;
		static c3d::MbString const Name;
		static uint32_t const Count = 4u;

	private:
		c3d::Texture m_hiImage;
		c3d::Texture m_kawaseImage;
		KawaseConfig m_kawaseCfg;
		KawaseUbo m_kawaseUbo;
		c3d::RawUniquePtr< HiPass > m_hiPass;
		c3d::RawUniquePtr< KawasePass > m_kawasePass;
		c3d::RawUniquePtr< CombinePass > m_combinePass;
	};
}

#endif

/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloomPostEffect___
#define ___C3D_PbrBloomPostEffect___

#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		static castor3d::PostEffectUPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		castor3d::PostEffect::setParameters
		 */
		void setParameters( castor3d::Parameters parameters )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_lastPass );
			return *m_lastPass;
		}

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		bool doInitialise( castor3d::RenderDevice const & device
			, castor3d::Texture const & source
			, castor3d::Texture const & target
			, crg::FramePass const & previousPass )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs )override;

	public:
		static castor::String const Type;
		static castor::MbString const Name;
		static castor::String const FocalDistance;
		static castor::String const FocalLength;
		static castor::String const BokehScale;
		static castor::String const EnableFarBlur;

	private:
		castor3d::Texture m_nearCoC;
		castor3d::Texture m_farCoC;
		castor3d::Texture m_nearBlur;
		castor3d::Texture m_farBlur;
		castor3d::Texture m_intermediate;
		std::unique_ptr< castor3d::GaussianBlur > m_blurNearCoC;
		std::unique_ptr< castor3d::GaussianBlur > m_blurFarCoC;
		crg::FramePass const * m_lastPass{};
		DepthOfFieldConfig m_data{};
		DepthOfFieldUbo m_ubo;
	};
}

#endif

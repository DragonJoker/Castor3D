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
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & param );
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & param );
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
		static c3d::String const FocalDistance;
		static c3d::String const FocalLength;
		static c3d::String const BokehScale;
		static c3d::String const EnableFarBlur;

	private:
		c3d::Texture m_nearCoC;
		c3d::Texture m_farCoC;
		c3d::Texture m_nearBlur;
		c3d::Texture m_farBlur;
		c3d::Texture m_intermediate;
		std::unique_ptr< c3d::GaussianBlur > m_blurNearCoC;
		std::unique_ptr< c3d::GaussianBlur > m_blurFarCoC;
		DepthOfFieldConfig m_data{};
		DepthOfFieldUbo m_ubo;
	};
}

#endif

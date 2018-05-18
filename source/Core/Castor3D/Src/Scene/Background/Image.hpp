/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImageBackground_H___
#define ___C3D_ImageBackground_H___

#include "Scene/Background/Background.hpp"

namespace castor3d
{
	/**
	*\version
	*	0.11.0
	*\~english
	*\brief
	*	Simple image background.
	*\~french
	*\brief
	*	Simple image de fond.
	*/
	class ImageBackground
		: public SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*/
		C3D_API explicit ImageBackground( Engine & engine
			, Scene & scene );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~ImageBackground();
		C3D_API bool loadImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\copydoc	castor3d::SceneBackground::initialiseDescriptorSet
		*/
		void initialiseDescriptorSet( MatrixUbo const & matrixUbo
			, ModelMatrixUbo const & modelMatrixUbo
			, HdrConfigUbo const & hdrConfigUbo
			, renderer::DescriptorSet & descriptorSet )const override;
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )const override;

	private:
		/**
		*\copydoc	castor3d::SceneBackground::doInitialiseShader
		*/
		renderer::ShaderStageStateArray doInitialiseShader()override;
		/**
		*\copydoc	castor3d::SceneBackground::doInitialiseDescriptorLayout
		*/
		void doInitialiseDescriptorLayout()override;
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( renderer::RenderPass const & renderPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::doCleanup
		*/
		void doCleanup()override;
		/**
		*\copydoc	castor3d::SceneBackground::doUpdate
		*/
		void doUpdate( Camera const & camera )override;

	private:
		renderer::UniformBufferPtr< castor::Point2f > m_sizeUbo;
	};
}

#endif

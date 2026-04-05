/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourBackground_H___
#define ___C3D_ColourBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/StagingTexture.hpp>

namespace c3d
{
	class ColourBackground
		: public SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\param[in] scene
		*	The scene.
		*\param name
		*	The background name.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène.
		*\param name
		*	Le nom du fond.
		*/
		C3D_API ColourBackground( Engine & engine
			, Scene & scene
			, String const & name = String{} );
		/**
		*\copydoc	SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )override;
		/**
		*\copydoc	SceneBackground::accept
		*/
		C3D_API void accept( ConfigurationVisitorBase & visitor )override;
		/**
		*\copydoc	SceneBackground::write
		*/
		C3D_API bool write( String const & tabs
			, Path const & folder
			, StringStream & stream )const override;

		C3D_API static void addParsers( AttributeParsers & result );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline HdrRgbColour const & getColour()const
		{
			return m_colour.value();
		}
		/**@}*/

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )override;
		void doGpuUpdate( GpuUpdater & updater )const override;
		void doUpload( UploadData & uploader )override;
		void doAddPassBindings( crg::FramePass & pass
			, Texture * targetImage
			, uint32_t & index )const override;
		void doAddLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const override;
		void doAddDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, Texture * targetImage
			, uint32_t & index )const override;

	private:
		mutable GroupChangeTracked< HdrRgbColour > m_colour;
		PxBufferBaseUPtr m_buffer;
	};
}

#endif

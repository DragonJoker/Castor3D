/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VolumetricRendering_H___
#define ___C3D_VolumetricRendering_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"

namespace c3d
{
	class VolumetricRendering
		: public OwnedBy< RenderTechnique >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent		The parent technique.
		 *\param[in]	device		The GPU device.
		 *\param[in]	progress	The optional progress bar.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent		La technique parente.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	progress	La barre de progression optionnelle.
		 */
		C3D_API VolumetricRendering( RenderTechnique & parent
			, RenderDevice const & device
			, Texture const & colour
			, ProgressBar * progress );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~VolumetricRendering()noexcept;
		/**
		 *\~english
		 *\brief			CPU side update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			GPU side update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Uploads data to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Uploade les données en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le ... visiteur.
		*/
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		FrustumFroxels const & getFrustumFroxels()const noexcept
		{
			return m_frustumFroxels;
		}

		Texture const & getColour()const noexcept
		{
			return m_colour;
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		Texture const & m_colour;
		FrustumFroxels m_frustumFroxels;
		TextureUPtr m_transmittance;
		TextureUPtr m_inscatter;
	};
}

#endif

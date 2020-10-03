/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoConfig_H___
#define ___C3D_SsaoConfig_H___

#include "SsaoModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct SsaoConfig
	{
		/**
		\author		Sylvain DOREMUS
		\date		05/06/2017
		\~english
		\brief		SsaoConfig loader
		\~english
		\brief		Loader de SsaoConfig
		*/
		class TextWriter
			: public castor::TextWriter< SsaoConfig >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	tabs	The tabulations to put at the beginning of each line.
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	tabs	Les tabulations à mettre à chaque début de ligne.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a SSAO configuration into a text file
			 *\param[in]	config	The object to write.
			 *\param[in]	file	The output file.
			 *\~french
			 *\brief		Ecrit une configuration SSAO dans un fichier texte
			 *\param[in]	config	L'objet à écrire.
			 *\param[in]	file	Le fichier de sortie.
			 */
			C3D_API bool operator()( castor3d::SsaoConfig const & config, castor::TextFile & file )override;
		};
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( castor::String const & name
			, PipelineVisitorBase & visitor );

		//!\~english	The effect activation status.
		//!\~french		Le statut d'activation de l'effet.
		bool enabled{ false };
		//!\~english	The log of the maximum pixel offset.
		//!\~french		Le log de l'offset maximum pour un pixel.
		castor::RangedValue< int32_t > logMaxOffset{ 3u, castor::makeRange( 2, 6 ) };
		int32_t maxMipLevel{ 5 };
		//!\~english	Minimal sample radius to compute AO.
		//!\~french		Rayon minimal d'échantillonnage pour calculer l'AO.
		float minRadius{ 1.0f };
		int32_t variation{ 0 };
		//!\~english	The high quality activation status.
		//!\~french		Le statut d'activation de haute qualité.
		bool highQuality{ false };
		//!\~english	The normals buffer usage status.
		//!\~french		Le statut d'utilisation du tampon de normales.
		bool useNormalsBuffer{ false };
		//!\~english	The effect radius.
		//!\~french		Le rayon de l'effet.
		float radius{ 0.5f };
		//!\~english	The effect bias.
		//!\~french		Le bias de l'effet.
		float bias{ 0.025f };
		//!\~english	The effect intensity.
		//!\~french		L'intensité de l'effet.
		float intensity{ 1.0f };
		//!\~english	The samples count pixel.
		//!\~french		Le nombre d'échantillons par pixel.
		uint32_t numSamples{ 19u };
		//!\~english	The edge sharpness, increase to make depth edges crisper, decrease to reduce flicker.
		//!\~french		La précision des bords, augmenter pour rendre les borodures plus précises, diminuer pour réduire le scintillement.
		float edgeSharpness{ 1.0f };
		//!\~english	The high quality activation status for the blur.
		//!\~french		Le statut d'activation de haute qualité pour le flou.
		bool blurHighQuality{ false };
		//!\~english	Step in 2-pixel intervals since we already blurred against neighbors in the first AO pass.
		//!\~french		Etape en intervalles de 2 pixels, un flou a déjà été appliqué, vis à vis des voisins, dans la première passe d'AO.
		castor::ChangeTracked< castor::RangedValue< uint32_t > > blurStepSize{ { 2u, castor::makeRange( 1u, 60u ) } };
		//!\~english	VkFilter radius in pixels. This will be multiplied by m_blurStepSize.
		//!\~english	Rayon du filtre en pixels. Sera multiplié par m_blurStepSize.
		castor::ChangeTracked< castor::RangedValue< int32_t > > blurRadius{ { 4, castor::makeRange( 1, 6 ) } };
	};
}

#endif

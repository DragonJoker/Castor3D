/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsgiConfig_H___
#define ___C3D_SsgiConfig_H___

#include "SsgiModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct SsgiConfig
	{
		/**
		\author		Sylvain DOREMUS
		\date		05/06/2017
		\~english
		\brief		SsgiConfig loader
		\~english
		\brief		Loader de SsgiConfig
		*/
		class TextWriter
			: public castor::TextWriter< SsgiConfig >
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
			 *\brief		Writes a SSGI configuration into a text file
			 *\param[in]	config	The object to write.
			 *\param[in]	file	The output file.
			 *\~french
			 *\brief		Ecrit une configuration SSGI dans un fichier texte
			 *\param[in]	config	L'objet à écrire.
			 *\param[in]	file	Le fichier de sortie.
			 */
			C3D_API bool operator()( castor3d::SsgiConfig const & config, castor::TextFile & file )override;
		};
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( castor::String const & name
			, RenderTechniqueVisitor & visitor );

		//!\~english	The effect activation status.
		//!\~french		Le statut d'activation de l'effet.
		bool enabled{ false };
		//!\~english	Gaussian blur kernel size.
		//!\~english	Taille du kernel du flou gaussien.
		castor::ChangeTracked< castor::RangedValue< uint32_t > > blurSize{ { 4u, castor::makeRange( 1u, 6u ) } };
	};
}

#endif

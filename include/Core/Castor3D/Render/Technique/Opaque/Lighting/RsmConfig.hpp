/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfig_H___
#define ___C3D_RsmConfig_H___

#include "LightingModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct RsmConfig
	{
		/**
		\author		Sylvain DOREMUS
		\date		05/06/2017
		\~english
		\brief		RsmConfig loader
		\~english
		\brief		Loader de RsmConfig
		*/
		class TextWriter
			: public castor::TextWriter< RsmConfig >
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
			C3D_API bool operator()( castor3d::RsmConfig const & config, castor::TextFile & file )override;
		};
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( castor::String const & name
			, PipelineVisitorBase & visitor );

		static constexpr uint32_t MaxRange = 800u;

		castor::ChangeTracked< float > intensity;
		castor::ChangeTracked< float > maxRadius;
		castor::ChangeTracked< castor::RangedValue< uint32_t > > sampleCount{ castor::makeRangedValue( 100u, 20u, MaxRange ) };
	};
}

#endif

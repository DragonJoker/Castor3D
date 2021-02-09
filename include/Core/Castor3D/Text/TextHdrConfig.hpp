/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfig_H___
#define ___C3D_HdrConfig_H___

#include "ToneMappingModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class HdrConfig
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Skybox loader.
		\~english
		\brief		Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< HdrConfig >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a HdrConfig into a text file.
			 *\param[in]	obj		The HdrConfig to save.
			 *\param[in]	file	The file to write the HdrConfig in.
			 *\~french
			 *\brief		Ecrit une HdrConfig dans un fichier texte.
			 *\param[in]	obj		La HdrConfig.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( HdrConfig const & obj
				, castor::TextFile & file )override;
		};

	public:
		//!\~english	The exposure value.
		//!\~french		La valeur d'exposition.
		float exposure{ 1.0f };
		//!\~english	The gamma correction value.
		//!\~french		La valeur de correction gamma.
		float gamma{ 2.2f };
	};
}

#endif

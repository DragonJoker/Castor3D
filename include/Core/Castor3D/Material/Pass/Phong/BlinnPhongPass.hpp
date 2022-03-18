/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BlinnPhongPass_H___
#define ___C3D_BlinnPhongPass_H___

#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class BlinnPhongPass
		: public PhongPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent			The parent material.
		 *\param[in]	initialFlags	The pass initial flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent			Le matériau parent.
		 *\param[in]	initialFlags	Les flags initiaux de la passe.
		 */
		C3D_API explicit BlinnPhongPass( Material & parent
			, PassFlags initialFlags = PassFlag::eNone );
		/**
		 *\~english
		 *\brief		Constructor, from derived classes.
		 *\param[in]	parent			The parent material.
		 *\param[in]	typeID			The pass real type ID.
		 *\param[in]	initialFlags	The pass initial flags.
		 *\~french
		 *\brief		Constructeur, depuis les classes dérivées.
		 *\param[in]	parent			Le matériau parent.
		 *\param[in]	typeID			L'ID du type réel de la passe.
		 *\param[in]	initialFlags	Les flags initiaux de la passe.
		 */
		C3D_API BlinnPhongPass( Material & parent
			, PassTypeID typeID
			, PassFlags initialFlags = PassFlag::eNone );

		C3D_API static PassSPtr create( Material & parent );
		C3D_API static castor::AttributeParsers createParsers();
		C3D_API static castor::AttributeParsers createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID
			, uint32_t remapSectionID
			, castor::String const & texRemapSectionName
			, uint32_t remapChannelSectionID );
		C3D_API static castor::StrUInt32Map createSections();
		/**
		 *\copydoc		castor3d::Pass::getPassSectionID
		 */
		C3D_API uint32_t getPassSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::getTextureSectionID
		 */
		C3D_API uint32_t getTextureSectionID()const override;

	public:
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;
	};
}

#endif

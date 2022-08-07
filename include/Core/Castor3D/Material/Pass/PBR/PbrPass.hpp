/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrPass_H___
#define ___C3D_PbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class PbrPass
		: public Pass
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
		C3D_API explicit PbrPass( Material & parent
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
		C3D_API PbrPass( Material & parent
			, PassTypeID typeID
			, PassFlags initialFlags = PassFlag::eNone );

		C3D_API static PassSPtr create( Material & parent );
		C3D_API static castor::AttributeParsers createParsers();
		C3D_API static castor::AttributeParsers createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID
			, uint32_t texRemapSectionID
			, castor::String const & texRemapSectionName
			, uint32_t remapChannelSectionID );
		C3D_API static castor::StrUInt32Map createSections();
		/**
		 *\copydoc		castor3d::Pass::fillBuffer
		 */
		C3D_API void fillBuffer( PassBuffer & buffer
			, uint16_t passTypeIndex )const override;
		/**
		 *\copydoc		castor3d::Pass::getPassSectionID
		 */
		C3D_API uint32_t getPassSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::getTextureSectionID
		 */
		C3D_API uint32_t getTextureSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::writeText
		 */
		C3D_API bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::RgbColour const & getAlbedo()const
		{
			return *m_albedo;
		}

		float getRoughness()const
		{
			return m_roughness;
		}

		float getGlossiness()const
		{
			return 1.0f - m_roughness;
		}

		float const & getMetalness()const
		{
			return *m_metalness;
		}

		castor::RgbColour const & getSpecular()const
		{
			return *m_specular;
		}
		/**
		 *\copydoc		castor3d::Pass::getColour
		 */
		castor::RgbColour const & getColour()const override
		{
			return getAlbedo();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setAlbedo( castor::RgbColour const & value )
		{
			m_albedo = value;
		}

		void setRoughness( float value )
		{
			m_roughness = value;
		}

		void setGlossiness( float value )
		{
			m_roughness = 1.0f - value;
		}

		void setMetalness( float value )
		{
			m_metalness = value;
			m_metalnessSet = true;
		}

		void setSpecular( castor::RgbColour const & value )
		{
			m_specular = value;
			m_specularSet = true;
		}
		/**
		 *\copydoc		castor3d::Pass::setColour
		 */
		void setColour( castor::RgbColour const & value ) override
		{
			setAlbedo( value );
		}
		/**@}*/

	public:
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;

	protected:
		C3D_API void doAccept( PassVisitorBase & vis )override;
		C3D_API void doAccept( TextureConfiguration & config
			, PassVisitorBase & vis )override;

	private:
		C3D_API void doPrepareTextures( TextureUnitPtrArray & result )override;
		C3D_API PassSPtr doClone( Material & material )const override;
		C3D_API void doJoinMtlRgh( TextureUnitPtrArray & result );
		C3D_API void doJoinSpcGls( TextureUnitPtrArray & result );

	private:
		castor::GroupChangeTracked< castor::RgbColour > m_albedo;
		castor::GroupChangeTracked< float > m_roughness;
		castor::GroupChangeTracked< float > m_metalness;
		castor::GroupChangeTracked< castor::RgbColour > m_specular;

	protected:
		bool m_specularSet{ false };
		bool m_metalnessSet{ false };
	};
}

#endif

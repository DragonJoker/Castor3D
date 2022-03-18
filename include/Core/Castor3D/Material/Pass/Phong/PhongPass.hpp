/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongPass_H___
#define ___C3D_PhongPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class PhongPass
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
		C3D_API explicit PhongPass( Material & parent
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
		C3D_API PhongPass( Material & parent
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
		C3D_API void fillBuffer( PassBuffer & buffer )const override;
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
		 *\copydoc		castor3d::Pass::setColour
		 */
		void setColour( castor::RgbColour const & value ) override
		{
			setDiffuse( value );
		}
		/**
		 *\copydoc		castor3d::Pass::getColour
		 */
		castor::RgbColour const & getColour()const override
		{
			return getDiffuse();
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setDiffuse( castor::RgbColour const & value )
		{
			m_diffuse = value;
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setSpecular( castor::RgbColour const & value )
		{
			m_specular = value;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'ambiante.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setAmbient( float const & value )
		{
			m_ambient = value;
		}
		/**
		 *\~english
		 *\brief		Sets the shininess.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setShininess( float value )
		{
			*m_shininess = value;
		}
		/**
		 *\~english
		 *\return		The shininess value.
		 *\~french
		 *\return		La valeur d'exposant.
		 */
		castor::RangedValue< float > const & getShininess()const
		{
			return *m_shininess;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		castor::RgbColour const & getDiffuse()const
		{
			return *m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		castor::RgbColour const & getSpecular()const
		{
			return *m_specular;
		}
		/**
		 *\~english
		 *\return		The ambient factor.
		 *\~french
		 *\return		Le facteur d'ambiante.
		 */
		float getAmbient()const
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		castor::RgbColour & getDiffuse()
		{
			return *m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		castor::RgbColour & getSpecular()
		{
			return *m_specular;
		}

	protected:
		C3D_API void doAccept( PassVisitorBase & vis )override;
		C3D_API void doAccept( TextureConfiguration & config
			, PassVisitorBase & vis )override;

	private:
		C3D_API void doPrepareTextures( TextureUnitPtrArray & result )override;
		C3D_API void doJoinSpcShn( TextureUnitPtrArray & result );

	public:
		static constexpr float MaxShininess = 256.0f;
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		castor::GroupChangeTracked< castor::RgbColour > m_diffuse;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		castor::GroupChangeTracked< castor::RgbColour > m_specular;
		//!\~english	The ambient contribution factor.
		//!\~french		Le facteur de contribution a l'ambiente.
		castor::GroupChangeTracked< float > m_ambient;
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		castor::GroupChangeTracked< castor::RangedValue< float > > m_shininess;
	};
}

#endif

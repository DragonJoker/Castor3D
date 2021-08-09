/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetallicRoughnessPbrPass_H___
#define ___C3D_MetallicRoughnessPbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class MetallicRoughnessPbrPass
		: public Pass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	parent	Le matériau parent.
		 */
		C3D_API explicit MetallicRoughnessPbrPass( Material & parent
			, PassFlags initialFlags = PassFlag::eNone );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	parent	Le matériau parent.
		 */
		C3D_API MetallicRoughnessPbrPass( Material & parent
			, PassTypeID typeID
			, PassFlags initialFlags = PassFlag::eNone );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MetallicRoughnessPbrPass();

		C3D_API static PassSPtr create( Material & parent );
		C3D_API static castor::AttributeParsers createParsers();
		C3D_API static castor::AttributeParsers createParsers( uint32_t mtlSectionID
			, uint32_t texSectionID );
		C3D_API static castor::StrUInt32Map createSections();
		/**
		 *\copydoc		castor3d::Pass::accept
		 */
		C3D_API void accept( PassBuffer & buffer )const override;
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
			setAlbedo( value );
		}
		/**
		 *\copydoc		castor3d::Pass::getColour
		 */
		castor::RgbColour const & getColour()const override
		{
			return getAlbedo();
		}
		/**
		 *\~english
		 *\brief		Sets the albedo colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur d'albédo.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setAlbedo( castor::RgbColour const & value )
		{
			m_albedo = value;
		}
		/**
		 *\~english
		 *\brief		Sets the roughness.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la rugosité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setRoughness( float value )
		{
			m_roughness = value;
		}
		/**
		 *\~english
		 *\brief		Sets the reflectance.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la réflectivité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void setMetallic( float value )
		{
			m_metalness = value;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		castor::RgbColour const & getAlbedo()const
		{
			return *m_albedo;
		}
		/**
		 *\~english
		 *\return		The roughness.
		 *\~french
		 *\return		La rugosité.
		 */
		float getRoughness()const
		{
			return m_roughness;
		}
		/**
		 *\~english
		 *\return		The reflectance.
		 *\~french
		 *\return		La réflectivité.
		 */
		float const & getMetallic()const
		{
			return *m_metalness;
		}

	public:
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;

	protected:
		C3D_API void doAccept( PassVisitorBase & vis )override;
		C3D_API void doAccept( TextureConfiguration & config
			, PassVisitorBase & vis )override;

	private:
		C3D_API void doInitialise()override;
		C3D_API void doCleanup()override;
		C3D_API void doSetOpacity( float value )override;
		C3D_API void doPrepareTextures( TextureUnitPtrArray & result )override;
		C3D_API void doJoinMtlRgh( TextureUnitPtrArray & result );

	private:
		//!\~english	The albedo colour.
		//!\~french		La couleur d'albédo.
		castor::GroupChangeTracked< castor::RgbColour > m_albedo;
		//!\~english	The roughness.
		//!\~french		La rugosité.
		castor::GroupChangeTracked< float > m_roughness;
		//!\~english	The reflectance.
		//!\~french		La réflectivité.
		castor::GroupChangeTracked< float > m_metalness;
	};
}

#endif

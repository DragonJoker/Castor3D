/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialModule_H___
#define ___C3D_MaterialModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{

	/**
	*\~english
	*\brief
	*	Supported material types enumeration.
	*\~french
	*\brief
	*	Enumération des types de matétiaux supportés.
	*/
	enum class MaterialType
	{
		//!\~english	Phong (non PBR).
		//!\~french		Phong (non PBR).
		ePhong,
		//!\~english	Metallic/Roughness PBR.
		//!\~french		PBR Metallic/Roughness.
		eMetallicRoughness,
		//!\~english	Specular/Glossiness PBR.
		//!\~french		PBR Specular/Glossiness.
		eSpecularGlossiness,
		CU_ScopedEnumBounds( ePhong )
	};
	castor::String getName( MaterialType value );
	/**
	*\~english
	*\brief
	*	Definition of a material
	*\remarks
	*	A material is composed of one or more passes.
	*\~french
	*\brief
	*	Définition d'un matériau
	*\remarks
	*	Un matériau est composé d'une ou plusieurs passes
	*/
	class Material;

	CU_DeclareSmartPtr( Material );

	//! Material pointer array
	CU_DeclareVector( MaterialSPtr, MaterialPtr );
	//! Material pointer map, sorted by name
	CU_DeclareMap( castor::String, MaterialSPtr, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialSPtr, MaterialPtrUInt );

	using OnMaterialChangedFunction = std::function< void( Material const & ) >;
	using OnMaterialChanged = castor::Signal< OnMaterialChangedFunction >;
	using OnMaterialChangedConnection = OnMaterialChanged::connection;

	C3D_API VkFormat convert( castor::PixelFormat format );
	C3D_API castor::PixelFormat convert( VkFormat format );

	//@}
}

#endif

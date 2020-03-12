/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleModule_H___
#define ___C3D_ParticleModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Particle */
	//@{

	/**
	*\~english
	*\brief
	*	Element usage enumeration
	*\~french
	*\brief
	*	Enumération des utilisations d'éléments de tampon
	*/
	enum class ElementUsage
		: uint32_t
	{
		//! Position coords
		eUnknown = 0x000,
		//! Position coords
		ePosition = 0x001,
		//! Normal coords
		eNormal = 0x002,
		//! Tangent coords
		eTangent = 0x004,
		//! Bitangent coords
		eBitangent = 0x008,
		//! Diffuse colour
		eDiffuse = 0x010,
		//! Texture coordinates
		eTexCoords = 0x020,
		//! Bone IDs 0
		eBoneIds0 = 0x040,
		//! Bone IDs 1
		eBoneIds1 = 0x080,
		//! Bone weights 0
		eBoneWeights0 = 0x100,
		//! Bone weights 1
		eBoneWeights1 = 0x200,
		//! Instantiation matrix
		eTransform = 0x400,
		//! Instantiation material index
		eMatIndex = 0x800,
	};
	CU_ImplementFlags( ElementUsage )
	C3D_API castor::String getName( ElementUsage value );
	enum class ParticleFormat
	{
		eInt,
		eVec2i,
		eVec3i,
		eVec4i,
		eUInt,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eFloat,
		eVec2f,
		eVec3f,
		eVec4f,
		eMat2f,
		eMat3f,
		eMat4f,
	};
	C3D_API castor::String getName( ParticleFormat value );
	/**
	*\~english
	*\brief
	*	Compute shader Particle system implementation.
	*\~french
	*\brief
	*	Implémentation d'un système de particules utilisant un compute shader.
	*/
	class ComputeParticleSystem;
	/**
	*\~english
	*\brief
	*	CPU only Particle system implementation.
	*\~french
	*\brief
	*	Implémentation d'un système de particules CPU.
	*/
	class CpuParticleSystem;
	/**
	*\~english
	*\brief
	*	Holds one particle data.
	*\~french
	*\brief
	*	Contient les données d'une particule.
	*/
	class Particle;
	/**
	*\~english
	*\brief
	*	Description of all elements in a vertex buffer
	*\~french
	*\brief
	*	Description de tous les éléments dans un tampon de sommets
	*/
	class ParticleDeclaration;
	/**
	*\~english
	*\brief
	*	Particle element description.
	*\remarks
	*	Describes usage and type of an element in a particle.
	*\~french
	*\brief
	*	Description d'un élément de particule.
	*\remarks
	*	Décrit l'utilisation et le type d'un élément de particule.
	*/
	struct ParticleElementDeclaration;
	/**
	*\~english
	*\brief
	*	Emits the particles.
	*\remarks
	*	Emits one particle at a time.
	*\~french
	*\brief
	*	Emet les particules
	*\remarks
	*	Emet une particle à la fois.
	*\remarks
	*	Décrit l'utilisation et le type d'un élément de particule.
	*/
	class ParticleEmitter;
	/**
	*\~english
	*\brief
	*	Particle system implementation.
	*\~french
	*\brief
	*	Implémentation d'un système de particules.
	*/
	class ParticleSystem;
	/**
	*\~english
	*\brief
	*	Particle system implementation base class.
	*\~french
	*\brief
	*	Classe de base de l'implémentation d'un système de particules.
	*/
	class ParticleSystemImpl;
	/**
	*\~english
	*\brief
	*	Updates the particles.
	*\remarks
	*	Met à jour une particule à la fois
	*\~french
	*\brief
	*	Met à jour les particules.
	*\remarks
	*	Met à jour une particule à la fois.
	*/
	class ParticleUpdater;
	/**
	*\~english
	*\brief
	*	Particle system factory.
	*\~french
	*\brief
	*	Fabrique de système de particules.
	*/
	using ParticleFactory = castor::Factory< CpuParticleSystem
		, castor::String
		, std::unique_ptr< CpuParticleSystem >
		, std::function< std::unique_ptr< CpuParticleSystem >( ParticleSystem & ) > >;

	template< ParticleFormat Type >
	struct ElementTyper;
	
	CU_DeclareSmartPtr( ComputeParticleSystem );
	CU_DeclareSmartPtr( CpuParticleSystem );
	CU_DeclareSmartPtr( ParticleElementDeclaration );
	CU_DeclareSmartPtr( ParticleEmitter );
	CU_DeclareSmartPtr( ParticleFactory );
	CU_DeclareSmartPtr( ParticleSystem );
	CU_DeclareSmartPtr( ParticleSystemImpl );
	CU_DeclareSmartPtr( ParticleUpdater );

	CU_DeclareVector( Particle, Particle );
	CU_DeclareVector( ParticleEmitterUPtr, ParticleEmitter );
	CU_DeclareVector( ParticleUpdaterUPtr, ParticleUpdater );

	using ParticleValues = ParametersT< size_t >;

	//@}
	//@}
}

#endif

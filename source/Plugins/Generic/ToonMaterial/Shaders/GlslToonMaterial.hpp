/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonMaterialModel_H___
#define ___C3D_GlslToonMaterialModel_H___

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	struct ToonProfile
		: public sdw::StructInstanceHelperT < "C3D_ToonProfile"
			, sdw::type::MemoryLayout::eStd430
			, sdw::FloatField< "edgeWidth" >
			, sdw::FloatField< "depthFactor" >
			, sdw::FloatField< "normalFactor" >
			, sdw::FloatField< "objectFactor" >
			, sdw::Vec4Field< "edgeColour" >
			, sdw::FloatField< "smoothBand" >
			, sdw::FloatField< "pad0" >
			, sdw::FloatField< "pad1" >
			, sdw::FloatField< "pad2" > >
	{
		friend class ToonProfiles;
		ToonProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		static std::string const & getName()
		{
			static std::string const name{ "C3D_ToonProfile" };
			return name;
		}
		
		auto edgeWidth()const { return getMember< "edgeWidth" >(); }
		auto depthFactor()const { return getMember< "depthFactor" >(); }
		auto normalFactor()const { return getMember< "normalFactor" >(); }
		auto objectFactor()const { return getMember< "objectFactor" >(); }
		auto edgeColour()const { return getMember< "edgeColour" >(); }
		auto smoothBand()const { return getMember< "smoothBand" >(); }
	};

	class ToonProfiles
		: public c3d::BufferT< ToonProfile >
	{
	public:
		explicit ToonProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		static castor3d::ShaderBufferUPtr create( castor3d::RenderDevice const & device );
		static void update( castor3d::ShaderBuffer & buffer
			, castor3d::Pass const & pass );
		static c3d::BufferBaseUPtr declare( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set );
	};

	struct ToonPhongLightMaterial
		: public c3d::LightMaterial
	{
		ToonPhongLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , ToonPhongLightMaterial );

		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, sdw::Float const & ambient )override;
		void create( sdw::Vec3 const & vtxColour
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const override;
		sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		sdw::Float getMetalness()const override;
		sdw::Float getRoughness()const override;

		static sdw::Float computeGlossiness( sdw::Float const & shininess );
		static sdw::Float computeShininess( sdw::Float const & glossiness );

		sdw::Float & ambient;
		sdw::Float & shininess;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;
		sdw::Vec4 edgeColour;
		sdw::Float smoothBand;

	private:
		void doBlendWith( c3d::LightMaterial const & material
			, sdw::Float const & weight )override;
	};

	Writer_Parameter( ToonPhongLightMaterial );

	struct ToonPbrLightMaterial
		: public c3d::LightMaterial
	{
		ToonPbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , ToonPbrLightMaterial );

		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & spcMtl
			, sdw::Vec4 const & colRgh
			, sdw::Float const & ambient )override;
		void create( sdw::Vec3 const & vtxColour
			, c3d::Materials const & materials
			, c3d::Material const & material )override;
		void output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const override;
		sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const override;
		void adjustDirectSpecular( sdw::Vec3 & directSpecular )const override;
		sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const override;
		sdw::Float getMetalness()const override;
		sdw::Float getRoughness()const override;

		sdw::Float & roughness;
		sdw::Float & metalness;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;
		sdw::Vec4 edgeColour;
		sdw::Float smoothBand;

	private:
		void doBlendWith( c3d::LightMaterial const & material
			, sdw::Float const & weight )override;
	};

	Writer_Parameter( ToonPbrLightMaterial );
}

#endif

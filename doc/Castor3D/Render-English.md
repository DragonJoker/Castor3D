# Render explanations

## General

The rendering in Castor3D is splitted in several passes:
- Environment passes
- Shadow passes
- Deferred rendering for opaque objects
- Background pass
- Weighted Blended rendering for transparent objects

![render](render.png)

## Environment pass

This pass generates the environment map for one node that needs it.

Note that multiple nodes can need a different environment map, thus trigger multiple environment passes.

It is splitted in 3 specific passes:
- Opaque pass (using forward rendering)
- Background pass
- Transparent pass (using forward rendering)

Its result is stored into a cube map.

![environment_pass](environment_pass.png)


## Shadow pass

This pass generates shadow map for one light source.

The engine currently supports three light source type, with their specific shadow passes:
- Directional: This pass generates cascaded shadow maps (with a maximum of 4 cascades)
- Point: This pass renders to a cube map, and is actually splitted in 6 passes (one per cube face)
- Spot: This pass renders to a single 2D texture

That being said, it is also important to note that point and spot shadow maps are stored in texture arrays (Cube an 2D, respectively).

The cascades of the directional shadow map are also stored in a 2D texture array.

It allows to limit the number of texture bindings, especially for passes using forward rendering.

Since Castor3D allows the user to choose the shadows type he wants (Raw, PCF or VSM), the results are always stored in textures using the R32G32_SFLOAT pixel format.

![shadow_pass](shadow_pass.png)


## Deferred rendering

This pass is the most complex one, it contains multiple subpasses:
- Opaque pass
- SSAO pass
- Subsurface Scattering pass
- Lighting pass
- Reflection pass

![deferred_rendering](deferred_rendering.png)

### Opaque pass

This pass is the geometry pass, generating the buffers needed for the other subpasses.

Castor3D uses currently 5 (+1) buffers:
- **>Depth buffer**
  Uses D24_UNORM_S8_UINT pixel format.
- **Data 1 buffer**
  Uses R32G32B32A32_SFLOAT pixel format.
  The RGB channels store the world space normal.
  The A channel stores object material flags.
- **Data 2 buffer**
  Uses R16G16B16A16_SFLOAT pixel format.
  The RGB channels store the diffuse/albedo colour.
  The A channel is used to store:
    - Shininess in Phong
    - Glossiness in PBR Specular/Glossiness
    - Unused in PBR Metallic/Roughness
- **Data 3 buffer**
  Uses R16G16B16A16_SFLOAT pixel format
  The RGB channels store the specular colour, in pipelines that need it.
  For PBR Metallic/Roughness, R stores the Metallic value, G stores the Roughness value, B is unused.
  The A channel stores the Ambient Occlusion value.
- **Data 4 buffer**
  Uses R16G16B16A16_SFLOAT pixel format
  The RGB channels store the emissive colour.
  The A channel stores the transmittance value.
- **Data 5 buffer**
  Uses R16G16B16A16_SFLOAT pixel format
  The RG channels store the velocity value (computed with previous frame).
  The B channel stores the material index.
  The A channel is unused.

### SSAO pass

This pass implements the Scalable Ambient Obscurance algorithm described here: https://research.nvidia.com/sites/default/files/pubs/2012-06_Scalable-Ambient-Obscurance/McGuire12SAO.pdf

It is thus splitted in three subpasses:
- Linearise depth pass
- Raw SSAO pass
- Blur pass

### Lighting pass

This pass is actually a group of lighting subpasses, one subpass per light source.

If the light source generates shadows, its shadow map will be used, to produce shadows of the type chosen by the user.

The point and spot light passes first run a stencil pass, to reduce their effect area.

### Subsurface Scattering pass

This pass implements the Screen-Space Subsurface Scattering algorithm described here : http://www.iryoku.com/sssss/

It is thus splitted in four passes:
- Three successive blur passes
- A combination pass

### Reflection pass

This pass combines the results of all the previous passes:
- It applies the ambient factor with provided ambient occlusion.
- It applies the diffuse and specular results from lighting pass.
- It also applies reflection and/or refraction, using the skybox or the provided environment maps.
- For PBR pipelines, it also applies the IBL, if no reflection is asked on the processed fragment.
- If the scene needs some fog, it is applied here.

## Background pass

This pass is the most simple one, it just draws the scene's skybox.

It is performed after the opaque objects pass to have the benefits of a depth test, after the depth buffer is filled by the opaque pass.


## Weighted Blended rendering

This pass implements the blended weighted rendering described here: http://jcgt.org/published/0002/02/09/paper.pdf

It is thus splitted in two passes:
- Accumulation pass
- Resolve pass

![weighted_blended_rendering](weighted_blended_rendering.png)


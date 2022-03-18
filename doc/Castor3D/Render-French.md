# Explications du rendu

## Général

Le rendu dans Castor3D est découpé en plusieurs passes.
- Passes d'environment
- Passes d'ombres
- Deferred rendering pour les objets opaques
- Passe de background
- Weighted Blended rendering pour les objets transparents

![render](render.png)

## Passe d'environment

Cette passe génère la texture d'environnement pour un noeud en ayant besoin.

Notez que plusieurs noeuds peuvent avoir besoin de textures d'environnement différentes, générant ainsi plusieurs passes d'environnement.

Elle est divisée en 3 passes distinctes:
- Passe opaque (utilisant le forward rendering)
- Passe de background
- Passe transparente (utilisant le forward rendering)

Son résultat est stocké dans une cube map.

![environment_pass](environment_pass.png)

## Passe d'ombres

Cette passe génère la texture d'ombres pour une source lumineuse.

Le moteur supporte pour l'instant trois types de sources lumineuses, ayant leur passe d'ombres spécifique :
- Directionelle: Cette passe génère des cascaded shadow maps (avec un maximum de 4 cascades)
- Omnidirectionnelle: Cette passe dessine dans une cube map, et est donc divisée en 6 passes (une par face du cube)
- Projecteur: Cette passe dessine dans une simple texture 2D

Cela dit, il est important de noter que les shadow maps omnidirectionnelle et projecteur sont stockées dans des texture arrays (respectivement Cube et 2D).

Les cascades de la shadow map directionnelle sont elles aussi stockées dans un texture 2D array.

Cela permet de limiter le nombre de bindings, notamment lors des passes utilisant le forward rendering.

Comme Castor3D permet à l'utilisateur de choisir le type d'ombres qu'il souhaite (Raw, PCF or VSM), Les résultats sont toujours stockés dans des textures au format R32G32_SFLOAT.

![shadow_pass](shadow_pass.png)

## Deferred rendering

Cette passe de rendu est la plus complexe, et est divisée en de multiples passes:
- Passe opaque
- Passe SSAO
- Passe de Subsurface Scattering
- Passe d'éclairage
- Passe de réflexions

![deferred_rendering](deferred_rendering.png)

### Passe opaque

Cette passe est la geometry pass, et génère les buffers dont les autres passes ont besoin.

Castor3D utilise actuellement 5 (+1) buffers :
- **Depth buffer**
  Utilise le format D24_UNORM_S8_UINT.
- **Data 1 buffer**
  Utilise le format R32G32B32A32_SFLOAT.
  Les canaux RGB stockent la normale en coordonnées monde.
  Le canal A stocke les flags de matériau de l'objet.
- **Data 2 buffer**
  Utilise le format R16G16B16A16_SFLOAT.
  Les canaux RGB stockent la couleur diffuse/albedo.
  Le canal A est utilisé pour stocker:
  - La Shininess en Phong
  - La Glossiness en PBR Specular/Glossiness
  - Inutilisé en PBR Metallic/Roughness
- **Data 3 buffer**
  Utilise le format R16G16B16A16_SFLOAT.
  Les canaux RGB stockent la couleur spéculaire, pour les pipelines en ayant besoin.
  Pour le PBR Metallic/Roughness, R stocke la valeur Metallic, G stocke la valeur de Roughness, B est inutilisé.
  Le canal A stocke la valeur d'Ambient Occlusion.
- **Data 4 buffer**
  Utilise le format R16G16B16A16_SFLOAT.
  Les canaux RGB stockent la couleur émissive.
  Le canal A stocke la valeur de transmittance.
- **Data 5 buffer**
  Utilise le format R16G16B16A16_SFLOAT.
  Les canaux RG stockent la valeur de vélocité (calculée avec la frame précédente).
  Le canal B stocke l'indice du matériau.
  Le canal A est inutilisé.

### Passe SSAO

Cette passe implémente l'algorithme Scalable Ambient Obscurance décrit ici : https://research.nvidia.com/sites/default/files/pubs/2012-06_Scalable-Ambient-Obscurance/McGuire12SAO.pdf

Elle est donc scindée en 3 sous-passes :
- Passe de linéarisation de la profondeur
- Pass de SSAO brut
- Pass de flou

### Passe d'éclairage

Cette passe est en fait un ensemble de sous-passes d'éclairage, une par source lumineuse.

Si la source lumineuse génère des ombres, sa shadow map sera utilisée, pour produire les ombres choisies par l'utilisateur.

Les sources omnidirectionnelles et projecteurs effectuent d'abord une passe de stencil, afin de déterminer leur zone d'application.

### Passe Subsurface Scattering

Cette passe implémente l'algorithme de Screen-Space Subsurface Scattering décrit ici : http://www.iryoku.com/sssss/

Elle est donc scindée en 4 sous-passes :
- Trois passes de blur successives
- Une passe de combinaison

### Passe de réflexions

Cette passe combine les résultats des passes précédentes :
- Elle applique le facteur d'ambiant avec l'occlusion ambiante fournie.
- Elle applique les résultats diffus et spéculaire de la passe d'éclairage.
- Elle applique aussi la réflexion et/ou la réfraction, en utilisant la skybox ou les environment maps fournies.
- Pour les pipelines PBR, elle applique aussi l'IBL, si aucune réflexion n'est demandée pour le fragment traité.
- Si la scène a besoin de brouillard, il est appliqué ici.

## Passe de background

Cette passe est la plus simple, il s'agit juste de dessiner la skybox de la scène.

Elle est appliquée après la passe de rendu des objets opaques afin de profiter du depth test, avec le depth buffer rempli par la passe opaque.

## Weighted Blended rendering

Cette passe implémente le blended weighted rendering décrit ici : http://jcgt.org/published/0002/02/09/paper.pdf

Elle est donc scindée en deux passes :
- Accumulation pass
- Resolve pass

![weighted_blended_rendering](weighted_blended_rendering.png)

/**
 * Real-time Realistic Ocean Lighting using Seamless Transitions from Geometry to BRDF
 * Copyright (c) 2009 INRIA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Author: Eric Bruneton
 */

uniform mat4 screenToCamera; // screen space to camera space
uniform mat4 cameraToWorld; // camera space to world space
uniform mat4 worldToScreen; // world space to screen space
uniform mat2 worldToWind; // world space to wind space
uniform mat2 windToWorld; // wind space to world space
uniform vec3 worldCamera; // camera position in world space

uniform float nbWaves; // number of waves
uniform sampler1D wavesSampler; // waves parameters (h, omega, kx, ky) in wind space
uniform float heightOffset; // so that surface height is centered around z = 0
uniform vec2 sigmaSqTotal; // total x and y variance in wind space
uniform float time; // current time

// grid cell size in pixels, angle under which a grid cell is seen,
// and parameters of the geometric series used for wavelengths
uniform vec4 lods;

uniform float nyquistMin; // Nmin parameter
uniform float nyquistMax; // Nmax parameter

attribute vec4 vertex;

varying  float lod;
varying  vec2 u; // coordinates in wind space used to compute P(u)
varying  vec3 P; // wave point P(u) in world space
varying  vec3 _dPdu; // dPdu in wind space, used to compute N
varying  vec3 _dPdv; // dPdv in wind space, used to compute N
varying  vec2 _sigmaSq; // variance of unresolved waves in wind space

void main() {
    gl_Position = vertex;

    vec3 cameraDir = normalize((screenToCamera * vertex).xyz);
    vec3 worldDir = (cameraToWorld * vec4(cameraDir, 0.0)).xyz;
    float t = (heightOffset - worldCamera.z) / worldDir.z;

    u = worldToWind * (worldCamera.xy + t * worldDir.xy);
    vec3 dPdu = vec3(1.0, 0.0, 0.0);
    vec3 dPdv = vec3(0.0, 1.0, 0.0);
    vec2 sigmaSq = sigmaSqTotal;

    lod = - t / worldDir.z * lods.y; // size in meters of one grid cell, projected on the sea surface

    vec3 dP = vec3(0.0, 0.0, heightOffset);
    float iMin = max(0.0, floor((log2(nyquistMin * lod) - lods.z) * lods.w));
    for (float i = iMin; i < nbWaves; i += 1.0) {
        vec4 wt = texture1DLod(wavesSampler, (i + 0.5) / nbWaves, 0.0);
        float phase = wt.y * time - dot(wt.zw, u);
        float s = sin(phase);
        float c = cos(phase);
        float overk = g / (wt.y * wt.y);

        float wp = smoothstep(nyquistMin, nyquistMax, (2.0 * M_PI) * overk / lod);

        vec3 factor = wp * wt.x * vec3(wt.zw * overk, 1.0);
        dP += factor * vec3(s, s, c);

        vec3 dPd = factor * vec3(c, c, -s);
        dPdu -= dPd * wt.z;
        dPdv -= dPd * wt.w;

        wt.zw *= overk;
        float kh = wt.x / overk;
        sigmaSq -= vec2(wt.z * wt.z, wt.w * wt.w) * (1.0 - sqrt(1.0 - kh * kh));
    }

    P = vec3(windToWorld * (u + dP.xy), dP.z);

    if (t > 0.0) {
        gl_Position = worldToScreen * vec4(P, 1.0);
    }

    _dPdu = dPdu;
    _dPdv = dPdv;
    _sigmaSq = sigmaSq;
}

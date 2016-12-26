# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8-80 compliant>

#
# Cycles deferred code.
#

VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST = 10

forwardGeneralDefineGLSL = """#define VKTS_GAMMA 2.2
#define VKTS_INV_GAMMA (1.0/VKTS_GAMMA)

#define VKTS_MAX_LIGHTS 16

#define VKTS_PI 3.14159265

#define VKTS_ONE_OVER_PI (1.0 / VKTS_PI)

#define VKTS_NORMAL_VALID_BIAS 0.1"""

forwardGeneralBufferGLSL = """layout (binding = 2, std140) uniform _u_bufferLights {
        vec4 L[VKTS_MAX_LIGHTS];
        vec4 color[VKTS_MAX_LIGHTS];
        int count;
} u_bufferLights;

layout (binding = 6, std140) uniform _u_bufferMatrices {
        mat4 inverseProjectionMatrix;
        mat4 inverseViewMatrix;
} u_bufferMatrices;

layout(push_constant) uniform _u_bufferParameter {
        int toneMap;
        float exposure;
} u_bufferParameter;"""

forwardGeneralTextureGLSL = """layout (binding = 9) uniform sampler2D u_lut;
layout (binding = 8) uniform samplerCube u_specularCubemap;
layout (binding = 7) uniform samplerCube u_diffuseCubemap;"""

forwardGeneralFunctionsGLSL = """vec3 basicTonemap(vec3 c)
{
    if (u_bufferParameter.toneMap != 0)
    {
        return c * u_bufferParameter.exposure / (1.0 + c / u_bufferParameter.exposure);
    }
    
    return c;
}

vec3 colorToLinear(vec3 c)
{
    return pow(c, vec3(VKTS_GAMMA, VKTS_GAMMA, VKTS_GAMMA));
}

vec3 colorToNonLinear(vec3 c)
{
    return pow(c, vec3(VKTS_INV_GAMMA, VKTS_INV_GAMMA, VKTS_INV_GAMMA));
}

float pow_5(float x)
{
    float x2 = x * x;
    float x4 = x2 * x2;
    
    return x * x4;
}

//

float ndfTrowbridgeReitzGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    
    float alpha2 = alpha * alpha;
    
    float divisor = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
        
    return alpha2 / (VKTS_PI * divisor * divisor); 
}

//

vec3 fresnel(float NdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow_5(1.0 - NdotV);
}

//

float geometricShadowingSchlick(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0f - k) + k);
}

float geometricShadowingSmithSchlickGGX(float NdotL, float NdotV, float roughness)
{
    float k = roughness * roughness * 0.5f;

    return geometricShadowingSchlick(NdotL, k) * geometricShadowingSchlick(NdotV, k);
}

//

vec3 lambert(vec3 L, vec3 lightColor, vec3 N, vec3 baseColor)
{
    return lightColor * baseColor * max(dot(L, N), 0.0) * VKTS_ONE_OVER_PI;
}

vec3 iblLambert(vec3 N, vec3 baseColor)
{
    return baseColor * colorToLinear(texture(u_diffuseCubemap, N).rgb);
}

vec3 cookTorrance(vec3 L, vec3 lightColor, vec3 N, vec3 V, float roughness, vec3 F0)
{
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    
    // Lighted and visible
    if (NdotL >= 0.0 && NdotV >= 0.0)
    {
        vec3 H = normalize(L + V);
        
        float NdotH = dot(N, H);
        float VdotH = dot(V, H);
        
        float D = ndfTrowbridgeReitzGGX(NdotH, roughness);
        vec3 F = fresnel(VdotH, F0);
        float G = geometricShadowingSmithSchlickGGX(NdotL, NdotV, roughness);
        
        vec3 f =  D * F * G / (4.0 * NdotL * NdotV);
        
        return f * lightColor;
    }
    
    return vec3(0.0, 0.0, 0.0);
}

vec3 iblCookTorrance(vec3 N, vec3 V, float roughness, vec3 F0)
{
    // Note: reflect takes incident vector.
    // Note: Use N instead of H for approximation.
    vec3 L = reflect(-V, N);
    
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    
    // Lighted and visible
    if (NdotL > 0.0 && NdotV >= 0.0)
    {
        int levels = textureQueryLevels(u_specularCubemap); 
    
        float scaledRoughness = roughness * float(levels);
        
        float rLow = floor(scaledRoughness);
        float rHigh = ceil(scaledRoughness);    
        float rFraction = scaledRoughness - rLow;
        
        vec3 prefilteredColor = mix(colorToLinear(textureLod(u_specularCubemap, L, rLow).rgb), colorToLinear(textureLod(u_specularCubemap, L, rHigh).rgb), rFraction);

        vec2 envBRDF = texture(u_lut, vec2(NdotV, roughness)).rg;
        
        return prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    }
    
    return vec3(0.0, 0.0, 0.0);
}"""

forwardOutDeclareGLSL = """layout (location = 0) out vec4 ob_fragColor;"""

forwardOutAssignGLSL = """
    vec3 color = vec3(0.0, 0.0, 0.0);
    
    float normalLength = length(Normal_0);
    
    if (normalLength > VKTS_NORMAL_VALID_BIAS)
    {
        vec3 N = mat3(u_bufferMatrices.inverseViewMatrix) * normalize(Normal_0);

        vec4 vertex = u_bufferMatrices.inverseProjectionMatrix * (v_f_vertex / v_f_vertex.w);
        
        vec3 V = mat3(u_bufferMatrices.inverseViewMatrix) * -normalize(vertex.xyz);


        float NdotV = dot(N, V);
        
        if (NdotV < 0.0)
        {
            N = -N;
            
            NdotV = dot(N, V);
        }
        
        
        float roughness = Roughness_0;

        float metallic = Metallic_0;
        
        vec3 baseColor = Color_0.rgb;

        vec3 emissiveColor = Emissive_0.rgb;
        
        float ambientOcclusion = AmbientOcclusion_0;
        
        vec3 F0_dielectric = vec3(0.04, 0.04, 0.04);
        vec3 F0_metallic = baseColor.rgb;
        
        //
        // Lambert.
        //

        vec3 colorLambert = vec3(0.0, 0.0, 0.0);

        //
        // Cook Torrance.
        //
                
        vec3 colorCookTorrance = vec3(0.0, 0.0, 0.0);
        
        // Image based lighting.
        
        //
        // Dielectric
        //
        
        vec3 dielectricFresnel = vec3(0.0, 0.0, 0.0);
        vec3 inverseDielectricFresnel = vec3(0.0, 0.0, 0.0);
        
        if (metallic < 1.0)
        {
            dielectricFresnel = fresnel(NdotV, F0_dielectric);
            inverseDielectricFresnel = vec3(1.0, 1.0, 1.0) - dielectricFresnel;
        
            colorLambert += iblLambert(N, baseColor) * ambientOcclusion * inverseDielectricFresnel;
        
            colorCookTorrance += iblCookTorrance(N, V, roughness, F0_dielectric) * dielectricFresnel;
        }

        //
        // Metallic
        //
    
        if (metallic > 0.0)
        {
            colorCookTorrance += iblCookTorrance(N, V, roughness, F0_metallic);
        }
        
        // Dynamic lights.
        
        // Bring vertex to world space.
        vertex = u_bufferMatrices.inverseViewMatrix * vertex; 
        
        for (int i = 0; i < min(u_bufferLights.count, VKTS_MAX_LIGHTS); i++)
        {
            vec3 light;
            
            if (u_bufferLights.L[i].w > 0.0)
            {
                light = normalize((u_bufferLights.L[i] - vertex).xyz);
            }
            else
            {
                light = u_bufferLights.L[i].xyz;
            }
            
            //
            
            if (metallic < 1.0)
            {
                colorLambert += lambert(light, u_bufferLights.color[i].xyz, N, baseColor) * inverseDielectricFresnel;
        
                colorCookTorrance += cookTorrance(light, u_bufferLights.color[i].xyz, N, V, roughness, F0_dielectric) * dielectricFresnel;
            }

            if (metallic > 0.0)
            {
                colorCookTorrance += cookTorrance(light, u_bufferLights.color[i].xyz, N, V, roughness, F0_metallic);
            }
        }                
        
        //
        
        color = colorToNonLinear(basicTonemap(mix(colorLambert, colorCookTorrance, metallic) + emissiveColor));
    }
    else
    {
        discard;
    }
    
    //

	ob_fragColor = vec4(color, 1.0);
"""

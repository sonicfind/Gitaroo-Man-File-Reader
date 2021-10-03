#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord[2];
	vec4 color;
} vs_in;

struct Material
{
	int blendType;
	int shadingType;
	vec4 diffuse;
	vec4 specular;
	int flags;
	int textEnv;
	int uTile;
	int vTile;
};

layout (std140) uniform Materials
{
	Material materials[2];
};
uniform sampler2D textures[2];
uniform int useTexture[2];
uniform int doMulti;

const int MAX_LIGHTS = 4;

struct Light
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	float min;
	float coefficient;
	float max;
};

layout (std140) uniform Lights
{
	int doLights;
	int numLights;
	int useGlobal;
	vec4 globalVertexColor;
	
	vec3 viewPosition;
	vec3 sceneAmbience;
	Light lights[MAX_LIGHTS];
};

vec4 doColor(const int index);
vec4 getBlendColor(const Material material, const vec4 color);
vec4 applyShading(const Material material, vec4 baseColor);

void main()
{	
	//if (gl_FrontFacing)
	//	FragColor = vec4(0, 1, 0, 1);
	//else
	//	FragColor = vec4(1, 0, 0, 1);
	//return;
	vec4 result = doColor(0);

	if (doMulti == 1)
	{
		vec4 col2 = doColor(1);
		FragColor = vec4(result.rgb + col2.rgb, result.a);
	}
	else
		FragColor = result;

	if (FragColor.a < .01)
		discard;
}

vec4 doColor(const int index)
{
	vec4 result = vec4(0);
	if (useTexture[index] == 1)
	{
		vec4 texColor = texture(textures[index], vs_in.texCoord[index]);
		result = getBlendColor(materials[index], texColor);

		if (materials[index].flags > 2)
			result.a *= 2 * texColor.a;
		
		if (materials[index].shadingType == 3)
			result *= getBlendColor(materials[index], vs_in.color);
	}	
	else if (materials[index].shadingType == 3)
		result = getBlendColor(materials[index], globalVertexColor + vs_in.color);
	else if (useGlobal == 1)
		result = getBlendColor(materials[index], globalVertexColor);
	else
		result = getBlendColor(materials[index], result);

	return applyShading(materials[index], result);
};

vec4 getBlendColor(const Material material, const vec4 color)
{
	switch (material.blendType)
	{
	case 0:
		return vec4(color.rgb, 1);
	case 1:
		return vec4(color.rgb, (color.r + color.g + color.b) / 3);
	case 2:
		return vec4(color.rgb, color.r * color.g * color.b);
	case 3:
		vec3 sub = vec3(1.0f) - color.rgb;
		return vec4(sub, 1 - min(material.diffuse.a * sub.r * sub.g * sub.b, 1));
	case 4:
		return vec4(color.rgb * .5, 1);
	case 5:
		return vec4(color.rgb, material.diffuse.a);
	}
}

vec4 applyShading(const Material material, vec4 baseColor)
{
	vec3 result = vec3(1);
	if (material.shadingType != 0 && material.shadingType != 3 && doLights == 1)
	{
		result = sceneAmbience;
		vec3 viewDir = normalize(viewPosition - vs_in.fragPos);
		vec3 diffuse;
		if (material.shadingType < 3)
			diffuse = material.diffuse.rgb;
		else
			diffuse = globalVertexColor.rgb + vs_in.color.rgb;

		for (int i = 0; i < numLights; ++i)
		{
			// diffuse shading
			float diff = max(dot(vs_in.normal, -lights[i].direction), 0.0);
			result += diff * lights[i].diffuse *  diffuse;

			if (material.shadingType == 1 || material.shadingType == 4)
			{
				// specular shading
				vec3 reflectDir = reflect(lights[i].direction, vs_in.normal);
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specular.a);
				//if (material.shadingType == 1)
					//spec *= material.diffuse.a;
				result += spec * lights[i].specular * material.specular.rgb;
			}
		}
	}
	return vec4(result * baseColor.rgb, baseColor.a);
};

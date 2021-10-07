#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 color;
} vs_in;

layout (std140) uniform Material
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

uniform sampler2D texture;
uniform int useTexture;

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
	float globalCoefficient;
	int useGlobal;

	vec4 globalVertexColor;
	vec3 sceneAmbience;
	vec3 camPosition;
	Light lights[MAX_LIGHTS];
};

vec4 getBlendColor(const vec4 color);
vec4 applyShading(const vec4 baseColor);

void main()
{	
	vec4 result;
	if (useTexture == 1)
	{
		vec4 texColor = texture(texture, vs_in.texCoord);
		result = getBlendColor(texColor);

		if (flags > 2)
			result.a *= 2;
		
		if (shadingType == 3)
			result *= getBlendColor(vs_in.color);
	}	
	else if (shadingType == 3)
		result = getBlendColor(globalVertexColor + vs_in.color);
	else if (useGlobal == 1)
		result = getBlendColor(globalVertexColor);
	else
		result = getBlendColor(vec4(0));

	if (result.a < .01)
		discard;

	FragColor = applyShading(result);
}

vec4 getBlendColor(const vec4 color)
{
	switch (blendType)
	{
	case 0:
	case 1:
		return color;
	case 2:
		return vec4(color.rgb, 1);
	case 3:
		return vec4(color.rgb, 1 - diffuse.a * (1 - color.r) * (1 - color.g) * (1 - color.b));
	case 5:
		return vec4(color.rgb, diffuse.a);
	case 4:
		return vec4(color.rgb * .5, 1);
	}
}

vec4 applyShading(const vec4 baseColor)
{
	vec3 result = vec3(1);
	if (shadingType != 0 && shadingType != 3 && doLights == 1)
	{
		result = sceneAmbience;
		vec3 viewDir = normalize(camPosition - vs_in.fragPos);

		for (int i = 0; i < numLights; ++i)
		{
			// diffuse shading
			float diff = min(max(dot(vs_in.normal, -lights[i].direction), lights[i].min), lights[i].max);
			if (shadingType < 3)
				result += globalCoefficient * diff * lights[i].diffuse * diffuse.rgb;
			else
				result += globalCoefficient * diff * lights[i].diffuse * (globalVertexColor.rgb + vs_in.color.rgb);

			if (shadingType == 1 || shadingType == 4)
			{
				// specular shading
				vec3 reflectDir = reflect(lights[i].direction, vs_in.normal);
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular.a);
				result += spec * lights[i].specular * specular.rgb;
			}
		}
	}
	return vec4(result * baseColor.rgb, baseColor.a);
};

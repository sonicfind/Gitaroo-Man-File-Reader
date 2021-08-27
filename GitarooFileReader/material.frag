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
	int blendingType;
	int shadingType;
	vec4 color;
	sampler2D diffuse;
	vec3 specular;
	float shininess;
	int alphaType;
	int alphaMultiplier;
};
uniform Material materials[2];
uniform int doMulti;

struct Light
{
	vec3 ambient; // 0
	vec3 diffuse; // 16
	vec3 specular; // 32

	float constant; // 44 ?
	float linear; // 48 ?
	float quadratic; // 52 ?
};

layout (std140) uniform Lights
{
	Light light;
};

uniform vec3 viewPos;
uniform vec3 lightPosition;

vec4 doStuff(const Material material, vec2 texCoord);
vec4 blend(vec4 baseColor, vec4 algoColor, int blendType, int alphaType);
vec4 blendColor(const Material material);
vec4 applyShading(const Material material, vec4 baseColor);
vec3 applySpecular(const Material material, vec3 lightDir, float attenuation);

void main()
{
	vec4 result = doStuff(materials[0], vs_in.texCoord[0]);
	if (result.a < 0.01)
		discard;

	if (false)//(doMulti == 1)
	{
		vec4 col2 = doStuff(materials[1], vs_in.texCoord[1]);
		FragColor = vec4(result.rgb * col2.a + col2.rgb * (1 - col2.a), result.a);
	}
	else
		FragColor = result;
}

vec4 doStuff(const Material material, vec2 texCoord)
{
	bool shade;
	vec4 result;
	if (material.shadingType == 3 || material.shadingType == 4)
	{
		result = blendColor(material);
		shade = false;//material.shadingType == 4;
	}
	else
	{
		vec4 texColor = texture(material.diffuse, texCoord);
		if (material.shadingType < 3)
			result = blend(texColor, material.color, material.blendingType, material.alphaType);
		else
			result = blend(texColor, vs_in.color, material.blendingType, material.alphaType);
	}

	if (FragColor.a < 0.01)
		discard;
	
	if (shade)
		return applyShading(material, result);
	else
		return result;
};

vec4 blend(vec4 baseColor, vec4 algoColor, int blendType, int alphaType)
{
	vec4 combo = baseColor * algoColor;
	vec4 result = vec4(1);
	switch (blendType)
	{
	case 0:
		result.rgb = combo.rgb;
		if (alphaType == 1)
			result.a = 2 * baseColor.a;
		break;
	case 1:
		result = combo;
		if (alphaType == 1)
			result.a *= 2;
		break;
	case 2:
		result = baseColor;
		break;
	case 3:
		result.r = max(algoColor.r - baseColor.r, 0);
		result.g = max(algoColor.g - baseColor.g, 0);
		result.b = max(algoColor.b - baseColor.b, 0);
		result.a = 1 -  min(algoColor.a * (baseColor.r + baseColor.g + baseColor.b) / 3, 1);
		break;
	case 4:
		result.rgb = combo.rgb * .5;
		break;
	case 5:
		result.rgb = baseColor.rgb;
		result.a = baseColor.a * algoColor.a;
		if (alphaType == 1)
			result.a *= 2;
	}
	return result;
};

vec4 blendColor(const Material material)
{
	vec4 combo = vs_in.color * material.color;
	vec4 result;
	switch (material.blendingType)
	{
	case 0:
		result = vec4(vs_in.color.rgb, 1);
		break;
	case 1:
		result = vec4(vs_in.color.rgb, (vs_in.color.r + vs_in.color.g + vs_in.color.b) / 3);
		break;
	case 2:
		result = vec4(vs_in.color.rgb, (combo.r * combo.g * combo.b));
		break;
	case 3:
		result = material.color - vs_in.color;
		result.a = max(1 - (material.color.a * (FragColor.r + FragColor.g + FragColor.b) / 3), 0);
		break;
	case 4:
		result = vec4(vs_in.color.rgb * material.color.rgb, 1);
		break;
	case 5:
		result = vec4(combo.rgb, 1 - material.color.a);
	}
	return result;
};

vec4 applyShading(const Material material, vec4 baseColor)
{
	float distance = length(lightPosition - vs_in.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = attenuation * light.ambient * vec3(baseColor);
	vec3 lightDir = normalize(lightPosition - vs_in.fragPos);
	float diff = max(dot(vs_in.normal, lightDir), 0.0);
	vec3 diffuse = attenuation * light.diffuse * diff * vec3(baseColor);

	if (material.shadingType != 1)
	{
		vec3 specular = applySpecular(material, lightDir, attenuation);
		return vec4((ambient + diffuse + specular) * baseColor.rgb, baseColor.a);
	}
	else
		return vec4((ambient + diffuse) * baseColor.rgb, baseColor.a);
};

vec3 applySpecular(const Material material, vec3 lightDir, float attenuation)
{
	vec3 viewDir = normalize(viewPos - vs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, vs_in.normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec =  material.color.a * pow(max(dot(vs_in.normal, halfwayDir), 0.0), material.shininess);
	return attenuation * material.specular * spec * light.specular;
};

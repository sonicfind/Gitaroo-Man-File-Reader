#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec4 ourColor;
in vec2 texCoord;

struct Material
{
    vec4 color;
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;
uniform int blendingType;
uniform int shadingType;
uniform int useTexAlpha;


    struct Light
    {
        vec3 position;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };
    uniform Light light;
    uniform vec3 viewPos;


void blendTexture(vec4 baseColor);
void blendColor();
void applyShading();
vec3 applySpecular(vec3 lightDir, float attenuation);

void main()
{
    if (shadingType < 3)
    {
	blendTexture(material.color);
	if (shadingType != 0)
	    applyShading();
    }
    else if (shadingType < 5)
    {
	blendColor();
	if (shadingType == 4)
	    applyShading();
    }
    else
    {
	blendTexture(ourColor);
	if (shadingType == 6)
	    applyShading();
    }
};

void blendTexture(vec4 baseColor)
{
    vec4 texColor = texture(material.diffuse, texCoord);
    vec4 comboColor = texColor * baseColor;
    switch (blendingType)
    {
    case 0:
    case 4:
	FragColor = vec4(comboColor.rgb, 1.0);
	break;
    case 1:
	FragColor = comboColor;
	break;
    case 2:
	FragColor = vec4((texColor.rgb * comboColor.a) * (baseColor.rgb * (1 - comboColor.a)), 1.0);
	break;
    case 3:
	FragColor = vec4(texColor.rgb / baseColor.rgb, 1.0);
	break;
    case 5:
	FragColor = vec4(comboColor.rgb, baseColor.a);
    }

    if (useTexAlpha == 1)
	FragColor.a *= texColor.a;

    if (FragColor.a == 0)
	discard;
};

void blendColor()
{
    switch (blendingType)
    {
    case 0:
    case 4:
	FragColor = vec4(ourColor.rgb, 1.0);
	break;
    default:
	FragColor = ourColor;
    }

    if (FragColor.a < 0.1)
	discard;
};

vec3 applySpecular(vec3 lightDir, float attenuation)
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    return attenuation * material.specular * spec * light.specular;
};

void applyShading()
{
    float distance = length(light.position - fragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float attenuation = 1;

    vec3 ambient = attenuation * light.ambient * vec3(FragColor);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = attenuation * light.diffuse * diff * vec3(FragColor);

    if (shadingType != 1)
    {
	vec3 specular = applySpecular(lightDir, attenuation);
	FragColor = vec4((ambient + diffuse + specular) * FragColor.rgb, FragColor.a);
    }
    else
	FragColor = vec4((ambient + diffuse) * FragColor.rgb, FragColor.a);
};
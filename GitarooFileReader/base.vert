#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;

const int MAX_INSTANCES = 32;

out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 color;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

layout (std140) uniform Projection
{
	mat4 projection;
};

layout (std140) uniform Models
{
	mat4 models[MAX_INSTANCES];
};

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

void main()
{
	mat4 model = models[gl_InstanceID];

	vec4 finalPos = vec4(aPos.xyz, 1);

	// Position in screen space
	gl_Position = projection * view * model * finalPos;

	// Position in world space
	vs_out.fragPos = vec3(model * finalPos);

	// Normalized normal vector
	// Interpretted as if it starts at (0, 0, 0) as translation do not apply
	vs_out.normal = normalize(vec3(model * vec4(aNorm, 0)));
	vs_out.color = aColor;

	if (textEnv == 0)
		vs_out.texCoord = aTexCoord;
	else
	{
		// Spherical texture environment
		vec3 r = vec3(reflect(view * vec4(vs_out.fragPos, 1), view * vec4(vs_out.normal, 0)));
		float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
		vs_out.texCoord = r.xy / m + .5;
	}
}

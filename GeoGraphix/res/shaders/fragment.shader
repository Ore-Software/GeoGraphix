#version 460 core

layout(location = 0) out vec4 color;

in vec3 pos;
in vec4 view_pos;
in vec3 view_pos_normal;
in vec4 light_pos;

void main()
{
	vec3 ka; // ambient constant
	vec3 kd; // diffuse constant
	vec3 ks; // specular constnat
	float p; // phong exponent
	vec3 n = normalize(view_pos_normal);
	vec3 v = normalize(-view_pos.xyz);
	vec3 l = normalize(light_pos.xyz - view_pos.xyz);
	vec3 h = normalize(v + l);

	if (pos.y > 0.5)
	{
		ka = vec3(0.0, 0.125, 0.0);
		kd = vec3(0.0, 0.25, 0.0);
		ks = vec3(0.5, 0.5, 0.5);
		p = 50.0f;
	}
	else
	{
		ka = vec3(0, 0, 0.25);
		kd = vec3(0, 0, 0.5);
		ks = vec3(0.8, 0.8, 0.8);
		p = 500.0f;
	}

	color = vec4(ka * 0.2 + kd * max(0.0, dot(n, l)) + ks * pow(max(0, dot(n, h)), p), 1.0);
};
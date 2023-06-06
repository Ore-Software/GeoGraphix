#version 460 core

layout(location = 0) out vec4 color;

in vec3 pos;
in vec4 view_pos;
in vec3 view_pos_normal;
in vec4 light_pos;

uniform int u_Is_Water;

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
	vec3 I = vec3(1, 1, 1); // light intensity of light

	if (u_Is_Water == 1) // is water
	{

		if (pos.y > 0.23)
		{
			ka = vec3(0.2, 0.2, 0.2);
			kd = vec3(0.8, 0.8, 0.8);
			ks = vec3(0.1, 0.1, 0.1);
			p = 500.0f;
		}
		else
		{
			ka = vec3(0, 0, 0.2);
			kd = vec3(0, 0, 0.8);
			ks = vec3(0.6, 0.6, 0.6);
			p = 5000.0f;
		}
	}
	else // is land
	{
		if (pos.y > 0.9)
		{
			ka = vec3(0.22, 0.235, 0.25);
			kd = vec3(0.88, 0.94, 1.0);
			ks = vec3(0.7, 0.7, 0.7);
			p = 500.0f;
		}
		else if (pos.y > 0.6)
		{
			ka = vec3(0.09, 0.09, 0.09);
			kd = vec3(0.36, 0.36, 0.36);
			ks = vec3(0.2, 0.2, 0.2);
			p = 10.0f;
		}
		else if (pos.y > 0.3)
		{
			ka = vec3(0.09, 0.1325, 0.0625);
			kd = vec3(0.36, 0.53, 0.25);
			ks = vec3(0.6, 0.8, 0.5);
			p = 50.0f;
		}
		else
		{
			ka = vec3(0.25, 0.25, 0.1825);
			kd = vec3(1.0, 0.99, 0.73);
			ks = vec3(0.8, 0.8, 0.6);
			p = 100.0f;
		}
	}
	
	color = vec4(ka * 0.2 + kd * I * max(0.0, dot(n, l)) + ks * I * pow(max(0, dot(n, h)), p), 1.0);
};
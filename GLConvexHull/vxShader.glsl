in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

uniform uint un_UseShading;
uniform mat4 un_MVP;
uniform vec3 un_LightPosition;
uniform vec4 un_LightColor;

out vec4 vx_Color;

void main()
{
	gl_Position = un_MVP*vec4(in_Position, 1.0);

	if (un_UseShading == 1)
	{
		vec3 vx_LightDirection = un_LightPosition - in_Position;
		float vx_LightDistance = length(vx_LightDirection);
		float vx_cos = dot(normalize(vx_LightDirection), in_Normal);

		if (vx_cos > 0.0)
		{
			vx_Color = in_Color*un_LightColor;
			vx_Color *= un_LightColor.a*vx_cos/vx_LightDistance;
		}
		else
			vx_Color.rgb = vec3(0.0);

		vx_Color += in_Color;
	}
	else
		vx_Color = in_Color;
}
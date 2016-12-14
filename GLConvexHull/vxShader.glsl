in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

uniform mat4 un_MVP;
uniform vec3 un_LightPosition;
uniform vec4 un_LightColor;

out vec4 vx_Color;

void main()
{
	gl_Position = un_MVP*vec4(in_Position, 1.0);

	vec4 vx_Normal = un_MVP*vec4(in_Normal, 0.0);
	vec4 vx_LightPosition = un_MVP*vec4(un_LightPosition, 1.0);
	vec4 vx_LightDirection = (vx_LightPosition - gl_Position);

	float vx_LightDistance = length(vx_LightDirection);
	float vx_cos = dot(normalize(vx_LightDirection), vx_Normal);

	if (vx_cos > 0.0)
	{
		vx_Color = vec4(in_Color.r*un_LightColor.r,
						in_Color.g*un_LightColor.g,
						in_Color.b*un_LightColor.b, 0.0);
		vx_Color *= un_LightColor.a*vx_cos/(vx_LightDistance*vx_LightDistance);
		vx_Color.w = in_Color.a;
	}
	else
		vx_Color = in_Color;
}
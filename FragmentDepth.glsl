#version 440

float near = 0.1f;
float far = 100.0f;

out float fragment_color;

void main ()
{
	/*float z = gl_FragCoord.z * 2.0 -1.0;
	float depth = (2 * near * far)/ (far + near - z * (far - near));
	fragment_color = vec4(vec3(depth)/ far, 1.0);*/
	fragment_color = 1;
}
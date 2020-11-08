#version 440
in vec4 color;
in vec2 texCoord;

uniform sampler2D particelTexur;

// this is the final pixel colour
out vec4 fragment_color;

void main () {
	  vec4 textureColor = texture(particelTexur, vec2(texCoord.x, 1-texCoord.y)) * color;
	  // make the transparansy of the texture work
	  if(textureColor.a < 0.1)
	  {
		discard;
	  }
	  fragment_color = textureColor;
}
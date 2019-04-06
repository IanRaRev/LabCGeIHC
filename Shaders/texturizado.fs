#version 330 core

in vec2 our_tex;
out vec4 color;
uniform sampler2D ourTexture;

void main(){
	color = texture(ourTexture, our_tex);

    //La a es la componente alfa. Transparencia
    if (color.a <= 0.1)
        discard;

	//color = vec4(0.3, 0.5, 0.4, 1.0);
}
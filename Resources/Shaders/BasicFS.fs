precision mediump float;
uniform sampler2D material_texture_diffuse1;
varying vec2 TexCoord0;
                   
void main()
{                                                                                                       
	gl_FragColor =  texture2D(material_texture_diffuse1, TexCoord0);
}
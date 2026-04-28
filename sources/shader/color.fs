#version 460 core
out vec4 FragColor;

uniform vec3 lightColor;
uniform vec4 objectColor;
void main(){
    FragColor = (vec4(lightColor,1.0) * objectColor)
}
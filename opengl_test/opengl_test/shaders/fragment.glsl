#version 330 core
/*out vec4 color;
void main()
{
    color = vec4(0.6f, 0.0f, 1.0f, 1.0f);
}*/

out vec4 color;

uniform vec3 objColor;
void main(){
    color = vec4(objColor, 1.0);
}

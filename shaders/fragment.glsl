uniform sampler2D texture;
uniform sampler2D palettes;
uniform float palId;
uniform float palMultiplier;
uniform float alpha;
varying mediump vec4 texc;

void main(void)
{
    //if (palMultiplier == 0.0f) {
        gl_FragColor = texture2D(texture, texc.st);
        if (gl_FragColor.a == 0.0) {
            discard;
        } else {
            gl_FragColor.a = alpha;
        }
    /* } else {
        vec4 i = texture2D(texture, texc.st);
        gl_FragColor = texture2D(palettes, vec2(i.r * palMultiplier, palId));
    } */
}

#version 150

uniform sampler2D colorTex;
uniform vec2 resoulotion;


uniform bool isFlipH;
uniform bool isFlipV;
uniform bool isGreyscale;
uniform bool isGaussian;

in vec2 pass_TexCoord;
out vec4 out_Color;


void main(void)
{
    vec2 texCoords = pass_TexCoord;

    if (isFlipV)
    {
        texCoords = vec2(1.0f - texCoords.x, texCoords.y);
    }

    if (isFlipH)
    {
        texCoords = vec2(1.0f-texCoords.x, texCoords.y);;
    }

    vec4 color = vec4(0.0f);
    if (isGaussian)
    {
         vec2 swap_res = vec2(1.0) / resoulotion;
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float d = abs(x) + abs(y);
                float factor = 0.25f * pow(2.0f, -d);
                vec2 offset = vec2(float(x), float(y));
                vec2 texCoord = texCoords + offset * swap_res;
                vec4 cl = texture(colorTex, texCoord);
                color += factor * cl;
            }
        }
    }
    else
    {
        color = texture(colorTex, texCoords);
    }

    if (isGreyscale)
    {
        float luminance = 0.2126f *color.r + 0.7152f*color.g  + 0.0722f * color.b;
		color = vec4(luminance,luminance, luminance, 1.0f);
    }

    out_Color = color;
}

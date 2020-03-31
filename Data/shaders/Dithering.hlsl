/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


// http://alex.vlachos.com/graphics/Alex_Vlachos_Advanced_VR_Rendering_GDC2015.pdf
inline float3 dither(float2 uv)
{
	float2 screen_pos   = (uv * g_resolution);
    float3 dither       = dot(float2(171.0f, 231.0f), screen_pos);
    dither              = frac(dither / float3(103.0f, 71.0f, 97.0f));  
    dither              /= 255.0f;
    
    return dither;
}

// Same as dither but offests over time, when TAA is enabled. Returns regular dither if TAA is not enabled.
inline float3 dither_temporal(float2 uv, float scale = 1.0f)
{
	float2 screen_pos   = (uv * g_resolution) + sin(g_time) * any(g_taa_jitter_offset);	
    float3 dither       = dot(float2(171.0f, 231.0f), screen_pos);
    dither              = frac(dither / float3(103.0f, 71.0f, 97.0f));  
    dither              /= 255.0f;
    
    return dither * scale;
}

// Same as dither but offests over time, when TAA is enabled. Returns fallback if TAA is not enabled.
inline float3 dither_temporal_fallback(float2 uv, float fallback, float scale = 1.0f)
{
    return any(g_taa_jitter_offset) ? dither_temporal(uv, scale) : fallback;
}
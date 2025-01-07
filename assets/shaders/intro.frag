#version 450

#include "defines/fragdef.h"
#include "defines/defines.h"

// layout (location = 0) out vec4 outfragcolor;

const int font[] = int[](
 0x69f99, 0x79797, 0xe111e, 0x79997, 0xf171f, 0xf1711, 0xe1d96, 0x99f99, 
 0xf444f, 0x88996, 0x95159, 0x1111f, 0x9f999, 0x9bd99, 0x69996, 0x79971,
 0x69b5a, 0x79759, 0xe1687, 0xf4444, 0x99996, 0x999a4, 0x999f9, 0x99699,
 0x99e8e, 0xf843f, 0x6bd96, 0x46444, 0x6942f, 0x69496, 0x99f88, 0xf1687,
 0x61796, 0xf8421, 0x69696, 0x69e84, 0x66400, 0x0faa9, 0x0000f, 0x00600,
 0x0a500, 0x02720, 0x0f0f0, 0x08421, 0x33303, 0x69404, 0x00032, 0x00002,
 0x55000, 0x00000, 0x00202, 0x42224, 0x24442);

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8
#define J 9
#define K 10
#define L 11
#define M 12
#define N 13
#define O 14
#define P 15
#define Q 16
#define R 17
#define S 18
#define T 19
#define U 20
#define V 21
#define W 22
#define X 23
#define Y 24
#define Z 25
#define N_0 26
#define N_1 27
#define N_2 28
#define N_3 29
#define N_4 30
#define N_5 31
#define N_6 32
#define N_7 33
#define N_8 34
#define N_9 35
#define APST 36
#define PI   37
#define UNDS 38
#define HYPH 39
#define TILD 40
#define PLUS 41
#define EQUL 42
#define SLSH 43
#define EXCL 44
#define QUES 45
#define COMM 46
#define FSTP 47
#define QUOT 48 
#define BLNK 49
#define COLN 50
#define LPAR 51
#define RPAR 52

const ivec2 MAP_SIZE = ivec2(4, 5);

int DrawChar(int char, vec2 pos, vec2 size, vec2 uv)
{
    uv -= pos;
    uv /= size;    

    // to bitmap space coordinates
    uv *= vec2(MAP_SIZE);

    // bitmap texel coordinates
    ivec2 iuv = ivec2(round(uv));
    
    if (iuv.x < 0 || iuv.x > MAP_SIZE.x - 1 || iuv.y < 0 || iuv.y > MAP_SIZE.y - 1) {
        return 0;
    }

    int index = MAP_SIZE.x * iuv.y + iuv.x;
    return (font[char] >> index) & 1;
}


int DrawText(vec2 uv, vec2 resolution, const float size, vec2 pos, int text[20], int textlen)
{
    vec2 charsize = vec2(size * vec2(MAP_SIZE)/ resolution.x);
    float spacesize = float(size * float(MAP_SIZE.x + 1)/ resolution.x);

    vec2 charpos = pos;
    int chr = 0;

    for (int i = 0; i < textlen; i++) {
        chr += DrawChar(text[i], charpos, charsize, uv);
        charpos.x += spacesize;
    }
    
    return chr;
}

float randomtime (vec2 st, float time) {
    return fract(sin(dot(st.xy, vec2(11.5621, 78.943)))  * (time / 100000.0) * 43758.5453123);
}
float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(11.5621, 78.943))) * 43758.5453123);
}

float GetWord(float num, vec2 coord, vec2 resolution)
{
    float colorr;
    if (num - 2 * (floor(num / 2)) != 0) {
        int arr2[20] = int[](N_2, N_0, N_2, N_4,R, A, X, A, I, BLNK, E, N, G, I, N, E, COMM, COMM, COMM, COMM);
        colorr = float(DrawText(coord, resolution, 22.2 , vec2(0.45, 0.35 ), arr2, 4));
    }
    else {
        int arr2[20] = int[](W, I, R, E, S, BLNK, I, N, BLNK, H, E, A, R, T, A, E, COMM, COMM, COMM, COMM);
        colorr = float(DrawText(coord, resolution, 12.2, vec2(0.30, 0.35 ), arr2, 14));
    }

  return colorr;
}

void main()
{    
    float time = GetResource(Camera, GetUniformInd()).time;
    vec2 resolution = GetResource(Camera, GetUniformInd()).viewport.xy;
    
    if (incoord.x < 0.12 || incoord.x > 0.88) {
        outfragcolor = vec4(0);
        return;
    }

    vec2 uv = incoord.xy;
    uv *= 50;

// background ---------------------------------------------------------------------

    vec2 ipos = floor(uv); 
    float dissapear = length(cos(vec3(time) + ipos.xyx * vec3(0.35)));

    vec3 col = vec3(random(ipos),random(ipos),random(ipos)) * dissapear * 0.5;
    col = col.r < (0.5) ? col : vec3(1.0, 0,0);
 
// dots ---------------------------------------------------------------------------

    if (incoord.x < 0.25 || incoord.x > 0.75) {
        vec2 dotscoord = floor(incoord.xy * 200.);

        float randomval = randomtime(dotscoord.xy, time);
        vec3 col2 = randomval < 0.2 ? vec3(1.) : vec3(0.);
        col.bg += col2.rg;
    }

    float sizemult = 2 * abs(cos(time * 0.93)) + 1.5;
    // sizemult = clamp(sizemult, 1.0, 4.0);

    sizemult = 2.0;

// introtext -----------------------------------------------------------------------

    float introtext = 0.0;
    vec2 coord = incoord.xy;

    int arr[20] = int[](A, N, T, H, R, A, X, A, I, BLNK, E, N, G, I, N, E, COMM, COMM, COMM, COMM);
    introtext = float(DrawText(coord, resolution, 6.0 * sizemult, vec2(0.26, 0.9), arr, 16));

    int arr2[20] = int[](E, X, P, L, O, R, E, BLNK, M, Y, BLNK, S, A, D, N, E, S, S, COMM, COMM);
    float introsectext = float(DrawText(coord, resolution, 5.6 * sizemult, vec2(0.26, 0.7 ), arr2, 18));

    col.gb += vec2(introtext);
    if(coord.y > 0.62 && coord.y < 0.8 && coord.x > 0.25 && coord.x < 0.75) {
        col.r *= introsectext;
    }

// second intro scene ---------------------------------------------------------------

    float sync = abs(cos(time * 0.3));
    if (sync < 0.5 && sync > 0.0) {

        float nextframetext = GetWord(round(time / 2), coord, resolution.xy);
        if(coord.y > 0.0 && coord.y < 1.0 ) {
            float randomval = randomtime(uv.xy, time);
            col.r = randomval < 0.5 ? col.r : 0;
            col *= vec3(nextframetext);

            uv = incoord.xy * 2.0 - 1.0;

            vec2 rotatedUV = vec2(-uv.y, uv.x);
            rotatedUV *= 3.0;
            vec3 colorline = vec3(0.0);
            float e = 0.0;

            for (float i = 3.0; i <= 4.0; i += 1.0) {
                float curve = 0.007 / abs((i / 15.0) + sin((time / 20.0) - 0.15 * i * (rotatedUV.x) * cos(i / 1.0 + (time / 2.0) + rotatedUV.x * 100.2)) + 1.5 * rotatedUV.y);
                e = max(e, curve);
            }
            colorline += e > 0.5 ? 1 : 0;
            col.rg += vec2(colorline);
        }
        if (incoord.x < 0.25 || incoord.x > 0.75) {
            vec2 dotscoord = floor(incoord.xy * 200.);
            float randomval = randomtime(dotscoord.xy, time);
            vec3 col2 = randomval < 0.2 ? vec3(1.) : vec3(0.);
            col.rg += col2.rg;
        }
    }
    else {
        if (coord.y < 0.55 && coord.y > 0.1 && coord.x > 0.3 && coord.x < 0.7) {
            col = vec3(0);
        }
    }
    outfragcolor = vec4(col, 1);
}

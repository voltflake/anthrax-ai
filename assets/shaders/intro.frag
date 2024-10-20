#version 450

#include "defines/fragdef.h"

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

int DrawChar(in int char, in vec2 pos, in vec2 size, in vec2 uv)
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

int DrawText( in vec2 uv, const float size )
{
    vec2 charsize = vec2(size * vec2(MAP_SIZE)/ 800);
    float spacesize = float(size * float(MAP_SIZE.x + 1)/ 800);
        
    vec2 charpos = vec2(0.05, 0.90);
    int chr = 0;

    chr += DrawChar( T, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( H, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( I, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( S, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( BLNK, charpos, charsize, uv); charpos.x += spacesize;

    chr += DrawChar( I, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( S, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( BLNK, charpos, charsize, uv); charpos.x += spacesize;

    chr += DrawChar( B, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( I, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( T, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( M, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( A, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( P, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( BLNK, charpos, charsize, uv); charpos.x += spacesize;

    chr += DrawChar( T, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( E, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( X, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( T, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( EXCL, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( EXCL, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( EXCL, charpos, charsize, uv); charpos.x += spacesize;
    
    charpos = vec2(0.05, .82);
    chr += DrawChar( I, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( N, charpos, charsize, uv); charpos.x += spacesize;
 
    charpos = vec2(0.05, .75);
    chr += DrawChar( S, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( H, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( A, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( D, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( E, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( R, charpos, charsize, uv); charpos.x += spacesize;
    chr += DrawChar( EXCL, charpos, charsize, uv); charpos.x += spacesize;   
 
    return chr;
}

void main()
{    
    float color = float(DrawText(incoord.xy, 5.5));
    outfragcolor = vec4(color, color, color, 1);
}
//
// 型指定
// 
#pragma once

//
// FIXME:このファイルの存在の是非
// 

#if defined (PI)
#undef PI
#endif
#define PI ((float)3.1415925026e+00)

#define elemsof(a)  ((int)(sizeof(a) / sizeof((a)[0])))

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;


#if defined (TARGET_OS_IPHONE)

typedef float GLdouble;

#define glFrustum(left, right, bottom, top, znear, zfar) glFrustumf(left, right, bottom, top, znear, zfar)
#define glOrtho(left, right, bottom, top, znear, zfar) glOrthof(left, right, bottom, top, znear, zfar)
#define glTranslated(x, y, z) glTranslatef(x, y, z)
#define glGetDoublev(pname, params) glGetFloatv(pname, params)
#define glLightModeli(pname, params) glLightModelx(pname, params)

#define glBegin(mode)
#define glEnd()
#define glVertex2f(x, y)
#define glTexCoord2f(x, y)

#endif

#pragma once

#include <stdio.h>
#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glu.h"

void glVersion()
{
	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  
	const GLubyte* Extensions = glGetString(GL_EXTENSIONS);
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //返回当前GLU工具库版本  
	printf("OpenGL实现厂商名字	：%s\n", name);
	printf("OpenGL渲染器标识符	：%s\n", biaoshifu);
	printf("OpenGL实现的版本号	：%s\n", OpenGLVersion);
	//printf("OpenGL支持的扩展	：%s\n", Extensions);
	printf("OGLU工具库版本		：%s\n", gluVersion);

	if (glewIsSupported("GL_VERSION_2_1"))
		printf("		Ready for OpenGL 2.1\n");
	else {
		printf("		Warning: Detected that OpenGL 2.1 not supported\n");
	}
}
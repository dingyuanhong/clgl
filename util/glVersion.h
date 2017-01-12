#pragma once

#include <stdio.h>
#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glu.h"

void glVersion(FILE *fp)
{
	glewInit();

	const GLubyte* name = glGetString(GL_VENDOR); //���ظ���ǰOpenGLʵ�ֳ��̵�����  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //����һ����Ⱦ����ʶ����ͨ���Ǹ�Ӳ��ƽ̨  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //���ص�ǰOpenGLʵ�ֵİ汾��  
	const GLubyte* Extensions = glGetString(GL_EXTENSIONS);
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //���ص�ǰGLU���߿�汾  
	fprintf(fp, "OpenGLʵ�ֳ�������	��%s\n", name);
	fprintf(fp, "OpenGL��Ⱦ����ʶ��	��%s\n", biaoshifu);
	fprintf(fp, "OpenGLʵ�ֵİ汾��	��%s\n", OpenGLVersion);
	//fprintf(fp,"OpenGL֧�ֵ���չ	��%s\n", Extensions);
	fprintf(fp, "OGLU���߿�汾		��%s\n", gluVersion);

	if (glewIsSupported("GL_VERSION_2_1"))
		fprintf(fp, "		Ready for OpenGL 2.1\n");
	else {
		fprintf(fp,"		Warning: Detected that OpenGL 2.1 not supported\n");
	}
}
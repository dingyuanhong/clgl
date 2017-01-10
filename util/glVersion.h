#pragma once

#include <stdio.h>
#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glu.h"

void glVersion()
{
	const GLubyte* name = glGetString(GL_VENDOR); //���ظ���ǰOpenGLʵ�ֳ��̵�����  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //����һ����Ⱦ����ʶ����ͨ���Ǹ�Ӳ��ƽ̨  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //���ص�ǰOpenGLʵ�ֵİ汾��  
	const GLubyte* Extensions = glGetString(GL_EXTENSIONS);
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //���ص�ǰGLU���߿�汾  
	printf("OpenGLʵ�ֳ��̵����֣�%s\n", name);
	printf("��Ⱦ����ʶ����%s\n", biaoshifu);
	printf("OpenGLʵ�ֵİ汾�ţ�%s\n", OpenGLVersion);
	//printf("OpenGL֧�ֵ���չ��%s\n", Extensions);
	printf("OGLU���߿�汾��%s\n", gluVersion);
}
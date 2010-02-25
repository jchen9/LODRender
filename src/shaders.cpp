#include "shaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

// #include <sys/types.h>
// todo global gpu program handlers

char *textFileRead(char *fn)
{
        FILE *fp;
        char *content = NULL;

        int f,count;
        f = open(fn, O_RDONLY);

        count = lseek(f, 0, SEEK_END);

        close(f);

        if (fn != NULL) {
                fp = fopen(fn,"rt");

                if (fp != NULL) {


                        if (count > 0) {
                                content = (char *)malloc(sizeof(char) * (count+1));
                                count = fread(content,sizeof(char),count,fp);
                                content[count] = '\0';
                        }
                        fclose(fp);
                }
        }

        return content;
}

void init_shaders()
{
/*
    // glew init
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // some problem
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    fprintf(stdout,"Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
*/
    
    printf("---> Initializing shader programs\n");
    init_shader_with("interval");
}

void init_shader_with(const char * name)
{
    printf("---> [TODO] Initializing %s shader program\n", name);
    /*
    char vFile[80];
    char fFile[80];

    strcpy(vFile, name);
    strcat(vFile, ".vert");

    strcpy(fFile, name);
    strcat(fFile, ".fraq");

    printf("Vertex program: [%s]\n", vFile);
    printf("Frag   program: [%s]\n", fFile);
    */


    /*
    // TODO
    GLhandleARB v, f, p;
    v = glCreateShaderObjectARB(GL_VERTEX_SHADER);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);

    char *vs, *fs;
    vs = textFileRead(vFile);
    fs = textFileRead(fFile);


    std::cout << "end redering" << std::endl;
    
    const char * vv = vs;
    const char * ff = fs;

    glShaderSourceARB(v, 1, &vv, NULL);
    glShaderSourceARB(f, 1, &ff, NULL);

    free(vs);free(fs);


    glCompileShaderARB(v);
    glCompileShaderARB(f);

    // printInfoLog(v);
    // printInfoLog(f);

    p = glCreateProgramObjectARB();

    glAttachObjectARB(p, v);
    glAttachObjectARB(p, f);

    glLinkProgramARB(p);
    // printInfoLog(p);

    glUseProgramObjectARB(p);
        glUseProgram(0);
    
        */

}

/**
GLhandleARB v,f,f2,p;

void setShaders() {

    char *vs,*fs;

    // create two shader objects and return their handles
    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    vs = textFileRead("toon.vert");
    fs = textFileRead("toon.frag");

    const char * vv = vs;
    const char * ff = fs;

    // sets the source code of the shaders to be the source code of the strings
    glShaderSourceARB(v, 1, &vv, NULL);
    glShaderSourceARB(f, 1, &ff, NULL);

    free(vs);free(fs);

    // compile the shaders - this can succeed or fail - more on this later
    glCompileShaderARB(v);
    glCompileShaderARB(f);

    // create a program object to link the shaders together and return its handle
    p = glCreateProgramObjectARB();

    // attach shader objects to the program object
    glAttachObjectARB(p,v);
    glAttachObjectARB(p,f);

    // link the program object - this can succeed or fail - more on this later
    glLinkProgramARB(p);

    // install the program object as part of the current rendering state
    glUseProgramObjectARB(p);
}


*/
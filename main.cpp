#include"OpenGLApp.h"


#undef main
int main(int, char**)
{
    
   
    
    
    OpenGLApp app;

    app.Run();

    //clean up
    app.Shutdown();
    

    return 0;
}

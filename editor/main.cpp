#include <app.h>
using namespace t4editor;

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {
        return app.run();
    }
    return -1;
}

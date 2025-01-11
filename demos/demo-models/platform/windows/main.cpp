#include <anthraxAI/engine.h>

int main(int argc, char **argv) {

	Engine::GetInstance()->Init();
	Engine::GetInstance()->Run();
	
	return 0;
}

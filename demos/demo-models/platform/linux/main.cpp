#include <anthraxAI/engine.h>

int main(int argc, char **argv) {

	Engine::GetInstance()->Init(argv);
	Engine::GetInstance()->Run();

	return 0;
}

#include "Scene.h"

void LoadScene() {
	Scene scene;
}

int main() {
	setlocale(LC_ALL, "Russian");
	std::cout << "Architecture X" << (sizeof(void*) == 8 ? "64" : "32") << std::endl;
	LoadScene();
    system("pause");
    return 0;
}
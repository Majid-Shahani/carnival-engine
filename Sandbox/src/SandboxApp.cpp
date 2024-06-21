#include <Carnival.h>

class Sandbox : public Carnival::Application {
public:
	Sandbox() {
	}
	~Sandbox() {
	}
};


Carnival::Application* Carnival::CreateApplication() {
	return new Sandbox();
}


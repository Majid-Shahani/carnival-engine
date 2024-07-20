#include <Carnival.h>

class ExampleLayer : public Carnival::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
	}

	void OnEvent(Carnival::Event& event) override
	{
	}
};

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


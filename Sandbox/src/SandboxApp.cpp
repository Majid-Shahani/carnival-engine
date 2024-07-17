#include <Carnival.h>

class ExampleLayer : public Carnival::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
	}

	void OnEvent(Carnival::Event& event) override
	{
		CL_TRACE(event.ToString());
	}
};

class Sandbox : public Carnival::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
	~Sandbox() {
	}
};


Carnival::Application* Carnival::CreateApplication() {
	return new Sandbox();
}


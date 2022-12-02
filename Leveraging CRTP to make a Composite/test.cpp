#include "Button.h"
#include <iostream>
//TEST

int main()
{
	sf::RenderWindow App(sf::VideoMode(1920, 1080), "");
	sf::Font font;
	font.loadFromFile("../Ressources/LEMONMILK-Regular.otf");
	sf::Texture t;
	t.loadFromFile("../Ressources/golden.png");

	//Buttons set up

	Button parent({ "parentName", font });
	parent.change_default_color(sf::Color::White);

	Button& child1 = parent.add("child1Name", sf::CircleShape(10));
	child1.change_default_color(sf::Color::Blue);

	Button& child2 = parent.add("child2Name", sf::CircleShape(50));
	//You can switch color or texture if you hold, click or do nothing with the button. You can also just put 3 same colors to have no particular state switch
	child2.change_default_color(sf::Color::Red);
	child2.set_position({ 800.f,800.f });

	Button& childOfchild1 = child2.add("childOfchild1", sf::CircleShape(30));

	parent.get("child1Name").set_position({900.f,900.f});
	parent.get("child2Name").create_function_call([]() { std::cout << "print whatever" << "\n"; });
	child2.get("childOfchild1");

	while (App.isOpen())
	{
		for (auto e = sf::Event{}; App.pollEvent(e);) {
			parent.apply_foreach<&Button::process_events>(e);
		}

		parent.set_color_state(sf::Color::Red, sf::Color::Cyan, sf::Color::Magenta);

		App.clear();
		parent.apply_foreach<&Button::draw>(App);
		App.display();
	}


	return 0;
}
#include <SFML/Graphics.hpp>

#include "SimplexNoise.h"

#define points 500
#define radius 250

#define deltaX 50
#define deltaY 50

#define deltaModX 600
#define deltaModY 0

#define deformScale 20

#define miliStep 1

std::vector<sf::Vector2f> GetPoints(int side) {

	sf::CircleShape cir(radius, points);

	std::vector<sf::Vector2f> ret;

	if (side == 0)
	{
		for (size_t i = 0; i < points / 2; i++)
		{
			sf::Vector2f pos = cir.getPoint(i);
			pos.x += deltaX;
			pos.y += deltaY;
			ret.push_back(pos);
		}
	}
	else
	{
		for (size_t i = points/2; i < points; i++)
		{
			sf::Vector2f pos = cir.getPoint(i);
			pos.x += deltaX;
			pos.y += deltaY;
			ret.push_back(pos);
		}		
	}

	return ret;

}

void Draw(std::vector<sf::Vector2f> vec, sf::RenderWindow & window) {

	for (size_t i = 1; i < vec.size(); i++)
	{

		sf::Vertex line[] = {
			sf::Vertex(vec[i]),
			sf::Vertex(vec[i - 1])
		};

		window.draw(line, 2, sf::Lines);

	}

}

void DrawLast(std::vector<sf::Vector2f> vec0, std::vector<sf::Vector2f> vec1, sf::RenderWindow & window) {

	sf::Vertex line[] = {
		vec0[vec0.size() - 1],
		vec1[0]
	};

	window.draw(line, 2, sf::Lines);

	sf::Vertex line1[] = {
		vec1[vec0.size() - 1],
		vec0[0]
	};

	window.draw(line1, 2, sf::Lines);

}

std::vector<float> GenerateDeform(float rng) {

	std::vector<float> ret;

	for (float i = 0; i < points/2; i++)
	{
		ret.push_back(SimplexNoise::noise(rng + i/10));
	}

	return ret;

}

std::vector<float> AdvanceDeform(std::vector<float> deform, float rng, float & totalDelta) {

	std::vector<float> ret = deform;

	for (int i = ret.size() - 1; i > 0; i--)
	{
		ret[i] = ret[i - 1];
	}

	totalDelta++;

	ret[0] = SimplexNoise::noise(rng + totalDelta/10);	

	return ret;

}

std::vector<sf::Vector2f> Deform(std::vector<sf::Vector2f> vec, int side, std::vector<float> deform) {

	std::vector<sf::Vector2f> ret = vec;

	int d = 0;
	int i = 0;

	if (side == 1)
	{
		for (i = 0; i < ret.size(); i++)
		{
			ret[i].x += deform[d] * deformScale;
			ret[i].x += deltaModX;
			ret[i].y += deltaModY;
			d++;
		}
	}
	else 
	{
		for (i = ret.size()-1; i >= 0; i--)
		{
			ret[i].x -= deform[d] * deformScale;
			ret[i].x += deltaModX;
			ret[i].y += deltaModY;
			d++;
		}
	}	

	return ret;

}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1200, 700), "Circle Noise Deformation");
	window.setFramerateLimit(60);

	srand(time(0));

	std::vector<sf::Vector2f> left;
	std::vector<sf::Vector2f> right;

	left = GetPoints(0);
	right = GetPoints(1);

	std::vector<sf::Vector2f> leftMod;
	std::vector<sf::Vector2f> rightMod;

	float rng = rand() / (float)RAND_MAX;
	float totalDelta = points / 2;
	std::vector<float> deform = GenerateDeform(rng);

	deform = AdvanceDeform(deform, rng, totalDelta);

	leftMod = Deform(left, 0, deform);
	rightMod = Deform(right, 1, deform);

	sf::Clock clock;

	while (window.isOpen())
	{
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		

		window.clear();
		
		Draw(left, window);
		Draw(right, window);
		DrawLast(right, left, window);

		Draw(leftMod, window);
		Draw(rightMod, window);
		DrawLast(rightMod, leftMod, window);

		window.display();

		if (clock.getElapsedTime().asMilliseconds() > miliStep)
		{
			deform = AdvanceDeform(deform, rng, totalDelta);

			leftMod = Deform(left, 0, deform);
			rightMod = Deform(right, 1, deform);

			clock.restart();
		}		

	}

	return 0;
}

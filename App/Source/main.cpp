#include <array>
#include <functional>
#include <random>

#include <SFML/Graphics.hpp>

// Utilities
#define sq(x) ((x) * (x))
#define cube(x) ((x) * (x) * (x))

// Is used to force alternation of points on the inner and outer circle
#define POINTS_ALTERNATE_ON_CURVE false

using Vec2 = sf::Vector2f;

// Constants
constexpr uint32_t MAX_LOOP_ITERATION = 100;
constexpr float WINDOW_SIZE = 800.0f;
constexpr float WINDOW_OUTTER_BORDER_OFFSET = 150.0f;
constexpr float WINDOW_INNER_BORDER_OFFSET = 200.0f;
constexpr size_t MAX_ANGLES = 12;
constexpr size_t MAX_POINTS = MAX_ANGLES;
constexpr size_t MAX_VERTICES = 8000;
constexpr float VERTICES_PER_SECOND = 250.0f;
constexpr float VERTICES_PER_CURVE = 100.0f;
constexpr float TIME_TO_CHANGE_ANGLES = 5.0f;

#pragma region Draw Functions
void DrawBoundCircles(sf::RenderTarget* _target, float _minRadius, float _maxRadius)
{
	sf::CircleShape circle(_minRadius);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1.0f);

	circle.setOutlineColor(sf::Color(0, 255, 0, 100));
	circle.setPosition(WINDOW_SIZE / 2.0f - _minRadius, WINDOW_SIZE / 2.0f - _minRadius);
	_target->draw(circle);

	circle.setRadius(_maxRadius);
	circle.setOutlineColor(sf::Color(255, 0, 0, 100));
	circle.setPosition(WINDOW_SIZE / 2.0f - _maxRadius, WINDOW_SIZE / 2.0f - _maxRadius);
	_target->draw(circle);
}

void DrawPoint(sf::RenderTarget* _target, const Vec2& _pointCoord)
{
	sf::CircleShape point(2.0f);
	point.setFillColor(sf::Color::Red);
	point.setPosition(_pointCoord);
	_target->draw(point);
}

void DrawCatmullRomCurve(sf::RenderTarget* _target, const std::vector<sf::Vertex>& _vertices, uint32_t _numberToDraw)
{
	for (uint32_t i = 1; i < _numberToDraw; i++)
	{
		sf::Vertex line[] =
		{
			_vertices[i - 1],
			_vertices[i]
		};
		_target->draw(line, 2, sf::LineStrip);
	}
}
#pragma endregion

#pragma region Random Functions
uint32_t GetRandomAngleWithoutCardinalAngle(uint32_t _index, uint32_t _minAngle = 0, uint32_t _maxAngle = 360, uint32_t _seed = time(NULL))
{
	std::mt19937 generator(_seed);
	std::uniform_int_distribution<uint32_t> distribution(_minAngle, _maxAngle);

	uint32_t angle;
	uint32_t loopCount = 0;
	do
	{
		angle = (_index % 3 == 0) * 30 * _index + (1 - (_index % 3 == 0)) * distribution(generator);
		loopCount++;
	} while (angle % 90 == 0 && loopCount < MAX_LOOP_ITERATION);

	return angle;
}

void GenerateAnglesValues(std::array<uint32_t, MAX_ANGLES>& _angles, uint32_t _angleMin, uint32_t _angleMax, float _time)
{
	for (uint32_t i = 0; i < MAX_ANGLES; i++)
	{
		_angles[i] = GetRandomAngleWithoutCardinalAngle(i, _angleMin, _angleMax);

		_angleMin += 45 * (1 - (i % 3 == 0));
		_angleMax += 45 * (1 - (i % 3 == 0));
	}
}

void GeneratesPointsValues(std::array<Vec2, MAX_POINTS>& _points, const std::array<uint32_t, MAX_ANGLES>& _angles, float _minRadius, float _maxRadius)
{
	for (uint32_t i = 0; i < _angles.size(); i++)
	{
		float angle = _angles[i] * 3.14159f / 180.0f;
#if POINTS_ALTERNATE_ON_CURVE
		bool isOnInnerCircle = i % 2 == 0;
#else
		bool isOnInnerCircle = rand() % 2 == 0;
#endif
		float radius = isOnInnerCircle * _minRadius + (1 - isOnInnerCircle) * _maxRadius;
		float x = WINDOW_SIZE / 2.0f + radius * cos(angle);
		float y = WINDOW_SIZE / 2.0f + radius * sin(angle);
		_points[i] = Vec2(x, y);
	}
}
#pragma endregion

#pragma region Compute Functions
sf::Color GetVertexColor(const Vec2& _coord)
{
	static Vec2 HalfScreenSize = Vec2(WINDOW_SIZE / 2.0f, WINDOW_SIZE / 2.0f);

	if (_coord.x > HalfScreenSize.x && _coord.y > HalfScreenSize.y) return sf::Color::Yellow;
	else if (_coord.x < HalfScreenSize.x && _coord.y > HalfScreenSize.y) return sf::Color::Cyan;
	else if (_coord.x < HalfScreenSize.x && _coord.y < HalfScreenSize.y) return sf::Color::Magenta;
	else return sf::Color::Blue;
}

void ComputeVerticesCoordinates(std::vector<sf::Vertex>& _vertices, const std::array<Vec2, MAX_POINTS>& _points, std::function<float(float)> _polynomials[4], float _drawTime)
{
	_vertices.clear();
	for (int64_t i = 3; i < MAX_POINTS + 3; i++)
	{
		Vec2 p1 = _points[(i - 3) % (MAX_POINTS)];
		Vec2 p2 = _points[(i - 2) % (MAX_POINTS)];
		Vec2 p3 = _points[(i - 1) % (MAX_POINTS)];
		Vec2 p4 = _points[i % (MAX_POINTS)];

		for (float t = 0; t < 1; t += 1.0f / VERTICES_PER_CURVE)
		{
			Vec2 coord = _polynomials[0](t) * p1 + _polynomials[1](t) * p2 + _polynomials[2](t) * p3 + _polynomials[3](t) * p4;
			_vertices.emplace_back(coord, GetVertexColor(coord));
		}
	}
}
#pragma endregion

void InitPolynomials(std::function<float(float)> _polynomials[4])
{
	_polynomials[0] = [=](float t)->float { return 0.0f - .5f * t + sq(t) - .5f * cube(t); };
	_polynomials[1] = [=](float t)->float { return 1.0f + 0.0f * t - 2.5f * sq(t) + 1.5f * cube(t); };
	_polynomials[2] = [=](float t)->float { return 0.0f + .5f * t + 2.0f * sq(t) - 1.5f * cube(t); };
	_polynomials[3] = [=](float t)->float { return 0.0f + 0.0f * t - .5f * sq(t) + .5f * cube(t); };
}

int main()
{
	// Window
	sf::RenderWindow window(sf::VideoMode(800, 800), "TD Maths");

	// Time
	sf::Clock clock;
	float time = 0.0f;
	float drawTime = 0.0f;

	// Constraints
	uint32_t angleMin = 10;
	uint32_t angleMax = 35;

	float minRadius = WINDOW_SIZE / 2.0f - WINDOW_INNER_BORDER_OFFSET;
	float maxRadius = WINDOW_SIZE / 2.0f - WINDOW_OUTTER_BORDER_OFFSET;

	// Angles
	std::array<uint32_t, MAX_ANGLES> angles;
	GenerateAnglesValues(angles, angleMin, angleMax, time);

	// Points
	std::array<Vec2, MAX_POINTS> points;
	GeneratesPointsValues(points, angles, minRadius, maxRadius);

	// Catmull Rom
	std::function<float(float)> polynomials[4];
	InitPolynomials(polynomials);

	// Curve Vertices
	std::vector<sf::Vertex> vertices;
	vertices.reserve(MAX_VERTICES);

	// Game Loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		// Time
		drawTime += clock.getElapsedTime().asSeconds();
		time += clock.restart().asSeconds();

		// Generate Angles and Points
		if (drawTime > TIME_TO_CHANGE_ANGLES)
		{
			drawTime = 0.0f;
			GenerateAnglesValues(angles, angleMin, angleMax, time);
			GeneratesPointsValues(points, angles, minRadius, maxRadius);
		}

		// Compute Vertices Coordinates
		ComputeVerticesCoordinates(vertices, points, polynomials, drawTime);

		// Rendering
		window.clear();

		DrawBoundCircles(&window, minRadius, maxRadius);
	
		// Compute how many vertices to draw
		uint32_t NumberToDraw = drawTime * VERTICES_PER_SECOND > vertices.size() ? vertices.size() : (uint32_t)(drawTime * VERTICES_PER_SECOND);
		DrawCatmullRomCurve(&window, vertices, NumberToDraw);

		// Draw Control Points
		for (const auto& point : points)
		{
			DrawPoint(&window, point);
		}

		window.display();
	}
	return 0;
}
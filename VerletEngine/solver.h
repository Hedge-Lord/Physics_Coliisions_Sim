#ifndef SOLVER_H
#define SOLVER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

struct Particle {
	sf::Vector2f pos_last;
	sf::Vector2f pos_curr;
	sf::Vector2f acc;
	float radius = 5.f;
	sf::CircleShape sprite;

	Particle() { sprite.setFillColor(sf::Color::White); sprite.setRadius(radius); }
	Particle(float rad, sf::Color color) {
		sprite.setFillColor(color);
		sprite.setRadius(rad);
		radius = rad;
		sprite.setOrigin(rad, rad);
	}

	void setVelocity(sf::Vector2f v, float dt) {
		pos_last = pos_curr - (v * dt);
	}

	void update(float dt) {
		const sf::Vector2f displacement = pos_curr - pos_last;
		pos_last = pos_curr;
		pos_curr = pos_curr + displacement + acc * (dt * dt);
		sprite.setPosition(pos_curr);
	}
};

class Solver {
public:
	Solver(int frameRate, sf::Vector2i dimensions) : frameRate(frameRate), dimensions(dimensions) {
		window.create(sf::VideoMode(dimensions.x, dimensions.y), "Collisions Simulation");
		window.setFramerateLimit(frameRate);
		frameTime = 1.0f / frameRate;
		stepTime = frameTime / numSteps;
	}

	void addParticle(sf::Vector2f accel, float rad, sf::Color color) {
		particles.push_back(Particle(rad, color));
		particles[particles.size() - 1].acc = accel;
		particles[particles.size() - 1].pos_curr = sf::Vector2f(dimensions.x/ 2, dimensions.y / 1.2f);
		particles[particles.size() - 1].pos_last = sf::Vector2f(dimensions.x / 2, dimensions.y / 1.2f);
	}

	void applyConstraint() {
		int size = particles.size();
		for (int i = 0; i < size; i++) {
			Particle& p = particles[i];
			if (p.pos_curr.y > dimensions.y - p.radius) p.pos_curr.y = dimensions.y - p.radius;
			if (p.pos_curr.x > dimensions.x - p.radius) p.pos_curr.x = dimensions.x - p.radius;
			// if (p.pos_curr.y < p.radius) p.pos_curr.y = p.radius; // ceiling
			if (p.pos_curr.x < p.radius) p.pos_curr.x = p.radius;
		}
	}

	void checkCollisions() {
		int size = particles.size();
		for (int i = 0; i < size; i++) {
			Particle& p1 = particles[i];
			for (int j = i + 1; j < size; j++) {
				Particle& p2 = particles[j];
				sf::Vector2f displacement = p1.pos_curr - p2.pos_curr;
				float dist_squared = displacement.x * displacement.x + displacement.y * displacement.y;
				float min_dist = p1.radius + p2.radius;
				if (dist_squared < min_dist * min_dist) {
					float dist = sqrt(dist_squared);
					sf::Vector2f normal = displacement / dist * (dist - min_dist) * 0.1f;
					float mass1 = p1.radius / min_dist;
					float mass2 = p2.radius / min_dist;
					p1.pos_curr -= normal * mass1;
					p2.pos_curr += normal * mass2;
				}
			}
		}
	}

	void updateObjects() {
		int size = particles.size();
		for (int i = 0; i < size; i++) particles[i].update(stepTime);
	}

	void renderObjects() {
		int size = particles.size();
		for (int i = 0; i < size; i++) window.draw(particles[i].sprite);
	}

	void run() {
		sf::Clock clock;
		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}


			// PARTICLE SPAWNING EVENT
			if (particles.size() <= maxNumObjects && clock.getElapsedTime() > sf::seconds(0.01f)) { 
				addParticle(gravity, particles.size() % 20 + 5, sf::Color(rand() % 255, rand() % 50 + 50, 200));
				particles[particles.size() - 1].setVelocity(sf::Vector2f(300, -200), frameTime);
				clock.restart();
			}



			for (int i = 0; i < numSteps; i++) {
				updateObjects();
				applyConstraint();
				checkCollisions();
			}


			window.clear();
			renderObjects();
			window.display();
		}
	}

	void setGravity(sf::Vector2f grav) { gravity = grav; }

	void setNumSteps(int num) {
		numSteps = num;
		stepTime = frameTime / num;
	}

	void setMaxNumObjects(int num) { maxNumObjects = num; }

private:
	sf::RenderWindow window;
	std::vector<Particle> particles;
	sf::Vector2f gravity;
	float frameTime;
	float stepTime;
	float numSteps = 1;
	int frameRate;
	sf::Vector2i dimensions;
	int maxNumObjects = 100;
};





#endif

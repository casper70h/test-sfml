#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void setPlayerCoordinateForView(float x, float y) {

	float tempX = x; float tempY = y; // считываем коорд. игрока и проверяем их, чтобы убрать края

	if (x < 400) tempX = 400; //убираем из вида левую сторону
	if (y < 340) tempY = 340; //верхнюю сторону
	if (y > 800) tempY = 800;//нижнюю стороню

	view.setCenter(tempX, tempY);
}


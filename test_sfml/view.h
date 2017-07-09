/*
#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void setPlayerCoordinateForView(float x, float y) {

	float tempX = x; float tempY = y; // ��������� �����. ������ � ��������� ��, ����� ������ ����

	if (x < 400) tempX = 400; //������� �� ���� ����� �������
	if (y < 340) tempY = 340; //������� �������
	if (y > 800) tempY = 800;//������ �������

	view.setCenter(tempX, tempY);
}
*/

#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void setPlayerCoordinateForView(float x, float y) {
	float tempX = x; float tempY = y;

	if (y > 624) tempY = 624;//bottom

	view.setCenter(tempX, tempY);
}


#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
//#include <conio.h>
#include <vector>
#include <list>

using namespace sf;



////////////////////////////////////����� ����� ��������//////////////////////////
class Entity {
public:
	float dx, dy, x, y, speed, moveTimer;//�������� ���������� ������ ��� ������� �����
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;//����� ����� ���� ������, �� �� ����� ������ ������ ����� ��� �������������� �����.����� ���� �������� ������ �� ����� � ����� ������� ���� �������� � update � ����������� �� �����
	Entity(Image &image, float X, float Y, int W, int H, String Name) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}

	virtual void update(float time) = 0;
};
////////////////////////////////////////////////////����� ������////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//��������� ��� ������������ - ��������� �������
	int playerScore;//��� ���������� ����� ���� ������ � ������
	bool isShoot;

	Player(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		playerScore = isShoot = 0; state = stay;
		if (name == "Player1") {
			sprite.setTextureRect(IntRect(27, 198, w, h));
		}
	}

	void control() {
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				state = left; speed = 0.1;
				sprite.setTextureRect(IntRect(3, 230, w, h));
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1;
				sprite.setTextureRect(IntRect(50, 167, w, h));
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//���� ������ ������� ����� � �� �� �����, �� ����� �������
				state = jump; dy = -0.6; onGround = false;//�������� ������ ������
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}

		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = y / 17; i < (y + h) / 17; i++)//���������� �� ��������� �����
			for (int j = x / 17; j<(x + w) / 17; j++)
			{
				if (TileMap[i][j] == '0')//���� ������� ��� ������ �����? ��
				{
					if (Dy>0) { y = i * 17 - h;  dy = 0; onGround = true; }//�� Y ����=>���� � ���(����� �� �����) ��� ������. � ���� ������ ���� ���������� ��������� � ��������� ��� �� �����, ��� ���� ������� ��� �� �� ����� ��� ����� ����� ����� �������
					if (Dy<0) { y = i * 17 + 17;  dy = 0; }//������������ � �������� ������ �����(����� � �� �����������)
					if (Dx>0) { x = j * 17 - w; }//� ������ ����� �����
					if (Dx<0) { x = j * 17 + 17; }// � ����� ����� �����
				}
				//else { onGround = false; }//���� ������ �.� �� ����� ���������� � �� ������ ����������� ��� ��������� ������� �������� ����
			}
	}

	void update(float time)
	{
		control();//������� ���������� ����������
		switch (state)//��� �������� ��������� �������� � ����������� �� ���������
		{
		case right:dx = speed; break;//��������� ���� ������
		case left:dx = -speed; break;//��������� ���� �����
		case up: break;//����� ��������� �������� ������ (�������� �� ��������)
		case down: dx = 0; break;//����� ��������� �� ����� ������ ��������� (�������� �� ��������)
		case stay: break;//� ����� ����		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//������������ ������������ �� �
		y += dy*time;
		checkCollisionWithMap(0, dy);//������������ ������������ �� Y
		sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
		if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//������� � ����� ������ ����������� � �����
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015*time;//��������� ������������� � �����
	}
};


class Enemy :public Entity {
public:
	Enemy(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		if (name == "EasyEnemy") {
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;//���� ��������.���� ������ ������ ���������
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//�-�� �������� ������������ � ������
	{
		for (int i = y / 17; i < (y + h) / 17; i++)//���������� �� ��������� �����
			for (int j = x / 17; j<(x + w) / 17; j++)
			{
				if (TileMap[i][j] == '0')//���� ������� ��� ������ �����, ��
				{
					if (Dy>0) { y = i * 17 - h; }//�� Y ����=>���� � ���(����� �� �����) ��� ������. � ���� ������ ���� ���������� ��������� � ��������� ��� �� �����, ��� ���� ������� ��� �� �� ����� ��� ����� ����� ����� �������
					if (Dy<0) { y = i * 17 + 17; }//������������ � �������� ������ �����(����� � �� �����������)
					if (Dx>0) { x = j * 17 - w; dx = -0.1; sprite.scale(-1, 1); }//� ������ ����� �����
					if (Dx<0) { x = j * 17 + 17; dx = 0.1; sprite.scale(-1, 1); }// � ����� ����� �����
				}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy") {

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//������ ����������� �������� ������ 3 ���
			checkCollisionWithMap(dx, 0);//������������ ������������ �� �
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
			if (health <= 0) { life = false; }
		}
	}
};


/////////////////////// ����� ���������� ��������� //////////////////////////
class MovingPlatform : public Entity {//����� ���������� ���������
public:
	MovingPlatform(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		sprite.setTextureRect(IntRect(0, 34, W, H));//������������� 
		dx = 0.08;//����������� ��������� �� �
	}

	void update(float time)//������� ���������� ���������.
	{
		x += dx * time;//���������� �������� �� �����������
		moveTimer += time;//���������� ������
		if (moveTimer>2000) { dx *= -1; moveTimer = 0; }//���� ������ �������� 2 ���, �� �������� ����������� �������� ���������,� ������ ����������
		sprite.setPosition(x + w / 2, y + h / 2);//������ ������� �������
	}
};

//class MENU {
//public:
//	Sprite sprite;
//	MENU(Image &image, float X, float Y, int W, int H, String Name) {
//		sprite.setTextureRect(IntRect(0, 0, W, H));//�������������
//	}
//};



/////////////////////// ����� ����(��������) //////////////////////////
class Bullet :public Entity {//����� ����
public:
	int direction;//����������� ����

	Bullet(Image &image, float X, float Y, int W, int H, String Name, int dir) :Entity(image, X, Y, W, H, Name) {
		x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 16;
		life = true;
	}


	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0;   break;
		case 1: dx = speed; dy = 0;   break;
		case 2: dx = 0; dy = -speed;   break;
		case 3: dx = 0; dy = -speed;   break;
		case 4: dx = 0; dy = -speed;   break;
		case 5: dx = 0; dy = -speed;   break;
		}

		x += dx*time;
		y += dy*time;

		//if (x <= 0) x = 1;
		//if (y <= 0) y = 1;

		for (int i(0); i < WIDTH_MAP; i++) {
			if (x == TileMap[26][i] && TileMap[26][i] == '0') {
				life = false;
			}
		}

		sprite.setPosition(x + w / 2, y + h / 2);//�������� ������� ����
	}
};


int main()
{
	RenderWindow window(VideoMode(900, 400), "test");
	view.reset(FloatRect(0, 0, 800, 400));


	Image map_image;
	map_image.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	Image heroImage;
	heroImage.loadFromFile("images\\Pokemon.png");
	heroImage.createMaskFromColor(Color(40, 152, 0));


	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images\\dark.png");
	easyEnemyImage.createMaskFromColor(Color(0, 0, 255));//����� �� �����


	Player p(heroImage, 300, 200, 17, 24, "Player1");//������ ������ ������
	Enemy easyEnemy(easyEnemyImage, 240, 380, 22, 29, "EasyEnemy");//������� ����, ������ ������ �����

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	MovingPlatform movePlatform(movePlatformImage, 650, 373, 50, 16, "MovingPlatform");

	Image BulletImage;//����������� ��� ����
	BulletImage.loadFromFile("images\\bullet.png");//��������� �������� � ������ �����������
	BulletImage.createMaskFromColor(Color(0, 0, 0));


	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;
	std::list<Entity*>::iterator it2;//������ ��������.��� �������������� ����� ��������� ������



	Clock clock;

	//Image menuButtonsImage;
	//menuButtonsImage.loadFromFile("images\\menuButtons.png");
	//menuButtonsImage.createMaskFromColor(Color(0, 0, 0));
	//MENU mainMenu(menuButtonsImage, 300, 200, 150, 50, "playButton");


	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;


		if ((movePlatform.name== "MovingPlatform") && (movePlatform.getRect().intersects(p.getRect())))//���� ����� ���������� � �������� ������ � ��� ����� ������� movingplatform
		{
			if ((p.dy>0) || (p.onGround == false))//��� ���� ����� ��������� � ��������� ����� ������, �.� ������ ����
				if (p.y + p.h<movePlatform.y + movePlatform.h)//���� ����� ��������� ���� ���������, �.� ��� ��� ���� ������� (�� �� ��� ��������� ��� �� ���������� � ����������)
				{
					p.y = movePlatform.y - p.h + 3; p.x += movePlatform.dx*time; p.dy = 0; p.onGround = true; // �� ����������� ������ ���, ����� �� ��� �� ����� �� ���������
				}
		}




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, p.x, p.y, 16, 16, "Bullet", p.state)); }//���� ����������, �� ���������� ����
		}

		for (it = entities.begin(); it != entities.end();)
		{
			Entity *b = *it;
			b->update(time);
			if (b->life == false) { it = entities.erase(it); delete b; }
			else it++;
		}

		p.update(time);// Player update function	
		movePlatform.update(time);
		easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear();


		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 17, 17));
				//if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(576, 175, 17, 17));
				//if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				//if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				if ((TileMap[i][j] == 'd')) s_map.setTextureRect(IntRect(0, 17, 17, 17));
				//if ((TileMap[i][j] == 'p')) s_map.setTextureRect(IntRect(0, 34, 17, 17));
				s_map.setPosition(j * 17, i * 17);

				window.draw(s_map);
			}

		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite);
		}
		window.draw(easyEnemy.sprite);
		window.draw(p.sprite);
		window.draw(movePlatform.sprite);
		window.display();
	}
	return 0;
}
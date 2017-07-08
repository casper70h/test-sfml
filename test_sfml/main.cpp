#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
//#include <conio.h>
#include <vector>
#include <list>

using namespace sf;



////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
	float dx, dy, x, y, speed, moveTimer;//добавили переменную таймер для будущих целей
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;//враги могут быть разные, мы не будем делать другой класс для различающегося врага.всего лишь различим врагов по имени и дадим каждому свое действие в update в зависимости от имени
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
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
	int playerScore;//эта переменная может быть только у игрока
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

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
				state = jump; dy = -0.6; onGround = false;//увеличил высоту прыжка
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}

		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 17; i < (y + h) / 17; i++)//проходимся по элементам карты
			for (int j = x / 17; j<(x + w) / 17; j++)
			{
				if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
				{
					if (Dy>0) { y = i * 17 - h;  dy = 0; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
					if (Dy<0) { y = i * 17 + 17;  dy = 0; }//столкновение с верхними краями карты(может и не пригодиться)
					if (Dx>0) { x = j * 17 - w; }//с правым краем карты
					if (Dx<0) { x = j * 17 + 17; }// с левым краем карты
				}
				//else { onGround = false; }//надо убрать т.к мы можем находиться и на другой поверхности или платформе которую разрушит враг
			}
	}

	void update(float time)
	{
		control();//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right:dx = speed; break;//состояние идти вправо
		case left:dx = -speed; break;//состояние идти влево
		case up: break;//будет состояние поднятия наверх (например по лестнице)
		case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
		case stay: break;//и здесь тоже		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy*time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
		if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//убираем и будем всегда притягивать к земле
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015*time;//постоянно притягиваемся к земле
	}
};


class Enemy :public Entity {
public:
	Enemy(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		if (name == "EasyEnemy") {
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;//даем скорость.этот объект всегда двигается
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф-ия проверки столкновений с картой
	{
		for (int i = y / 17; i < (y + h) / 17; i++)//проходимся по элементам карты
			for (int j = x / 17; j<(x + w) / 17; j++)
			{
				if (TileMap[i][j] == '0')//если элемент наш тайлик земли, то
				{
					if (Dy>0) { y = i * 17 - h; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
					if (Dy<0) { y = i * 17 + 17; }//столкновение с верхними краями карты(может и не пригодиться)
					if (Dx>0) { x = j * 17 - w; dx = -0.1; sprite.scale(-1, 1); }//с правым краем карты
					if (Dx<0) { x = j * 17 + 17; dx = 0.1; sprite.scale(-1, 1); }// с левым краем карты
				}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy") {

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0) { life = false; }
		}
	}
};


/////////////////////// КЛАСС ДВИЖУЩЕЙСЯ ПЛАТФОРМЫ //////////////////////////
class MovingPlatform : public Entity {//класс движущейся платформы
public:
	MovingPlatform(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		sprite.setTextureRect(IntRect(0, 34, W, H));//прямоугольник 
		dx = 0.08;//изначальное ускорение по Х
	}

	void update(float time)//функция обновления платформы.
	{
		x += dx * time;//реализация движения по горизонтали
		moveTimer += time;//наращиваем таймер
		if (moveTimer>2000) { dx *= -1; moveTimer = 0; }//если прошло примерно 2 сек, то меняется направление движения платформы,а таймер обнуляется
		sprite.setPosition(x + w / 2, y + h / 2);//задаем позицию спрайту
	}
};

//class MENU {
//public:
//	Sprite sprite;
//	MENU(Image &image, float X, float Y, int W, int H, String Name) {
//		sprite.setTextureRect(IntRect(0, 0, W, H));//прямоугольник
//	}
//};



/////////////////////// КЛАСС ПУЛИ(СТРЕЛЬБЫ) //////////////////////////
class Bullet :public Entity {//класс пули
public:
	int direction;//направление пули

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

		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
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
	easyEnemyImage.createMaskFromColor(Color(0, 0, 255));//маску по цвету


	Player p(heroImage, 300, 200, 17, 24, "Player1");//объект класса игрока
	Enemy easyEnemy(easyEnemyImage, 240, 380, 22, 29, "EasyEnemy");//простой враг, объект класса врага

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	MovingPlatform movePlatform(movePlatformImage, 650, 373, 50, 16, "MovingPlatform");

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images\\bullet.png");//загрузили картинку в объект изображения
	BulletImage.createMaskFromColor(Color(0, 0, 0));


	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;
	std::list<Entity*>::iterator it2;//второй итератор.для взаимодействия между объектами списка



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


		if ((movePlatform.name== "MovingPlatform") && (movePlatform.getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
		{
			if ((p.dy>0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
				if (p.y + p.h<movePlatform.y + movePlatform.h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
				{
					p.y = movePlatform.y - p.h + 3; p.x += movePlatform.dx*time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
				}
		}




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, p.x, p.y, 16, 16, "Bullet", p.state)); }//если выстрелили, то появляется пуля
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
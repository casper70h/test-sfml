#include <SFML/Graphics.hpp>
//#include "map.h"
#include "view.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include "iostream"
#include "level.h"
#include <vector>
#include <list>
#include "TinyXML/tinyxml.h"

using namespace sf;


////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//вектор объектов карты
	float dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image &image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {//ф-ция получения прямоугольника. его коорд,размеры (шир,высот).
		return FloatRect(x, y, w, h);//эта ф-ция нужна для проверки столкновений 
	}

	float getX() {
		return float(x);
	}

	virtual void update(float time) = 0; //все потомки переопределяют эту функцию
};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;
	int playerScore;
	bool isShoot;
	Player(Image &image, String Name, Level &lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		playerScore = 0; state = stay; obj = lev.GetAllObjects();//инициализируем.получаем все объекты для взаимодействия персонажа с картой
		if (name == "Player1") {
			sprite.setTextureRect(IntRect(176, 0, w, h));
		}
	}

	float getRotation(float rotation) {
		return rotation;
	}

	int bulletDirection = 0;

	float currentFrame = 0;
	void controlWithRotation(float time, float rotation) {
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				if ((rotation < -90 && rotation > -180) || (rotation >= 90 && rotation <= 180)) {
					bulletDirection = 0; state = left; speed = 0.1;
					currentFrame += 0.01*time;
					if (currentFrame > 6) currentFrame -= 6;
					if (int(currentFrame) == 6) sprite.setTextureRect(IntRect(199, 0, -w, h));
					else sprite.setTextureRect(IntRect(29 * int(currentFrame) + 29, 0, -w, h));

				}
				if ((rotation >= -90 && rotation <= 0) || (rotation > 0 && rotation < 90)) {
					bulletDirection = 1; state = left; speed = 0.1;
					currentFrame += 0.01*time;
					if (currentFrame > 6) currentFrame -= 6;
					sprite.setTextureRect(IntRect(29 * int(currentFrame), 0, w, h));
				}
				
			}
			

			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				if ((rotation <= -90 && rotation > -180) || (rotation > 90 && rotation <= 180)) {
					bulletDirection = 0; state = right; speed = 0.1;
					currentFrame += 0.01*time;
					if (currentFrame > 6) currentFrame -= 6;
					if (int(currentFrame) == 6) sprite.setTextureRect(IntRect(199, 0, -w, h));
					else sprite.setTextureRect(IntRect(29 * int(currentFrame) + 29, 0, -w, h));
				}
				if ((rotation > -90 && rotation <= 0) || (rotation > 0 && rotation <= 90)) {
					bulletDirection = 1; state = right; speed = 0.1;
					currentFrame += 0.01*time;
					if (currentFrame > 6) currentFrame -= 6;
					sprite.setTextureRect(IntRect(29 * int(currentFrame), 0, w, h));
				}
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

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			isShoot = true;
		}
		
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		/*вариант взаимодействия со старой картой
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
		if (TileMap[i][j] == '0')
		{
		if (Dy>0){ y = i * 32 - h;  dy = 0; onGround = true; }
		if (Dy<0){ y = i * 32 + 32;  dy = 0; }
		if (Dx>0){ x = j * 32 - w; }
		if (Dx<0){ x = j * 32 + 32; }
		}
		}*/

		for (int i = 0; i < obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w; }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; }
				}
			}
	}


	float returnTime(float time) {
		return time;
	}

	void update(float test) {

	}

	void updateWithRotation(float time, float rotation)
	{
		controlWithRotation(time, rotation);//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right:dx = speed; break;//состояние идти вправо
		case left:dx = -speed; break;//состояние идти влево
		case up: break;//будет состояние поднятия наверх (например по лестнице)
		case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
		case stay: dx = 0; break;//и здесь тоже		
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

class AnyObject {
public:
	std::vector<Object> obj;//вектор объектов карты
	float dX, dY, dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;

	enum { left, right, up, down, jump, stay } state;

	int playerScore;
	bool isShoot;
	AnyObject(Image &image, String Name, Level &lev, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, w, h));
	}

	void update(float time, float px, float py, float rotation)
	{	
		if ((rotation < -90 && rotation >= -180) || (rotation > 90 && rotation <= 180)) {
			x = px + 2.3;
			y = py + 23;
			sprite.setTextureRect(IntRect(0, 14, w, -h));
		}
		else {
			x = px-4;
			y = py + 11;
			sprite.setTextureRect(IntRect(0, 0, w, h));
		}
		sprite.setPosition(x + w / 2, y + h / 2);
		setPlayerCoordinateForView(x, y);
	}
};

class Enemy :public Entity {
public:
	Enemy(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");//инициализируем.получаем нужные объекты для взаимодействия врага с картой	if (name == "EasyEnemy") {
		sprite.setTextureRect(IntRect(0, 0, w, h));
		dx = 0.1;//даем скорость.этот объект всегда двигается
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		/*вариант взаимодействия со старой картой
		for (int i = y / 32; i < (y + h) / 32; i++)
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
		if (TileMap[i][j] == '0')
		{
		if (Dy>0){ y = i * 32 - h; }
		if (Dy<0){ y = i * 32 + 32; }
		if (Dx>0){ x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }
		if (Dx<0){ x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }
		}
		}*/

		for (int i = 0; i<obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				//if (obj[i].name == "solid"){//если встретили препятствие (объект с именем solid)
				if (Dy>0
					) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
				if (Dy<0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
				if (Dx>0) { x = obj[i].rect.left - w;  dx = -0.1; sprite.scale(-1, 1); }
				if (Dx<0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; sprite.scale(-1, 1); }
				//}
			}
	}

	int die_counter = 0;

	int beforeDying(float time, int die_counter) {
		std::cout << die_counter;
		//currentFrame += 0.1*time;
		Image tmpImage;
		tmpImage.loadFromFile("images\\spiderBlood.png");
		Texture tmpTexture;
		tmpTexture.loadFromImage(tmpImage);
		Sprite tmpSprite;
		tmpSprite.setTexture(tmpTexture);
		tmpSprite.setPosition(x, y);
		if (die_counter==13) return -1;
		return die_counter;
	}

	void update(float time)
	{
		if (name == "EasyEnemy") {
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0) { 
				dx = 0;
				beforeDying(time, die_counter);
				die_counter++;
				if (beforeDying(time, die_counter) == -1) life = false;
			}
		}
	}

};




/////////////////////// КЛАСС ДВИЖУЩЕЙСЯ ПЛАТФОРМЫ //////////////////////////
class MovingPlatform : public Entity {//класс движущейся платформы
public:
	MovingPlatform(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
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
	float _x, _y;
	Bullet(Image &image, String Name, Level &lvl, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {
		_x = X;
		_y = Y;
		x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 50;
		life = true;
	}
	void update(float time)
	{
		
		switch (direction) {
			case 0: dx = -speed; dy = 0;   break;
			case 1: dx = speed; dy = 0;   break;
			case 2: dx = 0; dy = -speed;   break;
			case 3: dx = 0; dy = -speed;   break;
			case 4: dx = 0; dy = -speed;   break;
			case 5: dx = 0; dy = -speed;   break;
		}

		x += dx*time;
		y += dy*time;

		if (abs((_x)-(x)) >= 250) life = false;
		if (abs((_y)-(y)) >= 250) life = false;

		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
	}
};


int main()
{
	RenderWindow window(VideoMode(900, 400), "test");
	view.reset(FloatRect(0, 0, 800, 400));


	//метод загрузки старой карты
			/*
			Image map_image;
			map_image.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
			Texture map;
			map.loadFromImage(map_image);
			Sprite s_map;
			s_map.setTexture(map);
			*/
	
	Level lvl;//создали экземпляр класса уровень
	lvl.LoadFromFile("newMap.tmx");//загрузили в него карту, внутри класса с помощью методов он ее обработает.

	Image heroImage;
	heroImage.loadFromFile("images\\player.png");
	heroImage.createMaskFromColor(Color(0, 0, 0));

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images\\spider.png");
	easyEnemyImage.createMaskFromColor(Color(255, 255, 255));//маску по цвету



	Image heroImageHand;
	heroImageHand.loadFromFile("images\\playerHand.png");
	heroImageHand.createMaskFromColor(Color(255, 255, 255));

	Object playerHand = lvl.GetObject("playerHand");
	AnyObject p_h(heroImageHand, "playerHand", lvl, playerHand.rect.left, playerHand.rect.top, 33, 14);


	Object player = lvl.GetObject("player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//объект легкого врага на нашей карте.задаем координаты игроку в начале при помощи него

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 30, 50);//передаем координаты прямоугольника player из карты в координаты нашего игрока
	//Enemy easyEnemy(easyEnemyImage, "EasyEnemy", lvl, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 80, 38);//передаем координаты прямоугольника easyEnemy из карты в координаты нашего врага

			//Player p(heroImage, 300, 200, 30, 50, "Player1");//объект класса игрока
			//Enemy easyEnemy(easyEnemyImage, 240, 386, 80, 38, "EasyEnemy");//простой враг, объект класса врага

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	//MovingPlatform movePlatform(movePlatformImage, "MovingPlatform", lvl, 650, 373, 50, 16);

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images\\bullet.png");//загрузили картинку в объект изображения
	BulletImage.createMaskFromColor(Color(0, 0, 0));


	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it; //итер для прохождения по эл. списка
	std::list<Entity*>::iterator it2;//второй итератор.для взаимодействия между объектами списка

	std::vector<Object> en = lvl.GetObjects("EasyEnemy"); //все объекты врака EasyEnemy на TMX хранятся в этом векторе

	std::vector<Object> pl = lvl.GetObjects("MovingPlatform"); // забираем все платформы в вектор

	for (int i(0); i < en.size(); i++) {
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, en[i].rect.left, en[i].rect.top, 80, 38));
	}

	for (int i(0); i < pl.size(); i++) {
		entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform", lvl, pl[i].rect.left, pl[i].rect.top, 50, 16));
	}



	Clock clock;

	//Image menuButtonsImage;
	//menuButtonsImage.loadFromFile("images\\menuButtons.png");
	//menuButtonsImage.createMaskFromColor(Color(0, 0, 0));
	//MENU mainMenu(menuButtonsImage, 300, 200, 150, 50, "playButton");


	float dX, dY = 0;

	while (window.isOpen())
	{

		Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
		Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)

		float dX = pos.x - p_h.x;//вектор , колинеарный прямой, которая пересекает спрайт и курсор
		float dY = pos.y - p_h.y;//он же, координата y
		float rotation = (atan2(dY, dX)) * 180 / 3.14159265;//получаем угол в радианах и переводим его в градусы
		std::cout << rotation << "\n";//смотрим на градусы в консольке
		p_h.sprite.setRotation(rotation);//поворачиваем спрайт на эти градусы

		
		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		//можно платформу создать так но я добавил её в общий цикл ниже с началом it
		/*if ((movePlatform.name== "MovingPlatform") && (movePlatform.getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
		{
			if ((p.dy>0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
				if (p.y + p.h<movePlatform.y + movePlatform.h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
				{
					p.y = movePlatform.y - p.h + 3; p.x += movePlatform.dx*time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
				}
		}
		*/



		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();						
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet",lvl, p.x, p.y, 16, 16, p.bulletDirection)); }//если выстрелили, то появляется пуля
		}
			

		p.updateWithRotation(time, rotation);// Player update function	

		p_h.update(time, p.x, p.y, rotation);
		

		/*
		for (it = entities.begin(); it != entities.end(); it++) {
			(*it)->update(time); // вызываем функ. update для всех эл. списка
		}
		*/


		
		for (it = entities.begin(); it != entities.end();)
		{
			Entity *b = *it;
			b->update(time);  //for (it = entities.begin(); it != entities.end(); it++) {(*it)->update(time); // вызываем функ. update для всех эл. списка
			if (b->life == false) { it = entities.erase(it); delete b; }
			else it++;
		}
		


		for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			if (((*it)->name == "MovingPlatform") && ((*it)->getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
			{
				Entity *movPlat = *it;
				if ((p.dy>0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
					if (p.y + p.h<movPlat->y + movPlat->h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
					{
						p.y = movPlat->y - p.h + 3; p.x += movPlat->dx*time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
					}
			}
			/*
			if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
			{
				if ((*it)->name == "EasyEnemy") {//и при этом имя объекта EasyEnemy,то..
					if ((*it)->dx>0)//если враг идет вправо
					{
						std::cout << "(*it)->x" << (*it)->x << "\n";//коорд игрока
						std::cout << "p.x" << p.x << "\n\n";//коорд врага

						(*it)->x = p.x - (*it)->w; //отталкиваем его от игрока влево (впритык)
						(*it)->dx = 0;//останавливаем

						std::cout << "new (*it)->x" << (*it)->x << "\n";//новая коорд врага
						std::cout << "new p.x" << p.x << "\n\n";//новая коорд игрока (останется прежней)
					}
					if ((*it)->dx < 0)//если враг идет влево
					{
						(*it)->x = p.x + p.w; //аналогично - отталкиваем вправо
						(*it)->dx = 0;//останавливаем
					}
					///////выталкивание игрока
					if (p.dx < 0) { p.x = (*it)->x + (*it)->w; }//если столкнулись с врагом и игрок идет влево то выталкиваем игрока
					if (p.dx > 0) { p.x = (*it)->x - p.w; }//если столкнулись с врагом и игрок идет вправо то выталкиваем игрока
				}
			}
			*/
			
			
			for (it2 = entities.begin(); it2 != entities.end(); it2++) {
				
				if ((*it)->getRect() != (*it2)->getRect())//при этом это должны быть разные прямоугольники
					if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyEnemy") && ((*it2)->name == "EasyEnemy"))//если столкнулись два объекта и они враги
					{
						(*it)->dx *= -1;//меняем направление движения врага
						(*it)->sprite.scale(-1, 1);//отражаем спрайт по горизонтали
					}
				if (((*it)->name == "Bullet") && ((*it2)->name == "EasyEnemy") && (*it)->getRect().intersects((*it2)->getRect())) {
					(*it2)->dx = 0;
					Image spiderBlood;
					spiderBlood.loadFromFile("images\\spiderBlood.png");
					spiderBlood.createMaskFromColor(Color(255, 255, 255));
					(*it2)->texture.loadFromImage(spiderBlood);
					(*it2)->sprite.setTexture((*it2)->texture);
					(*it2)->health = 0;
				}
			}
		}

						//   ^
						/*   |  
						for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
						{
							if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
							{
								if ((*it)->name == "easyEnemy") {//и при этом имя объекта EasyEnemy,то..

									if ((p.dy>0) && (p.onGround == false)) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }//если прыгнули на врага,то даем врагу скорость 0,отпрыгиваем от него чуть вверх,даем ему здоровье 0
									else {
										p.health -= 5;	//иначе враг подошел к нам сбоку и нанес урон
									}
								}
							}
						}
						*/



		//movePlatform.update(time);
		//easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//рисуем новую карту

		//создание карты старая реализация
							/*for (int i = 0; i < HEIGHT_MAP; i++)
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
							*/
			
		
			//window.draw(easyEnemy.sprite);

		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite); //рисуем всех врагов которые находятся в списке
		}


		
		window.draw(p.sprite);
		window.draw(p_h.sprite);
		//window.draw(movePlatform.sprite);
		window.display();
	}
	return 0;
}


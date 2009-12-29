#include <allegro.h> 
#include <fstream> // ustawienia w plikach
#include <time.h> // do srand wykorzystującego time
#include <string> // operacje na stringach
#include <sstream>  // zamiana stringa na int
#include <alpng.h> // pliki png

using namespace std;

class Resolution;
class Player;
class Enemy;
class Enemy_list;
class Bullet;
class Bullet_list;
class Bonus;
class Bonus_list;
class Exp;
class Exp_List;
class Manage;
class Engine;

BITMAP *bufor, *back, *p1_gfx, *p2_gfx, *hp_bar_gfx, *menu_gfx, *item_gfx;
BITMAP *b1_gfx, *b2_gfx, *b3_gfx, *b4_gfx, *b_gfx;
BITMAP *b11_gfx, *b12_gfx, *b13_gfx, *b14_gfx, *b15_gfx, *b16_gfx, *b17_gfx, *b18_gfx, *b19_gfx;
BITMAP *e1_gfx, *e2_gfx, *e3_gfx, *e4_gfx, *e5_gfx, *e6_gfx, *e_gfx;
BITMAP *bonus1_gfx, *bonus2_gfx, *bonus3_gfx, *bonus4_gfx, *bonus_gfx;
// BITMAP *exp_gfx, *exp1_gfx, *exp2_gfx, *exp3_gfx, *exp4_gfx;
float prop_x, prop_y;

class Resolution{
	public:
	int x,y,mode;
	string screen_mode,string_x,string_y;
	void check_settings(){
		ifstream istnieje("settings.txt");
		if(istnieje){ // sprawdzanie poprawnosci ustawien
			getline(istnieje,screen_mode);
			getline(istnieje,string_x);
			getline(istnieje,string_y);
			if(screen_mode != "1" && screen_mode != "2"){ // znalazlem blad
				istnieje.close();
				save_settings(2,800,600);
				return;
			}
			istringstream iss(string_x);
			istringstream iss2(string_y);
			iss >> x;
			iss2 >> y;
			if(!((x==800 && y==600) || (x==1024 && y==768) || (x==1240 && y==800))){ // znazlem blad
				istnieje.close();
				save_settings(2,800,600);
				return;
			}
		}
		else{
			save_settings(2,800,600);
		}
	}
	void load_settings(){
		string s1,s2,s3;
		ifstream plik("settings.txt");
		getline(plik,s1); // screen mode
		getline(plik,s2); // x
		getline(plik,s3); // y
		istringstream iss(s1);
		istringstream iss2(s2);
		istringstream iss3(s3);
		iss >> this->mode;
		iss2 >> this->x;
		iss3 >> this->y;
		plik.close();
	}
	void save_settings(int new_mode, int new_x, int new_y){
		ofstream new_settings("settings.txt");
		new_settings << new_mode << endl; // okienko
		new_settings << new_x << endl;
		new_settings << new_y << endl;
		new_settings.close();
	}
};

Resolution scr;

class Player{
	friend class Engine;
	friend class Manage;
	int x,y,weapon,hp,lifes,points,bullet_side;
	Player(){
		points=0;
	}
	void start_val(){
		x = scr.x/2; // ustawiam gracza na ekranie
		y = scr.y-100;
		weapon = 1; // na poczatek gracz dostaje najsłabszą broń
		hp = 100; // okreslam procent zycia
		lifes = 5; // posiada 5 zyc oraz
		points = 0; // zero pkt
		bullet_side = 0;
	}
};

class Enemy{
	friend class Manage;
	friend class Enemy_list;
	int id,x,y,hp,type;
};

class Enemy_list{
	friend class Manage;
	int licznik;
	struct Wezel{
		Enemy E;
		Wezel *next;
	};
	Wezel *pocz;
	Enemy_list(){
		pocz = NULL;
		licznik=0;
	}
	void dodaj(int x,int enemy_type){
		licznik++;
		Wezel *nowy;
		nowy = (struct Wezel*) malloc (sizeof(struct Wezel)); // rezerwuje pamiec dla wezla i dodaje odpowiednie dane do komorek
		nowy->E.id = licznik;
		nowy->E.x = x;
		nowy->E.y = -20;
		nowy->E.hp = enemy_type*150;
		nowy->E.type = enemy_type;
		nowy->next = pocz;
		pocz = nowy;
	}
	void usun(int id){
		if(!pocz){ 
			licznik=0; // jesli lista jest pusta to licznik = 0; 
			return; // wychodze bo nie mam nic do usuniecia
		}
		Wezel *tmp;
		tmp = pocz;
		if(pocz->E.id == id){
		pocz=pocz->next;
		free(tmp);
		}
		else{
			while(tmp->next->E.id!=id){
				tmp=tmp->next;
			}
			Wezel *usuwany;
			usuwany = tmp->next;
			tmp->next=tmp->next->next;
			
		}
		if(pocz==NULL) licznik=0; // jesli po suunieciu lista jest pusta to zeruje licznik
	}
	void usun_all(){
		Wezel *tmp;
		while(pocz){
			tmp = pocz;
			pocz=pocz->next;
			free(tmp);
		}
		licznik=0;
	}
	void move_and_refresh(){
		if(pocz==NULL) return;
		Wezel *tmp;
		tmp = pocz; 
		while(tmp){
			if(tmp->E.type==1){
				tmp->E.y+=prop_y+0.5;
				draw_trans_sprite(bufor, e1_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==2){
				tmp->E.y+=prop_y*2+0.5;
				draw_trans_sprite(bufor, e2_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==3){
				tmp->E.y+=prop_y*3+0.5;
				draw_trans_sprite(bufor, e3_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==4){
				tmp->E.y+=prop_y*2+0.5;
				draw_trans_sprite(bufor, e4_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==5){
				tmp->E.y+=prop_y*4+0.5;
				draw_trans_sprite(bufor, e5_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==6){
				tmp->E.y+=prop_y*4+0.5;
				draw_trans_sprite(bufor, e6_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==7){ // inaczej latajacy enemy3
				tmp->E.y+=prop_y*3+0.5;
				tmp->E.x+=prop_x*2+0.5;
				draw_trans_sprite(bufor, e3_gfx, tmp->E.x, tmp->E.y);
			}
			if(tmp->E.type==0){
				tmp->E.y=scr.y+50;
			}
			if((tmp->E.y) > scr.y) usun(tmp->E.id);
			tmp=tmp->next;
		}
	}
};

class Bullet{
	friend class Manage;
	friend class Bullet_list;
	int id,x,y,type,damage,who;
};

class Bullet_list{
	friend class Manage;
	int licznik;
	struct Wezel{
		Bullet B;
		Wezel *next;
	};
	Wezel *pocz;
	Bullet_list(){
		pocz = NULL;
		licznik=0;
	}
	void dodaj(int x,int y, int bullet_type, int who){
		licznik++;
		Wezel *nowy;
		nowy = (struct Wezel*) malloc (sizeof(struct Wezel)); // rezerwuje pamiec dla wezla i dodaje odpowiednie dane do komorek
		nowy->B.id = licznik;
		nowy->B.x = x;
		nowy->B.y = y;
		nowy->B.type = bullet_type;
		nowy->B.damage = bullet_type*2;
		nowy->B.who = who;
		nowy->next = pocz;
		pocz = nowy;
	}
	void usun(int id){
		if(pocz==NULL){ 
			licznik=0; // jesli lista jest pusta to licznik = 0; 
			return; // wychodze bo nie mam nic do usuniecia
		}
		Wezel *tmp;
		tmp = pocz;
		if(pocz->B.id == id){
		pocz=pocz->next;
		free(tmp);
		}
		else{
			while(tmp->next->B.id!=id){
				tmp=tmp->next;
			}
			Wezel *usuwany;
			usuwany = tmp->next;
			tmp->next=tmp->next->next;
		}
		if(pocz==NULL) licznik=0;
	}
	void usun_all(){
		Wezel *tmp;
		while(pocz){
			tmp = pocz;
			pocz=pocz->next;
			free(tmp);
		}
		licznik = 0;
	}
	void move_and_refresh(){ // porusza bulletami jeśli who=0 bullety player,a who=1 enemy - inaczej okresla kierunek poruszania bulletu
		if(!pocz) return;
		Wezel *tmp;
		tmp = pocz; 
		while(tmp){ // zarzadza szybkoscia poruszania sie pociskow
			if(tmp->B.who==0){
				if(tmp->B.type==1){
					tmp->B.y-=prop_y*9+0.5;
					draw_trans_sprite(bufor, b1_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==2){
					tmp->B.y-=prop_y*10+0.5;
					draw_trans_sprite(bufor, b2_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==3){
					tmp->B.y-=prop_y*11+0.5;
					draw_trans_sprite(bufor, b3_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==4){
					tmp->B.y-=prop_y*12+0.5;
					draw_trans_sprite(bufor, b4_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==5 || tmp->B.type==6){
					tmp->B.y-=prop_y*12+0.5;
					if(tmp->B.type==5) tmp->B.x-=2;
					else tmp->B.x+=2;
					draw_trans_sprite(bufor, b4_gfx, tmp->B.x, tmp->B.y);
				}
			}
			else{
				if(tmp->B.type==11){ // spada w dol
					tmp->B.y+=prop_y*5+0.5;
					draw_trans_sprite(bufor, b11_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==12){ // w dol 
					tmp->B.y+=prop_y*6+0.5;
					draw_trans_sprite(bufor, b12_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==13){
					tmp->B.y+=prop_y*6+0.5;
					tmp->B.x-=2; // w lewo/pod katem
					draw_trans_sprite(bufor, b13_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==14){ 
					tmp->B.y+=prop_y*6+0.5;
					tmp->B.x+=2; // w prawo/pod katem
					draw_trans_sprite(bufor, b14_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==15){ // w lewo/nie spada
					tmp->B.x-=prop_x*5+0.5;
					draw_trans_sprite(bufor, b15_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==16){ // w prawo/nie spada
					tmp->B.x+=prop_x*5+0.5;
					draw_trans_sprite(bufor, b16_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==17){ // do gory
					tmp->B.y-=prop_y*5+0.5;
					draw_trans_sprite(bufor, b17_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==18){ // w dol, podluzna grafika
					tmp->B.y+=prop_y*10+0.5;
					draw_trans_sprite(bufor, b18_gfx, tmp->B.x, tmp->B.y);
				}
				if(tmp->B.type==19){ // kulka - autonaprowadzająca z czasem
					tmp->B.y+=prop_y*10+0.5; // do napisania 
					draw_trans_sprite(bufor, b19_gfx, tmp->B.x, tmp->B.y);
				}
			}
			if(tmp->B.y+10 < 0 || tmp->B.y>scr.y || tmp->B.x+50<0 || tmp->B.x>scr.x || tmp->B.type==0) usun(tmp->B.id); // czyszcze bullety z poza ekranu
			tmp=tmp->next;
		}
	}
};

class Bonus{
	friend class Bonus_list;
	friend class Manage;
	int id,x,y,type;
};

class Bonus_list{
	friend class Manage;
	int licznik;
	struct Wezel{
		Bonus bonus;
		Wezel *next;
	};
	Wezel *pocz;
	Bonus_list(){
		pocz = NULL;
		licznik=0;
	}
	void dodaj(int x,int type){
		licznik++;
		Wezel *nowy;
		nowy = (struct Wezel*) malloc (sizeof(struct Wezel)); // rezerwuje pamiec dla wezla i dodaje odpowiednie dane do komorek
		nowy->bonus.id = licznik;
		nowy->bonus.x = x;
		nowy->bonus.y = -20;
		nowy->bonus.type = type;
		nowy->next = pocz;
		pocz = nowy;
	}
	void usun(int id){
		if(pocz==NULL){ 
			licznik=0; // jesli lista jest pusta to licznik = 0; 
			return; // wychodze bo nie mam nic do usuniecia
		}
		Wezel *tmp;
		tmp = pocz;
		if(pocz->bonus.id == id){
			pocz=pocz->next;
			free(tmp);
		}
		else{
			while(tmp->next->bonus.id!=id){
				tmp=tmp->next;
			}
			Wezel *usuwany;
			usuwany = tmp->next;
			tmp->next=tmp->next->next;
		}
		if(pocz==NULL) licznik=0;
	}
	void usun_all(){
		Wezel *tmp;
		while(pocz){
			tmp = pocz;
			pocz=pocz->next;
			free(tmp);
		}
		licznik = 0;
	}
	void move_and_refresh(){
		if(pocz==NULL) return;
		Wezel *tmp;
		tmp = pocz; 
		while(tmp){
			if(tmp->bonus.type==1){
				tmp->bonus.y+=prop_y*2+0.5;
				draw_trans_sprite(bufor, bonus1_gfx, tmp->bonus.x, tmp->bonus.y);
			}
			if(tmp->bonus.type==2){
				tmp->bonus.y+=prop_y*3+0.5;
				draw_trans_sprite(bufor, bonus2_gfx, tmp->bonus.x, tmp->bonus.y);
			}
			if(tmp->bonus.type==3){
				tmp->bonus.y+=prop_y*3+0.5;
				draw_trans_sprite(bufor, bonus3_gfx, tmp->bonus.x, tmp->bonus.y);
			}
			if(tmp->bonus.type==4){
				tmp->bonus.y+=prop_y*2+0.5;
				draw_trans_sprite(bufor, bonus4_gfx, tmp->bonus.x, tmp->bonus.y);
			}
			if(tmp->bonus.type==0){
				tmp->bonus.y=scr.y+50;
			}
			if((tmp->bonus.y) > scr.y) usun(tmp->bonus.id);
			tmp=tmp->next;
		}
	}
};

class Manage{ // wiekszosc funkcji potrzebnych do wyzej wypisanych klas
	friend class Engine;
	//Resolution scr;
	Player p1,p2;
	Enemy_list E_list;
	Bullet_list B_list;
	Bonus_list Bon_list;
	int trigger;
	Manage(){
		E_list.pocz = NULL;
		B_list.pocz = NULL;
		Bon_list.pocz = NULL;
		srand ((int)time(NULL));
		trigger=(-1);
	}
	void keys(int freq){
		if(mouse_x>=0 && mouse_x+p1_gfx->w <= scr.x) p1.x = mouse_x;
		if(mouse_y>=30 && mouse_y+p1_gfx->h <= scr.y) p1.y = mouse_y;
		if(key[KEY_1]) p1.weapon = 1;
		if(key[KEY_2]) p1.weapon = 2;
		if(key[KEY_3]) p1.weapon = 3;
		if(key[KEY_4]) p1.weapon = 4;
		if(key[KEY_5]) p1.weapon = 5;
		if(key[KEY_E]) E_list.dodaj((rand()%(scr.x-100))+50,rand()%7+1);
		if(key[KEY_B]) Bon_list.dodaj((rand()%(scr.x-100))+50,rand()%4+1);
		if(mouse_b) fire(freq,p1);
	}
	void keys_multi(int freq){
		if(key[KEY_E]) E_list.dodaj((rand()%(scr.x-100))+50,rand()%6+1);
		if(key[KEY_B]) Bon_list.dodaj((rand()%(scr.x-100))+50,rand()%4+1);
		// Player 1 sterowanie
		if(key[KEY_A] && p1.x-5 >= 0) p1.x -=10;
		if(key[KEY_D] && 5+p1.x+p1_gfx->w < scr.x) p1.x +=10;
		if(key[KEY_W] && p1.y-5 >= 30) p1.y -=10;
		if(key[KEY_S] && 5+p1.y+p1_gfx->h <= scr.y) p1.y +=10;
		if(key[KEY_R]) fire(freq,p1);
		// Player 2 sterowanie
		if(key[KEY_LEFT] && p2.x-5 >= 0) p2.x -=10;
		if(key[KEY_RIGHT] && 5+p2.x+p2_gfx->w < scr.x) p2.x +=10;
		if(key[KEY_UP] && p2.y-5 >= 30) p2.y -=10;
		if(key[KEY_DOWN] && 5+p2.y+p2_gfx->h <= scr.y) p2.y +=10;
		if(key[KEY_L]) fire(freq,p2);
	}
	void single_values(){
		p1.start_val();
	}
	void multi_values(){
		p1.start_val();
		p2.start_val();
		p1.x=300; 
		p2.x=scr.x-300;
	}
	void usun_all(){
		E_list.usun_all();
		B_list.usun_all();
		Bon_list.usun_all();
	}
	void refresh(){
		clear_to_color(bufor, makecol(0,0,0));
		stretch_blit(back,bufor, 0,0,back->w,back->h,0,0,scr.x,scr.y);
		E_list.move_and_refresh();
		B_list.move_and_refresh();
		Bon_list.move_and_refresh();
		draw_trans_sprite(bufor, p1_gfx, p1.x, p1.y);
		hud();
		blit(bufor,screen,0,0,0,0,scr.x,scr.y);
	}
	void refresh_multi(){
		clear_to_color(bufor, makecol(0,0,0));
		stretch_blit(back,bufor, 0,0,back->w,back->h,0,0,scr.x,scr.y);
		E_list.move_and_refresh();
		B_list.move_and_refresh();
		Bon_list.move_and_refresh();
		if(p1.lifes>0 && p1.hp>0) draw_trans_sprite(bufor, p1_gfx, p1.x, p1.y);
		if(p2.lifes>0 && p2.hp>0) draw_trans_sprite(bufor, p2_gfx, p2.x, p2.y);
		hud_multi();
		blit(bufor,screen,0,0,0,0,scr.x,scr.y);
	}
	void check_life(Player &p){
		if(p.hp <= 0 && p.lifes>0){
			p.lifes--;
			if(p.lifes>0) p.hp=100;
		}
	}
	void hud(){
		textprintf_ex(bufor,font,10,10,makecol(255,255,255),-1,"Lifes left: %d",p1.lifes);
		textprintf_ex(bufor,font,10,20,makecol(255,255,255),-1,"Points: %d",p1.points);
		textprintf_ex(bufor,font,scr.x-150,10,makecol(255,255,255),-1,"Level: Brak danych");
		textprintf_ex(bufor,font,scr.x-150,20,makecol(255,255,255),-1,"Weapon type: %d",p1.weapon);
		check_life(p1);
		masked_blit(hp_bar_gfx, bufor,0,0, 400-(hp_bar_gfx->w/2), 10, hp_bar_gfx->w*(0.01*p1.hp), hp_bar_gfx->h);
	}
	void hud_multi(){
		textprintf_ex(bufor,font,10,10,makecol(255,255,255),-1,"P1 Lifes left: %d",p1.lifes);
		textprintf_ex(bufor,font,10,20,makecol(255,255,255),-1,"P2 Lifes left: %d",p2.lifes);
		textprintf_ex(bufor,font,10,30,makecol(255,255,255),-1,"P1 Points: %d",p1.points);
		textprintf_ex(bufor,font,10,40,makecol(255,255,255),-1,"P2 Points: %d",p2.points);
		textprintf_ex(bufor,font,scr.x-150,10,makecol(255,255,255),-1,"Level: Brak danych");
		textprintf_ex(bufor,font,scr.x-150,20,makecol(255,255,255),-1,"P1 Weapon type: %d",p1.weapon);
		textprintf_ex(bufor,font,scr.x-150,30,makecol(255,255,255),-1,"P2 Weapon type: %d",p2.weapon);
		check_life(p1);
		check_life(p2);
		if(p1.hp>0) masked_blit(hp_bar_gfx, bufor,0,0, 400-(hp_bar_gfx->w/2),10, hp_bar_gfx->w*(0.01*p1.hp), hp_bar_gfx->h);
		if(p2.hp>0) masked_blit(hp_bar_gfx, bufor,0,0, 400-(hp_bar_gfx->w/2),50, hp_bar_gfx->w*(0.01*p2.hp), hp_bar_gfx->h);
	}
	void end_message(){
		textprintf_ex(bufor,font,100,100,makecol(255,255,255),0,"Game over!");
		textprintf_ex(bufor,font,100,120,makecol(255,255,255),0,"Your points result in single player mode is:");
		textprintf_ex(bufor,font,100,150,makecol(255,255,255),0,"%d",p1.points);
		blit(bufor,screen,0,0,0,0,scr.x,scr.y);
		sleep(2);
	}
	void end_message_multi(){
		textprintf_ex(bufor,font,100,100,makecol(255,255,255),0,"Game over!");
		textprintf_ex(bufor,font,100,120,makecol(255,255,255),0,"Your points result in multi player mode is:");
		textprintf_ex(bufor,font,100,150,makecol(255,255,255),0,"%d",p1.points+p2.points);
		blit(bufor,screen,0,0,0,0,scr.x,scr.y);
		sleep(2);
	}
	void fire(int freq, Player &p){
		if (p.lifes <=0 && p.hp <=0) return;
		int half=((p1_gfx->w)/2)-2;
		if(p.weapon == 1 && freq%5==0) B_list.dodaj(p.x+half,p.y,1,0);
		else if(p.weapon == 2 && freq%5==0){
			B_list.dodaj(p.x+half-22,p.y,1,0);
			B_list.dodaj(p.x+half+22,p.y,1,0);
		}
		else if(p.weapon == 3 && freq%5==0){
			B_list.dodaj(p.x+half-22,p.y,1,0);
			B_list.dodaj(p.x+half+22,p.y,1,0);
			if(freq%10==0) B_list.dodaj(p.x+half,p.y,2,0);
		}
		else if(p.weapon == 4 && freq%5==0){
			B_list.dodaj(p.x+half-22,p.y,1,0);
			B_list.dodaj(p.x+half+22,p.y,1,0);
			if(freq%10==0) B_list.dodaj(p.x+half,p.y,2,0);
			if(p.bullet_side==0)B_list.dodaj(p.x+half-30,p.y,3,0);
			else if(p.bullet_side==1) B_list.dodaj(p.x+half+30,p.y,3,0);
			if(p.bullet_side) p.bullet_side=0;
			else p.bullet_side=1;
		}
		else if(p.weapon == 5 && freq%5==0){
			B_list.dodaj(p.x+half-22,p.y,1,0);
			B_list.dodaj(p.x+half+22,p.y,1,0);
			if(freq%10==0) B_list.dodaj(p.x+half,p.y,2,0);
			if(p.bullet_side==0) B_list.dodaj(p.x+half-30,p.y,3,0);
			else if(p.bullet_side==1) B_list.dodaj(p.x+half+30,p.y,3,0);
			if(p.bullet_side) p.bullet_side=0;
			else p.bullet_side=1;
			// Strzelam pod kątem
			if(freq%15==0) B_list.dodaj(p.x+half,p.y,5,0);
			if(freq%15==0) B_list.dodaj(p.x+half,p.y,6,0);
		}
	}
	void collisions(){
		player_enemy(p1);
		pb_enemy();
		eb_player(p1);
		player_bonus(p1);
	}
	void collisions_multi(){
		player_enemy(p1);
		player_enemy(p2);
		pb_enemy(); 
		eb_player(p1);
		eb_player(p2);
		player_bonus(p1);
		player_bonus(p2);
	}
	void player_enemy(Player &p){ // wpisujac inne wspolrzedne
		if(E_list.pocz==NULL || (p.lifes <=0 && p.hp <=0)) return;
		Enemy_list::Wezel *tmp;
		tmp = E_list.pocz;
		while(tmp){
			if(tmp->E.type==1) e_gfx = e1_gfx;
			else if(tmp->E.type==2) e_gfx = e2_gfx;
			else if(tmp->E.type==3) e_gfx = e3_gfx;
			else if(tmp->E.type==4) e_gfx = e4_gfx;
			else if(tmp->E.type==5) e_gfx = e5_gfx;
			else if(tmp->E.type==6) e_gfx = e6_gfx;
			else if(tmp->E.type==7) e_gfx = e3_gfx;
			if(p.x+p1_gfx->w >= tmp->E.x && p.x <= tmp->E.x + e_gfx->w && p.y <= tmp->E.y + e_gfx->h && p.y + p1_gfx->h >= tmp->E.y){
				p.hp-=50;
				tmp->E.type=0;
				return;
			}
			tmp=tmp->next;
		}
	}
	void pb_enemy(){
		Enemy_list::Wezel *e_tmp;
		Bullet_list::Wezel *b_tmp;
		if(B_list.pocz==NULL && E_list.pocz==NULL) return;
		e_tmp = E_list.pocz;
		b_tmp = B_list.pocz;
		while(e_tmp){
			b_tmp=B_list.pocz;
			if(e_tmp->E.type==1) e_gfx = e1_gfx;
			else if(e_tmp->E.type==2) e_gfx = e2_gfx;
			else if(e_tmp->E.type==3) e_gfx = e3_gfx;
			else if(e_tmp->E.type==4) e_gfx = e4_gfx;
			else if(e_tmp->E.type==5) e_gfx = e5_gfx;
			else if(e_tmp->E.type==6) e_gfx = e6_gfx;
			else if(e_tmp->E.type==7) e_gfx = e3_gfx;
			while(b_tmp){
				if(b_tmp->B.type==1) b_gfx = b1_gfx;
				else if(b_tmp->B.type==2) b_gfx = b2_gfx;
				else if(b_tmp->B.type==3) b_gfx = b3_gfx;
				else if(b_tmp->B.type==4) b_gfx = b4_gfx;
				if(b_tmp->B.who==0 && b_tmp->B.y <= e_tmp->E.y+e_gfx->h && b_tmp->B.y+b1_gfx->h >= e_tmp->E.y && b_tmp->B.x<=e_tmp->E.x+e_gfx->w && b_tmp->B.x+b1_gfx->w >= e_tmp->E.x){ // ograniczenie poziomie
						e_tmp->E.hp-=b_tmp->B.damage*10;
						b_tmp->B.type=0;
						if(e_tmp->E.hp <= 0 && e_tmp->E.type>0){
							p1.points+=e_tmp->E.type*10;
							e_tmp->E.type=0;
						}
				}
				b_tmp=b_tmp->next;
			}
			e_tmp=e_tmp->next;
		}
	}
	void eb_player(Player &p){
		Bullet_list::Wezel *b_tmp;
		if(B_list.pocz==NULL || (p.lifes <=0 && p.hp <=0)) return;
		b_tmp = B_list.pocz;
		while(b_tmp){
			if(b_tmp->B.type==11 || b_tmp->B.type==15 || b_tmp->B.type==16 || b_tmp->B.type==17) b_gfx = b1_gfx;
			else if(b_tmp->B.type==12) b_gfx = b2_gfx;
			else if(b_tmp->B.type==13 || b_tmp->B.type==14) b_gfx = b3_gfx;
			else if(b_tmp->B.type==18) b_gfx = b18_gfx;
			if(b_tmp->B.type>10 && b_tmp->B.y+b_gfx->h >= p.y && b_tmp->B.y <= p.y+p1_gfx->h && b_tmp->B.x+b_gfx->w >= p.x && b_tmp->B.x <= p.x+p1_gfx->w){
				p.hp-=(b_tmp->B.type-10)*2;
				b_tmp->B.type = 0;
			}
			b_tmp=b_tmp->next;
		}
	}
	void player_bonus(Player &p){
		if(Bon_list.pocz==NULL) return;
		Bonus_list::Wezel *tmp;
		tmp = Bon_list.pocz;
		while(tmp){
			if(tmp->bonus.type==1) bonus_gfx = bonus1_gfx;
			else if(tmp->bonus.type==2) bonus_gfx = bonus2_gfx;
			else if(tmp->bonus.type==3) bonus_gfx = bonus3_gfx;
			else if(tmp->bonus.type==4) bonus_gfx = bonus4_gfx;
			if(p.x+p1_gfx->w >= tmp->bonus.x && p.x <= tmp->bonus.x+bonus_gfx->w && p.y <= tmp->bonus.y+bonus_gfx->h && p.y+p1_gfx->h >= tmp->bonus.y){
				if(tmp->bonus.type==1) p.hp=100; // leczenie
				else if(tmp->bonus.type==2) p.lifes+=1; // +1 do zycia
				else if(tmp->bonus.type==3 && p.weapon<5) p.weapon+=1; // ulepszenie broni (ulepszam do 6 typu, wiecej nie ma)
				else if(tmp->bonus.type==4){ // niespodzianka :P
					int inside_type=rand()%100+1;
					if(inside_type<=50) p.points+=10000;
					else p.lifes-=1;
				}
				tmp->bonus.type=0;
				return;
			}
			tmp=tmp->next;
		}
	}
	void enemy_fire(int freq){
		Enemy_list::Wezel *tmp;
		tmp = E_list.pocz;
		if(tmp==NULL) return;
		int half;
		if(freq==1) trigger++;
		while(tmp){
			if(tmp->E.type==1 && trigger%2==0 && freq==20){
				half=e1_gfx->w/2;
				B_list.dodaj(tmp->E.x+half,tmp->E.y+e1_gfx->h,11,1);
			}
			if(tmp->E.type==2 && trigger%3==0 && (freq==1 || freq==8)){
				half=e2_gfx->w/2;
				B_list.dodaj(tmp->E.x+half-b12_gfx->w/2+10,tmp->E.y+e2_gfx->h,12,1);
				B_list.dodaj(tmp->E.x+half-b12_gfx->w/2-10,tmp->E.y+e2_gfx->h,12,1);
			}
			if((tmp->E.type==3) && trigger%2==0 && (freq==1 || freq==10)){
				half=e3_gfx->w/2;
				B_list.dodaj(tmp->E.x+half,tmp->E.y+e3_gfx->h,13,1);
				B_list.dodaj(tmp->E.x+half,tmp->E.y+e3_gfx->h,14,1);
			}
			if(tmp->E.type==4 && trigger%2==0 && (freq==10 || freq==20)){ 
				half=e4_gfx->w/2;
				B_list.dodaj(tmp->E.x+half ,tmp->E.y+e4_gfx->h,11,1); // w dol
				B_list.dodaj(tmp->E.x, tmp->E.y+e4_gfx->h/2,15,1); // w lewo
				B_list.dodaj(tmp->E.x+half*2, tmp->E.y+e4_gfx->h/2,16,1); // w prawo
				B_list.dodaj(tmp->E.x+half, tmp->E.y,17,1); // do gory
			}
			if(tmp->E.type==5 && trigger%2==0 && freq==10){
				half=e5_gfx->w/2;
				B_list.dodaj(tmp->E.x+half-b13_gfx->w/2, tmp->E.y+e5_gfx->h,13,1);
				B_list.dodaj(tmp->E.x+half-b14_gfx->w/2, tmp->E.y+e5_gfx->h,14,1);
				B_list.dodaj(tmp->E.x+half-b18_gfx->w/2, tmp->E.y+e5_gfx->h,18,1);
			}
			if(tmp->E.type==6 && trigger%2==0 && freq==3){
				B_list.dodaj(tmp->E.x+e6_gfx->w/2-b19_gfx->w/2,tmp->E.y+e6_gfx->h/2,19,1); // autonaprowadzanie
			}
			if(tmp->E.type==7 && trigger%3==0 && freq<=6){
				B_list.dodaj(tmp->E.x,tmp->E.y+e3_gfx->h/2,15,1); // w lewo
			}
			tmp=tmp->next;
		}
		
	}
	void add_enemy(int freq){
	}
};

class Engine{ // zarzadzanie cala gra
	Manage m;
	int fps, freq;
	bool change;
	public:
	Engine(){
		scr.check_settings();
		scr.load_settings();
		prop_x = scr.x/800;
		prop_y = scr.y/600;
		alpng_init();
		allegro_init();
		set_alpha_blender();
		install_timer();
		install_mouse();
		set_mouse_speed(1,1);
		install_keyboard();
		set_color_depth(32);
		set_gfx_mode(scr.mode,scr.x,scr.y,0,0);
		set_palette(default_palette);
		bufor = create_bitmap(scr.x,scr.y);
		p1_gfx = load_png("grafika/p1.png",default_palette);
		p2_gfx = load_png("grafika/p2.png",default_palette);
		b1_gfx = load_png("grafika/b1.png",default_palette);
		b2_gfx = load_png("grafika/b2.png",default_palette);
		b3_gfx = load_png("grafika/b3.png",default_palette);
		b4_gfx = load_png("grafika/b4.png",default_palette);
		b11_gfx = load_png("grafika/b11.png",default_palette); 
		b12_gfx = load_png("grafika/b12.png",default_palette);
		b13_gfx = load_png("grafika/b13.png",default_palette);
		b14_gfx = load_png("grafika/b14.png",default_palette);
		b15_gfx = load_png("grafika/b15.png",default_palette);
		b16_gfx = load_png("grafika/b16.png",default_palette);
		b17_gfx = load_png("grafika/b17.png",default_palette);
		b18_gfx = load_png("grafika/b18.png",default_palette);
		b19_gfx = load_png("grafika/b19.png",default_palette);
		e1_gfx = load_png("grafika/e1.png",default_palette);
		e2_gfx = load_png("grafika/e2.png",default_palette);
		e3_gfx = load_png("grafika/e3.png",default_palette);
		e4_gfx = load_png("grafika/e4.png",default_palette);
		e5_gfx = load_png("grafika/e5.png",default_palette);
		e6_gfx = load_png("grafika/e6.png",default_palette);
		bonus_gfx = load_png("grafika/bonus1.png",default_palette);
		bonus1_gfx = load_png("grafika/bonus1.png",default_palette);
		bonus2_gfx = load_png("grafika/bonus2.png",default_palette);
		bonus3_gfx = load_png("grafika/bonus3.png",default_palette);
		bonus4_gfx = load_png("grafika/bonus4.png",default_palette);
		back = load_png("grafika/back.png",default_palette);
		menu_gfx = load_png("grafika/menu.png",default_palette);
		item_gfx = load_png("grafika/item.png",default_palette);
		hp_bar_gfx = load_tga("grafika/hp_bar.tga",default_palette);
		fps = 60;
		freq = 0;
		change = false;
		menu();
	}
	~Engine(){
		m.usun_all();
		destroy_bitmap(bufor);
		destroy_bitmap(p1_gfx);
		destroy_bitmap(p2_gfx);
		destroy_bitmap(b1_gfx);
		destroy_bitmap(b2_gfx);
		destroy_bitmap(b3_gfx);
		destroy_bitmap(b4_gfx);
		destroy_bitmap(b11_gfx);
		destroy_bitmap(b12_gfx);
		destroy_bitmap(b13_gfx);
		destroy_bitmap(b14_gfx);
		destroy_bitmap(b15_gfx);
		destroy_bitmap(b16_gfx);
		destroy_bitmap(b17_gfx);
		destroy_bitmap(b18_gfx);
		destroy_bitmap(b19_gfx);
		destroy_bitmap(hp_bar_gfx);
		destroy_bitmap(e1_gfx);
		destroy_bitmap(e2_gfx);
		destroy_bitmap(e3_gfx);
		destroy_bitmap(e4_gfx);
		destroy_bitmap(e5_gfx);
		destroy_bitmap(e6_gfx);
		destroy_bitmap(bonus1_gfx);
		destroy_bitmap(bonus2_gfx);
		destroy_bitmap(bonus3_gfx);
		destroy_bitmap(bonus4_gfx);
		destroy_bitmap(menu_gfx);
		destroy_bitmap(item_gfx);
		//destroy_bitmap(b_gfx);
		//destroy_bitmap(e_gfx);
		allegro_exit();
	}
	void menu(){
		int menu_item=0;
		while(!key[KEY_ESC] && !change){
			rest(1000/20);
			clear_to_color(bufor, makecol(0,0,0));
			stretch_blit(menu_gfx,bufor, 0,0,menu_gfx->w,menu_gfx->h,0,0,scr.x,scr.y);
			if(key[KEY_UP]){
				menu_item--;
				if(menu_item<0) menu_item=5;
			}
			if(key[KEY_DOWN]){
				menu_item++;
				if(menu_item>5) menu_item=0;
			}
			if(key[KEY_ENTER]){
				if(menu_item==0) single_mode();
				if(menu_item==1) multi_mode();
				if(menu_item==2) settings();
				if(menu_item==5) break;
				rest(50);
			}
			if(menu_item==0) draw_trans_sprite(bufor, item_gfx, 580*scr.x/1024, 275*scr.y/768);
			else if(menu_item==1) draw_trans_sprite(bufor, item_gfx, 525*scr.x/1024, 355*scr.y/768);
			else if(menu_item==2) draw_trans_sprite(bufor, item_gfx, 490*scr.x/1024, 430*scr.y/768);
			else if(menu_item==3) draw_trans_sprite(bufor, item_gfx, 480*scr.x/1024, 515*scr.y/768);
			else if(menu_item==4) draw_trans_sprite(bufor, item_gfx, 490*scr.x/1024, 590*scr.y/768);
			else if(menu_item==5) draw_trans_sprite(bufor, item_gfx, 510*scr.x/1024, 680*scr.y/768);
			rest(10);
			blit(bufor,screen,0,0,0,0,scr.x,scr.y);
		}
		/*while(!key[KEY_ESC] && !change){
			rest(1000/20);
			clear_to_color(bufor, makecol(0,0,0));
			textprintf_ex(bufor,font,10,10,makecol(200,200,200),-1,"1) Signle player");
			textprintf_ex(bufor,font,10,30,makecol(200,200,200),-1,"2) Multi player");
			textprintf_ex(bufor,font,10,50,makecol(200,200,200),-1,"3) Options");
			textprintf_ex(bufor,font,10,70,makecol(200,200,200),-1,"4) Help");
			textprintf_ex(bufor,font,10,90,makecol(200,200,200),-1,"5) About");
			textprintf_ex(bufor,font,10,110,makecol(200,200,200),-1,"ESC) Exit");
			blit(bufor,screen,0,0,0,0,scr.x,scr.y);
			if(key[KEY_1]) single_mode();
			if(key[KEY_2]) multi_mode();
			if(key[KEY_3]) settings();
		}*/
	}
	void single_mode(){
		m.single_values(); 
		while(!key[KEY_Q] && m.p1.lifes>0){
			rest(1000/fps);
			if(freq==31) freq=0;
			freq++;
			m.keys(freq);
			m.refresh();
			m.collisions();
			m.enemy_fire(freq);
			m.add_enemy(freq);
		}
		m.end_message();
		m.usun_all();
	}
	void multi_mode(){
		m.multi_values();
		while(!key[KEY_Q] && (m.p1.lifes>0 || m.p2.lifes>0)){
			rest(1000/fps);
			if(freq==31) freq=0;
			freq++;
			m.keys_multi(freq); // done
			m.refresh_multi(); // done 
			m.collisions_multi(); // done
			m.enemy_fire(freq); // done
			m.add_enemy(freq);
		}
		m.end_message_multi();
		m.usun_all();
	}
	void settings(){
		clear_to_color(bufor, makecol(0,0,0));
		textprintf_ex(bufor,font,10,10,makecol(200,200,200),-1,"Resolution:");
		textprintf_ex(bufor,font,10,30,makecol(200,200,200),-1,"F1) 800x600  | windowed mode");
		textprintf_ex(bufor,font,10,50,makecol(200,200,200),-1,"F2) 1024x768 | windowed mode");
		textprintf_ex(bufor,font,10,70,makecol(200,200,200),-1,"F3) 800x600  | full screen mode");
		textprintf_ex(bufor,font,10,90,makecol(200,200,200),-1,"F4) 1024x768 | full screen mode");
		textprintf_ex(bufor,font,10,110,makecol(200,200,200),-1,"F5) 1240x800 | full screen mode");
		textprintf_ex(bufor,font,10,130,makecol(200,200,200),-1,"ESC) Back");
		textprintf_ex(bufor,font,10,200,makecol(200,200,200),-1,"Restart game after changing resolution!");
		blit(bufor,screen,0,0,0,0,scr.x, scr.y);
		while(!key[KEY_ESC]){
			rest(1000/fps);
			if(key[KEY_F1]){
				scr.save_settings(2,800,600);
				change=true;
				break;
			}
			if(key[KEY_F2]){
				scr.save_settings(2,1024,768);
				change=true;
				break;
			}
			if(key[KEY_F3]){
				scr.save_settings(1,800,600);
				change=true;
				break;
			}
			if(key[KEY_F4]){
				scr.save_settings(1,1024,768);
				change=true;
				break;
			}
			if(key[KEY_F5]){
				scr.save_settings(1,1240,800);
				change=true;
				break;
			}
		}
		if(change){
			clear_to_color(bufor, makecol(0,0,0));
			textprintf_ex(bufor,font,10,10,makecol(200,200,200),-1,"After changing settings game will shutdown by it self");
			textprintf_ex(bufor,font,10,30,makecol(200,200,200),-1,"Do not panic, it's normal ;)");
			textprintf_ex(bufor,font,10,70,makecol(200,200,200),-1,"If you want to play just lauch the game again.");
			textprintf_ex(bufor,font,10,120,makecol(200,200,200),-1,"Any key will imedially shutdown game...");
			blit(bufor,screen,0,0,0,0,scr.x, scr.y);
			clear_keybuf();
			readkey();
		}
		else
			rest(100);
	}
};

#include "RaycastWindow.h"
#include <cmath>
#include <stdexcept>
#include <SDL2/SDL_image.h>

double const pi = acos (-1);

constexpr float e = 0.01;
constexpr int MAP_W = 15, MAP_H = 10;
const char map[] =
		"###############"
		"#    #    #   #"
		"# ####    #   #"
		"#             #"
		"# ###     #   #"
		"# #       #   #"
		"# #   #####   #"
		"# #####     # #"
		"#       ##### #"
		"###############";

float dX, dY, rX, rY, Dh, Dv, D, t, tW, tH, cosB, sinB, alphaR, gammaR, betaR, d, speedX, speedY;
float x = 7, y = 5, alpha = 90, speed = 0.1;
int h, col, colRaycastWindow, RaycastWindow_w, RaycastWindow_h;

void raycasting(float &rX, float &rY, float dX, float dY){
	do{
		rX += dX;
		rY += dY;
	} while (map[(int)rX + (int)rY * MAP_W] != '#' && ((0<=rY) && (rY<=MAP_H) && (0<=rX) && (rX<=MAP_W)));
}


RaycastWindow::RaycastWindow(int width, int height)
: Window(width, height),
  _FOV(75), _d0(width / (2*tan(_FOV/2 * pi /180))),
  _map(width*height/4), _rect_map(),
  _rect_map_w((width/6)/MAP_W), _rect_map_h((height/6)/MAP_H), _cols_in_rect(width / MAP_W)
{

	_texture = std::shared_ptr<SDL_Texture>(
			IMG_LoadTexture(_renderer.get(), "texture.png"),
					SDL_DestroyTexture);
	if (_texture == nullptr)
			throw std::runtime_error(
					std::string("Не удалось загрузить текстуры: ") +
					std::string(SDL_GetError()));
	SDL_QueryTexture(_texture.get(), nullptr, nullptr, &RaycastWindow_w, &RaycastWindow_h);
}

void RaycastWindow::render()
{
	SDL_SetRenderDrawColor(_renderer.get(), 200, 200, 200, 255);
	SDL_RenderClear(_renderer.get());
	_rect_map.w = width();
	_rect_map.h = height()/2;
	_rect_map.x = 0;
	_rect_map.y = 0;
	SDL_SetRenderDrawColor(_renderer.get(), 125, 125, 125, 255);
	SDL_RenderFillRect(_renderer.get(), &_rect_map);

	for (col = 0; col < width(); ++col){
		gammaR = atan2(col - width()/2, _d0);
		alphaR = alpha * pi / 180;
		betaR = gammaR + alphaR;

		cosB = cos(betaR);
		sinB = sin(betaR);
		if (cosB > e){
			rX = floor(x) + e;
			dX = 1;
			dY = tan(betaR);
			rY = y - (x - rX) * dY;
		}
		else if (cosB < -e){
			rX = ceil(x) - e;
			dX = -1;
			dY = tan(-betaR);
			rY = y - (rX - x) * dY;
		}
		raycasting(rX, rY, dX, dY);

		tH = rY - floor(rY);

		if (((-e <= cosB) && (cosB <= e)) || (rX < 0) || (rX > MAP_W) || (rY < 0) || (rY > MAP_H))
			Dv = width()*3;
		else
			Dv = hypot(rX - x, rY - y);

		if (sinB > e){
			dY = 1;
			rY = floor(y) + e;
			dX = 1/tan(betaR);
			rX = x - (y - rY) * dX;
		}
		else if (sinB < -e){
			dY = -1;
			rY = ceil(y) - e;
			dX =1/ tan(-betaR);
			rX = x - (rY - y) * dX;
		}
		raycasting(rX, rY, dX, dY);



		if ((( -e <= sinB) && (sinB <= e)) || (rX < 0) || (rX > MAP_W) || (rY < 0) || (rY > MAP_H))
					Dh= height()*3;
				else
					Dh = hypot(rX - x, rY - y);

		D = std::min(Dh, Dv);
		d = _d0 / cos(gammaR);
		h = d / D;
		tW = rX-floor(rX);
		if (D == Dh) t = tW;
		else t = tH;

		colRaycastWindow =  col % _cols_in_rect;

		_rect_part.x = floor (RaycastWindow_w * t);
		_rect_part.y = 0;
		_rect_part.w = 1;
		_rect_part.h = RaycastWindow_h;
		SDL_Rect rect_walls{col, height()/2 - h/2, 1, h};
		SDL_RenderCopy(_renderer.get(), _texture.get(), &_rect_part, &rect_walls);

	}
		for (unsigned i = 0; i<MAP_H; i++){
			for (unsigned j = 0; j<MAP_W; j++){
				_rect_map.w = std::max(_rect_map_w, _rect_map_h);
				_rect_map.h = std::max(_rect_map_w, _rect_map_h);
				_rect_map.x = _rect_map.w * j;
				_rect_map.y = _rect_map.h * i;
				SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 220);
				if ((i == (unsigned)y) && (j == (unsigned)x) && (map[j+i*MAP_W]==' ')){
					SDL_SetRenderDrawColor(_renderer.get(), 255, 0, 255, 230);
					_rect_map.w -= _rect_map.w/2;
					_rect_map.h -= _rect_map.h/2;
					_rect_map.x += _rect_map.w/2;
					_rect_map.y += _rect_map.h/2;
				} else if (map[j+i*MAP_W]==' ') continue;
				SDL_RenderFillRect(_renderer.get(),&_rect_map);
			}
		}





}

void RaycastWindow::do_logic()
{

}

void RaycastWindow::handle_keys(const Uint8 *keys)
{
	if (keys[SDL_SCANCODE_UP]){
		speedX = cos(alpha * pi / 180)* speed;
		speedY = sin(alpha * pi / 180)* speed;
		if (map[(int)(floor(x+speedX))+(int)(floor(y+speedY))*MAP_W]==' '){
			x += speedX;
			y += speedY;
		}
	}
	if (keys[SDL_SCANCODE_DOWN]){
		speedX = cos(alpha * pi / 180)* speed;
		speedY = sin(alpha * pi / 180)* speed;
		if (map[(int)(floor(x-speedX))+(int)(floor(y-speedY))*MAP_W]==' '){
			x -= speedX;
			y -= speedY;
		}
	}
	if (keys[SDL_SCANCODE_LEFT]){ alpha -= 2;}
	if (keys[SDL_SCANCODE_RIGHT]){ alpha += 2;}
}




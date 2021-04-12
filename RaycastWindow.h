#ifndef RAYCASTWINDOW_H_
#define RAYCASTWINDOW_H_

#include "Window.h"
#include <vector>

class RaycastWindow: public Window
{
protected:
	int _FOV;
	float _d0;
	std::shared_ptr<SDL_Texture> _texture;
	std::vector<uint32_t> _map;
	SDL_Rect _rect_map, _rect_part;
	unsigned _rect_map_w, _rect_map_h, _cols_in_rect;
public:
	RaycastWindow(
			int width = DEFAULT_WIDTH,
			int height = DEFAULT_HEIGHT);
	virtual ~RaycastWindow() = default;

	virtual void render() override;
	virtual void do_logic() override;
	virtual void handle_keys(const Uint8 *keys) override;
};


#endif /* RAYCASTWINDOW_H_ */

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "iwidget.hpp"
#include "image.hpp"
#include "rsc_manager.hpp"


class Sprite : public IWidget {
public:
	Sprite(Image* pImg, vect2df_t vPos);
	Sprite(SpriteSheet* pSprSht, uint uFrameNb, vect2df_t vPos);
    
	uint getFrame();
	void setFrame(uint uNewFrame);

	void draw(uint8* buffer);

protected:
	uint m_rscId;
	Image* m_pImg;
	SpriteSheet* m_pSprSht;
	uint m_uFrameNb;
};

#endif

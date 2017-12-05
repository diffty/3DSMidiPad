#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "animatedsprite.hpp"

class ParticleSystem;

class Particle : public AnimatedSprite
{
private:
	vect2df_t m_vStartPos;
	vect2df_t m_vStartVel;
	int m_iCurrFrame;
	int m_iStartFrame;
	int m_iEndFrame;
	ParticleSystem* m_pParticleSys;

public:
	Particle(SpriteSheet* pSprSht, vect2df_t vStartPos, vect2df_t vStartVel, int iStartFrame, int iEndFrame, uint uFPS, ParticleSystem* pParentSystem);
	~Particle();

	void update();
};


#endif

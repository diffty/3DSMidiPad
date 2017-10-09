#include "common_types.hpp"


vect2df_t getNormalizedVect(vect2df_t vVect) {
	vect2df_t vNormalizedVect;

	if (abs(vVect.x) < abs(vVect.y)) {
		vNormalizedVect.x = vVect.x / abs(vVect.y);
		vNormalizedVect.y = vVect.y / abs(vVect.y);
	}
	else if (abs(vVect.x) > abs(vVect.y)) {
		vNormalizedVect.x = vVect.x / abs(vVect.x);
		vNormalizedVect.y = vVect.y / abs(vVect.x);
	}
	else {
		vNormalizedVect.x = vVect.x / abs(vVect.x);
		vNormalizedVect.y = vVect.y / abs(vVect.y);
	}

	return vNormalizedVect;
}
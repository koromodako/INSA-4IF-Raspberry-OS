#include <stdint.h>
#include "kheap.h"
#include "fb.h"
#include "util.h"

/* trace une ligne entre deux points
** On trace sur un x tant que l'erreur e est pas trop importante, 
** sinon, on incrémente y.
// TODO FAIRE LES AUTRES CAS (ne marche que pour la diagonale haut gauche vers bas droite)
*/
void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2){
	uint32_t dx, dy;
	int32_t e;
	e = x2 - x1;
	dx = e * 2;
	dy = (y2 - y1) * 2;
	while(x1 <= x2){
        put_pixel_RGB24(x1, y1, 255, 255, 255);
		++x1;
		e -= dy;
		if(e < 0){
			++y1;
			e = e + dx;
		}
	}
	put_pixel_RGB24(x2, y2, 255, 255, 255);
}
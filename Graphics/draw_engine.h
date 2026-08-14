/**
 *	\file draw_engine.h
 *	\brief Stellt den Bresenham-Algorithmus und das setzen der Pixel bereit
 *
 *	\date 14.12.2012
 */


#ifndef DRAW_ENGINE_H_
#define DRAW_ENGINE_H_

/**
 * \brief Bresenham-Algorithmus: Linien auf Rastergeräten zeichnen
 *
 *	Generiert eine Linie zwischen den beiden angegebenen Pixeln
 *
 * \param xstart,ystart Koordinaten des Startpunkts
 * \param xend,yend Koordinaten des Endpunkts
*/
//void gbham(uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend);
void gbham(uint8_t xstart,uint8_t ystart,uint8_t xend,uint8_t yend);


/**
 * \brief Funktion zum setzen eines Pixels auf dem Oszilloskop
 *
 * \param x,y Koordinaten des darzustellenden Pixels
*/
void SetPixel(uint8_t x,uint8_t y);

#endif /* DRAW_ENGINE_H_ */

#include "image.hpp"


Image::Image(char* fileName) {
	loadFromFile(fileName);
}

Image::~Image() {
	free(m_pImgData);
	free(m_aPalette);
}


void Image::loadFromFile(char* fileName) {
	FILE* fp;
	int fh;
	int nBytes, nBytesToRead;
	int i, j, k;

	int* imgW = (int *) malloc(4);
	int* imgH = (int *) malloc(4);
	int* imgPalSize = (int *) malloc(4);

	unsigned long* dataSize = (unsigned long *) malloc(4);
	int* startOffset = (int *) malloc(4);
	int* headerSize  = (int *) malloc(4);
	unsigned long nbPixels;

	byte* fileBuf = (byte *) malloc(FREAD_BUFFER_SIZE);
	long seekPtr;

	byte* paletteData;

	printf("Loading image %s.\n", fileName);

	/*if ((image = (image_t *) malloc(sizeof(image_t))) == NULL) {
	printf("Not enough memory forS new image allocation.\n");
	exit(1);
	}*/
    
	if ((fp = fopen(fileName, "rb")) == NULL) {
		printf("Can't read image file %s. Aborting.\n", fileName);
		exit(1);
	}
    
	// Reading header
	fseek(fp, 0x0002, SEEK_SET);
	fread(dataSize, 4, 1, fp);
	fseek(fp, 0x000A, SEEK_SET);
	fread(startOffset, 4, 1, fp);
	fseek(fp, 0x000E, SEEK_SET);
	fread(headerSize, 4, 1, fp);
	fseek(fp, 0x0012, SEEK_SET);
	fread(imgW, 4, 1, fp);
	fseek(fp, 0x0016, SEEK_SET);
	fread(imgH, 4, 1, fp);
	fseek(fp, 0x002E, SEEK_SET);
	fread(imgPalSize, 4, 1, fp);

	m_size.w = *imgW;
	m_size.h = *imgH;
	m_paletteSize = *imgPalSize;

	if (m_paletteSize == 0) m_paletteSize = 256;

	nbPixels = (long) m_size.h * (long) m_size.w;

    long rowPadding = 0;
    
    if (m_size.w % 4 != 0) rowPadding = 4 - m_size.w % 4;

	printf("Size : %ld\n", *dataSize);
	printf("Start offset : %d\n", *startOffset);
	printf("Header size : %d\n", *headerSize);
	printf("Image width : %d\n", m_size.w);
	printf("Image height : %d\n", m_size.h);
	printf("Palette size : %d\n", m_paletteSize);
	printf("Nb pixels : %d\n", nbPixels);
	printf("Padding: %d\n", rowPadding);

	// Reading palette
	paletteData = (byte *) malloc(m_paletteSize*4);

	fseek(fp, (*headerSize)+14, SEEK_SET);
	fread(paletteData, 1, m_paletteSize*4, fp);

	m_aPalette = (color_t *) malloc(sizeof(color_t) * m_paletteSize);

	for (i = 0; i < m_paletteSize; i++) {
		m_aPalette[i].b = paletteData[i*4];
		m_aPalette[i].g = paletteData[i*4+1];
		m_aPalette[i].r = paletteData[i*4+2];
	}

	// Reading pixel array
	m_pImgData = (uint8 *) malloc(nbPixels * SCREEN_BPP * sizeof(uint8));

	seekPtr = *startOffset;

	int nbPixelsWPadding = nbPixels + (m_size.h - 1) * rowPadding;
    long currPixNb = 0;
    
    long imgDataPtr = 0;
    
	while (seekPtr < (nbPixelsWPadding + (*startOffset))) {
		nBytesToRead = min((nbPixelsWPadding - (seekPtr - (*startOffset))), FREAD_BUFFER_SIZE);
        
		fseek(fp, seekPtr, SEEK_SET);
		fread(fileBuf, 1, nBytesToRead, fp);

        long fileBufSeek = 0;

        for (int i = 0; i < nBytesToRead; i++) {
			if (currPixNb % m_size.w == 0 && currPixNb > 0) {
				i += rowPadding;
			}

#ifdef TARGET_3DS
            fileBufSeek = i;
            imgDataPtr = ((currPixNb % m_size.w) * m_size.h) + (currPixNb / m_size.w);
#else
			fileBufSeek = i;
            imgDataPtr = currPixNb;
#endif
            byte currByte = fileBuf[fileBufSeek];
            
            m_pImgData[(imgDataPtr * SCREEN_BPP)]     = m_aPalette[(int)currByte].b;
			m_pImgData[(imgDataPtr * SCREEN_BPP) + 1] = m_aPalette[(int)currByte].g;
			m_pImgData[(imgDataPtr * SCREEN_BPP) + 2] = m_aPalette[(int)currByte].r;
#if TARGET_SDL
			m_pImgData[(imgDataPtr * SCREEN_BPP) + 3] = 0;
#endif
			currPixNb++;
		}

		seekPtr += nBytesToRead;
	}


	fclose(fp);

	/*
	m_mask = (u8 *) malloc(2 * sizeof(u8));
	
	// Building transparency info
	j = 0;
	k = 0;
	m_maskNbZone = 0;

	for (i = 0; i < nbPixels; i++) {
		if (m_pImgData[i] != m_pImgData[0]) {
			j++;
		}

		if (i < (nbPixels) - 1) {
			if (m_pImgData[i] != m_pImgData[i+1]) {
				if (m_pImgData[i+1] == m_pImgData[0] || i+1 == (nbPixels) - 1 || (i > 0 && i % m_size.w == 0)) {
					m_mask = (u8 *) realloc(m_mask, 2 * (m_maskNbZone + 1) * sizeof(u8));

					m_mask[k] = i-j+1;
					m_mask[k+1] = j;

					k += 2;
					j = 0;

					m_maskNbZone++;
				}
			}
		}
	}
	*/

	free(dataSize);
	free(startOffset);
	free(headerSize);
	free(fileBuf);
	free(paletteData);
	free(imgW);
	free(imgH);
	free(imgPalSize);
}

void Image::draw(uint8* buffer, int dstX, int dstY, int srcX, int srcY, int srcW, int srcH, bool reversed, bool masked) {
    int xb, yb;
    unsigned int imgBufIdx, zoneSize;
	unsigned int overflowLeft = 0, overflowRight = 0, overflowTop = 0, overflowBottom = 0;

    if (srcX >= m_size.w || srcY >= m_size.h) {
        return;
    }
    
    if (srcX >= 0 || srcY >= 0 || srcW > 0 || srcH > 0) {
        if (srcW > m_size.w || srcH > m_size.h) {
            return;
        }
        
        if ((srcX + srcW) > m_size.w) {
            srcW = m_size.w - srcX;
        }
        if ((srcY + srcH) > m_size.h) {
            srcH = m_size.h - srcY;
        }
    }
    else {
        srcW = m_size.w;
        srcH = m_size.h;
    }
    
	overflowLeft = max(0, -dstX);
	overflowRight = max(0, (dstX + srcW) - SCREEN_WIDTH);
	overflowTop = max(0, -dstY);
	overflowBottom = max(0, (dstY + srcH) - SCREEN_HEIGHT);

	// printf("%d, %d, %d, %d\n", overflowLeft, overflowRight, overflowBottom, overflowTop);

	if (overflowLeft > srcW || overflowRight > srcW || overflowTop > srcH || overflowBottom > srcH) {
		return;
	}
    
  	if (masked) {
		/*for (int i = 0; i < m_maskNbZone; i++) {
			imgBufIdx = m_mask[i*2];
			zoneSize = m_mask[i*2+1];

			xb = imgBufIdx % srcW;

			if (reversed)
				yb = imgBufIdx / m_size.w;
			else
				yb = m_size.h - (imgBufIdx / m_size.w) - 1;

			// Clipping
			if (dstX + xb + zoneSize < 0 || dstX + xb >= SCREEN_WIDTH || dstY + yb < 0 || dstY + yb >= SCREEN_HEIGHT) {
				continue;
			}
			else if (dstX + xb + zoneSize > SCREEN_WIDTH) {
				zoneSize = zoneSize - ((x + xb + zoneSize) % SCREEN_WIDTH);
			}
			else if (dstX + xb < 0) {
				imgBufIdx += -(dstX + xb);
				zoneSize = zoneSize + (dstX + xb);
				xb = imgBufIdx % m_size.w;
			}

			// ...Then current line copy
			//memcpy(buffer + (dstX + xb) + ((dstY * SCREEN_WIDTH) + (yb * SCREEN_WIDTH)),
			//       m_pImgData + imgBufIdx,
			//       zoneSize);

			// ...Then current line copy
			memcpy(buffer + ((dstY + yb) * SCREEN_BPP) + ((dstX * SCREEN_HEIGHT * SCREEN_BPP) + (xb * SCREEN_HEIGHT)),
			       m_pImgData + imgBufIdx,
			       zoneSize);
		}*/
	}
	else {
#if TARGET_3DS
		for (int i = overflowLeft; i < srcW - overflowRight; i++) {
			memcpy(buffer + ((SCREEN_HEIGHT - m_size.h - 1 - dstY) * SCREEN_BPP) + ((dstX + i) * SCREEN_HEIGHT * SCREEN_BPP),
				m_pImgData + ((i + srcX) * m_size.h * SCREEN_BPP),
				(srcH - overflowTop - overflowBottom) * SCREEN_BPP);
		}
#elif TARGET_SDL
		for (int i = overflowTop; i < srcH - overflowBottom; i++) {
			memcpy(buffer + (max(0, dstX) * SCREEN_BPP) + ((max(0, dstY) + i - overflowTop) * SCREEN_WIDTH * SCREEN_BPP),
				m_pImgData + ((m_size.h - (i + srcY) - 1) * (m_size.w * SCREEN_BPP)) + ((overflowLeft + srcX) * SCREEN_BPP),
				(srcW - overflowLeft - overflowRight) * SCREEN_BPP);
		}
#endif
	}
}

void Image::draw(uint8* buffer, int dstX, int dstY, bool reversed, bool masked) {
    draw(buffer, dstX, dstY, 0, 0, m_size.w, m_size.h, reversed, masked);
}

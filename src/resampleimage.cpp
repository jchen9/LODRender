/******************************************************************************
 * CoreWall / Corelyzer - An Initial Core Description Tool
 * Copyright (C) 2004, 2005 Arun Gangadhar Gudur Rao, Julian Yu-Chung Chen,
 * Sangyoon Lee, Electronic Visualization Laboratory, University of Illinois 
 * at Chicago
 *
 * This software is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either Version 2.1 of the License, or 
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser Public License along
 * with this software; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *****************************************************************************/

#include "utility.h"
#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif
//--------------------------------------------------------//
char* ResampleBuffer(char* oldbuffer, int bufferwidth, int bufferheight,
                    int channels, float scale, char*& newbuffer,
                    int& newWidth, int& newHeight, bool highQuality)
{
        float K = 1 / scale;
        newWidth = (int) floor(scale * bufferwidth);
        newHeight = (int) floor(scale * bufferheight);
        float   f_x, f_y, a, b;
        char *pix1, *pix2, *pix3, *pix4;
        int i_x, i_y;
        
        if(newbuffer)    
                delete [] newbuffer;
        
        newbuffer = new char[ newWidth * newHeight * channels];
//#if defined(DEBUG)
//        fprintf(stderr,"Resample called with new scale at %f\n", scale);
//        fprintf(stderr,"Initial image dim %d x %d\nAllocated image of dimensions: %d x %d x %d\n",
//           bufferwidth, bufferheight, newWidth,newHeight,channels);
//#endif
 
        //resampling using bilinear interpolation
        //      cerr << "resampling using bilinear interpolation\t" << clock() << endl
        //              << "original image size: " << ui_data.width << " x "
        //      << ui_data.height << endl
        //              << "new image size: " << newWidth << " x " << newHeight << endl;
        int j, i, c;
        
        for(j = 0; j < newHeight; j++)
        {
                f_y = (float) j * K;
                i_y = (int) floor(f_y);
                a = f_y - floor(f_y);   

                for( i = 0; i < newWidth; i++)
                {
                        f_x = (float) i * K;
                        i_x = (int) floor(f_x);
                        b = f_x - floor(f_x);
        
                        //BOUNDS CHECKING
        
                        if(highQuality)
                        {
                                pix1 = oldbuffer + (( i_x ) * channels) +
                                    (bufferwidth * (i_y) * channels);
                                pix2 = oldbuffer + (( i_x + 1) * channels) +
                                    (bufferwidth * (i_y) * channels);
                                pix3 = oldbuffer + (( i_x ) * channels) +
                                    (bufferwidth * (i_y + 1) * channels);
				pix4 = oldbuffer + (( i_x + 1) * channels) +
                                    (bufferwidth * (i_y + 1) * channels);
                        
                        }
                        
                        for(c = 0; c < channels; c++)
                        {
                                if(highQuality)
                                {
                                        newbuffer[i*channels + (j * newWidth * channels) + c] =
                                               (char) floor( (1.0F - a) * (((1.0F - b)*pix1[c]) +
                                               (b*pix2[c])) + a * (((1.0F - b)*pix3[c]) +
                                               (b*pix4[c])) ) ;
                                }
                                else
                                {
                                        newbuffer[i*channels + (j * newWidth * channels) + c] =
                                           oldbuffer[ (( i_x ) * channels) +
                                                       (bufferwidth * (i_y) * channels) + c];
                                }
                        }
                                
                }
                                        
                                               
        }
       return newbuffer;
}
   
#ifdef _cplusplus
}
#endif
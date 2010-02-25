/*
 *  scenegraph_c.cpp
 *  JustNative
 *
 *  Created by Julian on 9/1/09.
 *  Copyright 2009 CoreWall.org. All rights reserved.
 *
 */

#include "scenegraph_c.h"

using namespace std;

#include "annotationmarker.h"
#include "canvas.h"
#include "trackscene.h"
#include "textureresource_ex.h"
#include "coresection.h"
#include "model.h"
#include "dataset.h"
#include "graph.h"
#include "fontsys.h"
#include "freedraw.h"
#include <vector>
#include <iostream>
#include <math.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <Carbon/Carbon.h>
#endif

#pragma mark Utility function declarations
float get_scene_center_x();
float get_scene_center_y();
void  translate_scene_center(float dx, float dy);
void  update_center_point();
char* default_block_dir = NULL;
int duplicateSection(int trackId, int sectionId, int newTrackId);

int PickedTrack    = -1;
int PickedSection  = -1;
int PickedGraph    = -1;
int PickedMarker   = -1;
int PickedFreeDraw = -1;

bool MeasureMode	= false;
int  MeasurePoints  = 0;

void perform_pick(int canvas, float x, float y);

#pragma mark Local variables
#define TRACK        0
#define SECTION      1
#define SECTION_ANNO 2
#define GRAPH        3
#define GRAPH_ANNO   4

#define DEFAULT_FONT_NAME "arial"

// for limit zooming levels
#define MIN_SCALE (0.02)
#define MAX_SCALE (4000)

// Levels of texture blocks to generate
#define LEVELS (3)

// For separation of image_block_generation and textureset_insertion
typedef struct {
    char *imageFilename;
    MultiLevelTextureSetEX* texset;    
} TextureSet;

int   selected_objects[5];
int   default_track_scene = -1;
float center_x = 0.0f;
float center_y = 0.0f;

bool markers_initialized = false;

// for highlight track and section
static int prevPickedTrack   = -1;
static int prevPickedSection = -1;
static float allScale = 1.0f;

// Is there a good cross platform solution? Or I can only do that in Java?
// A semaphore in scenegraph.java: access with texturelock using calls:
// imageLock(); and imageUnlock();
static std::vector<TextureSet*> tVec;

#pragma mark Utility functions
// private function for update CoreSection metadata // FIXME mmm...
void updateCoreSectionMeta(CoreSection *cs)
{
    // in cm
    float horiDPI = (cs->orientation) ? cs->dpi_y : cs->dpi_x;
    float vertDPI = (cs->orientation) ? cs->dpi_x : cs->dpi_y;
	
	cs->width  = (float)get_texset_src_width(cs->src) * CM_PER_INCH / horiDPI;
	cs->height = (float)get_texset_src_height(cs->src)* CM_PER_INCH / vertDPI;
	
	cs->intervalTop = 0;
	cs->intervalBottom = (cs->width > cs->height) ? cs->width : cs->height;
}

//====================================================================
float get_scene_center_x()
{
    return center_x;
}

//====================================================================
float get_scene_center_y()
{
    return center_y;
}

//====================================================================
void translate_scene_center(float dx, float dy)
{
#ifdef DEBUG
    printf("Translating scene from %f, %f to %f, %f\n",
           center_x, center_y, center_x + dx, center_y + dy);
#endif
	
    /*
	 bool isHorizontalDepth = get_horizontal_depth();
	 if(isHorizontalDepth)
	 {
	 ;
	 }
	 else
	 {
	 float t = dx;
	 dx = dy;
	 dy = t;
	 }
	 */
	
    center_x += dx;
    center_y += dy;
	
    for( int i = 0; i < num_canvases(); ++i)
    {
        float cx, cy, cz;
        int camera;
        
        if(!is_canvas(i)) continue;
        camera = get_canvas_camera(i);
        if(!is_camera(camera)) continue;
		
        
        get_camera_position( camera, &cx, &cy, &cz);
        position_camera( camera, cx + dx, cy + dy, cz);
		
    }
	
}

//=======================================================================
void update_center_point()
{
    int i;
    float max_x = 0.0f, max_y = 0.0f;
    float min_x = 0.0f, min_y = 0.0f;
	
    for( i = 0; i < num_canvases(); ++i)
    {
        if(!is_canvas(i)) continue;
        float x, y, z, w, h;
        int camera;
        camera = get_canvas_camera(i);
        if(!is_camera(camera)) continue;
		
        get_camera_position( camera, &x, &y, &z);
        get_canvas_dimensions( i, &w, &h);
		
        if( x < min_x )     min_x = x;
        if( y < min_y )     min_y = y;
        if( x + w > max_x ) max_x = x + w;
        if( y + h > max_y ) max_y = y + h;
		
    }
	
    center_x = min_x + ( (max_x - min_x) * 0.5f);
    center_y = min_y + ( (max_y - min_y) * 0.5f); 
	
#ifdef DEBUG
    printf("Center Point %f, %f\n", center_x, center_y);
#endif
}

//=======================================================================
void perform_pick(int canvas, float _x, float _y)
{
    float x, y;
	
    if(get_horizontal_depth()) {
        x = _x;
        y = _y;
    } else { // rotate 90 degree to match the coordinates
        x =  _y;
        y = -_x;
    }
	
#ifdef DEBUG
    printf("Pick coord (_x, _y) = %.2f %.2f\n", _x, _y);
    printf("Pick coord (x, y)   = %.2f %.2f\n", x, y);
#endif
	
    prevPickedTrack   = PickedTrack;
    prevPickedSection = PickedSection;
	
    PickedTrack    = -1;
    PickedSection  = -1;
    PickedGraph    = -1;
    PickedMarker   = -1;
    PickedFreeDraw = -1;
	
    // go through zorder of scene's tracks from front to back
    int zlen, *order;
    int i,k,l;
    TrackSceneNode* tsn;
    float cdpix, cdpiy;
    
    if(!is_canvas(canvas)) return;
    get_canvas_dpi(canvas,&cdpix,&cdpiy);
	
	zlen = get_scene_track_zorder_length(default_track_scene);
    if( zlen <= 0 ) return;
	
    order = (int*) malloc( sizeof(int) * zlen);
    get_scene_track_zorder( default_track_scene, order);
	
    for( i = 0; i < zlen && PickedTrack == -1; ++i) // track
    {
        if(!is_track(default_track_scene, order[i])) continue;
		
        tsn = get_scene_track( default_track_scene, order[i] );
		
        // go through each section in track and determine they picked something
        // associated with the section (image, graph, marker?)
        int cs_zlen, *cs_order;
        float tx, ty;
        CoreSection* cs;
		
        cs_zlen = get_track_section_zorder_length(tsn);
		if( cs_zlen <= 0 ) continue;
		
        tx = x - tsn->px;
        ty = y - tsn->py;
        cs_order = (int*) malloc( sizeof(int) * cs_zlen);
        get_track_section_zorder(tsn,cs_order);
		
        // bounding box test
        for( k = 0; k < cs_zlen && PickedSection == -1; ++k) // section
        {
            float w, h;
			
            if(!is_section_model(tsn,cs_order[k])) continue;
			
            cs = get_track_section(tsn,cs_order[k]);
			
            float startPx = cs->px;
            if (cs->src == -1) // no image section
            {
                w = cs->width * INCH_PER_CM;   // inch
                w *= cdpix;                    // pixel in canvas
                h = cs->height * INCH_PER_CM;  // inch
                h *= cdpiy;                    // pixel in canvas
				
                if(cs->orientation == PORTRAIT)
                {
                    float t;
                    w = h;
                    h = t;
                }
            }
            else // image
            {
                w  = get_texset_src_width(cs->src);  // pixel in src width
                h  = get_texset_src_height(cs->src); // pixel in src height
				
                float intTop = (cs->intervalTop / 2.54) * cs->dpi_x;
                float intBot = (cs->intervalBottom / 2.54) * cs->dpi_x;
                float visibleW = intBot - intTop;
				
                // check the orientation
                if(cs->orientation == PORTRAIT)
                {
                    float t;
                    t = w;
                    w = h;
                    h = t;
                }
				
                w = visibleW;
                startPx = cs->px + (cdpix * intTop / cs->dpi_x);
				
                w  /= cs->dpi_x;                     // inch
                h  /= cs->dpi_y;                     // inch
                w  *= cdpix;                         // pixel in canvas
                h  *= cdpiy;                         // pixel in canvas
            }
			
            // check horizontal range
			// offset?
			if (cs->graph_offset == 0.0f)
			{
                if( tx < startPx ) continue;
                if( tx > (startPx + w) ) continue;
			}
			else
			{
				// adjust offset
                int min_var = startPx > startPx + cs->graph_offset ?
				startPx + cs->graph_offset : startPx;
				
                int max_var = startPx + w > startPx + w + cs->graph_offset ?
				startPx + w : startPx + w + cs->graph_offset;
				
                if (tx < min_var) continue;
                if (tx > max_var) continue;
			}
			
            // see if it's part of the actual image before checking 
            // things associated with section
            if( ty >= cs->py && ty <= cs->py + h)
            {
                // Un-highlight previous selected section
                TrackSceneNode *prev_track =
				get_scene_track( default_track_scene, prevPickedTrack );
				
                CoreSection *prev_section = (prev_track) ?
				get_track_section(prev_track, prevPickedSection) : NULL;
				
                if(prev_section)
                {
                    prev_section->highlight = false;
                    set_crosshair_label(NULL);
                }
				
                // Highligh the picked section
                PickedSection = cs_order[k];
                cs->highlight = true;
				
                // set crosshair label
                // char *label = new char[128];
                int bufferSize = 128;
                char * label = (char *) malloc(sizeof(char) * bufferSize);
                {
                    sprintf(label, "section [%s]", get_section_name(cs));
                    set_crosshair_label(label);
                }
                free(label);
				
				// if marker locates inside section image?
				// TODO Check lithology markers
				for(l = 0;
					l < cs->annovec.size() && PickedMarker == -1; 
					l++)
                {
                    CoreAnnotation *ca = cs->annovec[l];
                    if(!ca) continue;
                    float mx, my, mw, mh;
                    mx = ca->m.px;
                    my = ca->m.py;
                    mw = ca->m.w * getMarkerScale();
                    mh = ca->m.h * getMarkerScale();
					
                    if( ty >= my
					   && ty <= my + mh
					   && tx >= mx + startPx
					   && tx <= mx + mw + startPx)
                    {
                        PickedMarker = l;
                    }
				}
				
                continue;
            }
			
            // within the horizontal bounds of a section.  
            // check for graphs
            if( ty < cs->py )
            {
                // traverse marker first and then graph
				
                // test marker
                for( l = 0;
					l < cs->annovec.size() && PickedMarker == -1; 
					l++)
                {
                    CoreAnnotation *ca = cs->annovec[l];
                    if(!ca) continue;
                    float mx, my, mw, mh;
                    mx = ca->m.px;
                    my = ca->m.py;
                    mw = ca->m.w * getMarkerScale();
                    mh = ca->m.h * getMarkerScale();
					
                    if( 	ty >= my
					   && ty <= my + mh
					   && tx >= mx + startPx
					   && tx <= mx + mw + startPx)
                    {
                        PickedMarker = l;
                    }
                }
				
                // test graph
                for( l = 0; 
					l < cs->graphvec.size() && PickedGraph == -1 && 
					PickedMarker == -1; 
					l++)
                {
                    Box* b;
                    b = get_graph_box( cs, cs->graphvec[l] );
                    if(!b) continue;
                    // only need to check vertically
#ifdef DEBUG
                    printf("Checking graph %d\ny:%f h:%f vs. ty:%f\n",
                           l, b->y * cdpiy , b->h * cdpiy, ty);
#endif
                    if( ty >= (b->y * cdpiy) && ty <= ((b->y + b->h) * cdpiy))
                        PickedGraph = cs->graphvec[l];
                    delete b;
                }
#ifdef DEBUG
                printf("Picked Graph %d\n", PickedGraph );
#endif
				
                // test freedraw
                for( l = 0;
					l < cs->freedrawvec.size() && PickedGraph == -1 && 
					PickedMarker == -1 && PickedFreeDraw == -1;
					l++)
                {
                    int fdid = cs->freedrawvec[l];
					
                    if( !is_free_draw_rectangle(fdid)) continue;
					
                    float mx, my, mw, mh;
                    mx = get_free_draw_x(fdid);
                    my = get_free_draw_y(fdid);
                    mw = get_free_draw_width(fdid);
                    mh = get_free_draw_height(fdid);
					
                    if( ty >= my
					   && ty <= my + mh
					   && tx >= mx + startPx
					   && tx <= mx + mw + startPx)
                    {
                        PickedFreeDraw = fdid;
                    }
				}
            }
			
            // check for Annotation Markers
            //if( ty > cs->py + h && PickedGraph == -1) 
            if( ty > cs->py + h && PickedMarker == -1) 
            {
                // TODO Check lithology markers
                for( l = 0;
					l < cs->annovec.size() && PickedMarker == -1; 
					l++)
                {
                    CoreAnnotation *ca = cs->annovec[l];
                    if(!ca) continue;
                    float mx, my, mw, mh;
                    mx = ca->m.px;
                    my = ca->m.py;
                    mw = ca->m.w * getMarkerScale();
                    mh = ca->m.h * getMarkerScale();
					
                    if( 	ty >= my
					   && ty <= my + mh
					   && tx >= mx + startPx
					   && tx <= mx + mw + startPx)
                    {
                        PickedMarker = l;
                    }
				}
			}
			
            if( PickedGraph != -1 || PickedMarker != -1 
			   || PickedFreeDraw != -1)
            {
                // Un-highlight previous selected section
                TrackSceneNode *prev_track =
				get_scene_track( default_track_scene, prevPickedTrack );
				
                CoreSection *prev_section = (prev_track) ?
				get_track_section(prev_track, prevPickedSection) : NULL;
				
                if(prev_section)
                {
                    prev_section->highlight = false;
                    set_crosshair_label(NULL);                    
                }
				
                // Highligh the picked section
                PickedSection = cs_order[k];
                cs->highlight = true;
				
                // set crosshair label
                // char *label = new char[128];
                int bufferSize = 128;
                char * label = (char *) malloc(sizeof(char) * bufferSize);
                {
                    sprintf(label, "section [%s]", get_section_name(cs));
                    set_crosshair_label(label);
                }
                free(label);
            }
			
        } // done going through each section in track
		
        if( PickedSection != -1)
        {
            PickedTrack = order[i];
        }
        else 
        {
            // Didn't pick any section, unhighlight previous picked section
            TrackSceneNode *prev_track =
			get_scene_track( default_track_scene, prevPickedTrack );
			
            CoreSection *prev_section = (prev_track) ?
			get_track_section(prev_track, prevPickedSection) : NULL;
			
            if(prev_section)
            {
                prev_section->highlight = false;
                set_crosshair_label(NULL);                
            }
			
            // check free draws of tracks
            // convert tx, ty from dots to meters
            float fx, fy;
            fx = tx / cdpix * 2.54f / 100.0f;
            fy = ty / cdpiy * 2.54f / 100.0f;
            
            for( int l = 0;
				l < tsn->freedrawvec.size() && PickedFreeDraw == -1;
				l++)
            {
                int fdid = tsn->freedrawvec[l];
                
                if( !is_free_draw_rectangle(fdid)) continue;
                
                float mx, my, mw, mh;
                mx = get_free_draw_x(fdid);
                my = get_free_draw_y(fdid);
                mw = get_free_draw_width(fdid);
                mh = get_free_draw_height(fdid);
				
                if( ty >= my 
				   && ty <= my + mh
				   && tx >= mx 
				   && tx <= mx + mw )
                {
                    PickedFreeDraw = fdid;
                    PickedTrack = order[i];
                }
            }
            
        }
		
        free(cs_order);
		
    } // done going through each track
	
    free(order);
    
}

/* Duplicate a section(trackId, sectionId) to new track newTrackID */
int duplicateSection(int trackId, int sectionId, int newTrackId)
{
	TrackSceneNode * t;
	TrackSceneNode * newTrack;
    CoreSection * cs;
	
    t  = get_scene_track(default_track_scene, trackId);
    if(!t) return -1;
	
    newTrack = get_scene_track(default_track_scene, newTrackId);
    if(!t) return -1;
	
    cs = get_track_section(t, sectionId);
    if(!cs) return -1;
	
    // cs is ready, duplicate the CoreSection structure
    CoreSection* newSection = new CoreSection();
    newSection->g     = cs->g;
    newSection->dpi_x = cs->dpi_x;
    newSection->dpi_y = cs->dpi_y;
    newSection->orientation = cs->orientation;
    newSection->rotangle = cs->rotangle;
    newSection->highlight = cs->highlight;
    newSection->px = cs->px;
    newSection->py = cs->py;
    newSection->draw_vert_line = cs->draw_vert_line;
    newSection->vert_line_x = cs->vert_line_x;
    newSection->movable = cs->movable;
    newSection->name = (char *) "Splited"; // fixme input var
    newSection->width = cs->width;
    newSection->height = cs->height;
    newSection->depth = cs->depth;
    newSection->graph_offset = cs->graph_offset;
    newSection->src = cs->src; // texture index
    inc_texset_ref_count(newSection->src);
	
    // put into newTrack
    newSection->track = newTrackId;
    newSection->section = append_model(newTrack, newSection);
	
    newSection->parentTrack = trackId;
    newSection->parentSection = sectionId;
	
    return newSection->section;
}

#pragma mark Interface implementations
void startUp()
{
    printf("\n--- SceneGraph startUp called ---\n");
    if( default_track_scene < 0) {
        default_track_scene = create_track_scene();    
	}
	
    if( default_track_scene >= 0)
    {
        printf("Default Track Scene Made & Bound: %d \n", default_track_scene);
        bind_scene(default_track_scene);
    }
    else
	{
        printf("Unable to make Default Track Scene\n");
	}
	
    // set the default block directory to imgblocks
    default_block_dir = (char*) malloc( strlen("imgblocks/") + 1);
    strcpy( default_block_dir, "imgblocks/");
	
    // set_current_jnienv(jenv);
    // set_plugin_manager_object(plugin_manager);
	
	
		// Font setups
	 const int kBufferSize = 256;
	 char* fontPath = (char *) malloc(kBufferSize);

#if defined(__APPLE__) || defined(MACOSX)
	CFBundleRef frameworkBundle = CFBundleGetBundleWithIdentifier(CFSTR("org.corewall.LODRender"));		
	
	if(frameworkBundle != NULL)
	{
		CFURLRef fontURL = CFBundleCopyResourceURL( frameworkBundle,
										  CFSTR(DEFAULT_FONT_NAME),
										  CFSTR("ttf"),
										  NULL );
		
		if(fontURL != NULL)
		{
			CFStringRef pathString = CFURLCopyFileSystemPath(fontURL, kCFURLPOSIXPathStyle);				
			CFStringEncoding encoding = kCFStringEncodingASCII;
			
			Boolean b = CFStringGetCString(pathString, fontPath, kBufferSize, encoding);
			if(!b)
			{
				printf("- Copy CFString failed! \n");
			}
		} else {
			printf("- fontURL is null!!!\n");
		}
	}
	else
	{
		printf("- BundleRef is null!!!\n");
	}
#else
	strcpy(logoPath, "./resources/arial.ttf");
#endif
	 
	 set_current_font(queue_font_to_load(fontPath));	
}

void closeDown()
{
    if( default_block_dir ) 
	{
        free(default_block_dir);
	}
	
    printf("\n--- SceneGraph closeDown called ---\n");
    printf("Freeing Default Track Scene\n");
    free_track_scene(default_track_scene);
    
	default_track_scene = -1;
    printf("Freeing all texture resources\n");
	
	//  free_all_texsets(true);
	
	/*  // Free up section annotation markers
	 if (markers_initialized)
	 {
	 printf("\n---- Free up section annotation markers ----\n");
	 free_section_annotation_markers();
	 markers_initialized = false;
	 }
	 */	
}

void setTexBlockDirectory(const char* abspath)
{
    int i = strlen(abspath);
    if(i <= 1)
	{
		return;
	}
	
    if( default_block_dir )
	{
		free(default_block_dir);
	}
	
    default_block_dir = (char*) malloc(i*sizeof(char));
	strcpy(default_block_dir, abspath);
	
    printf("Default Tex Block Directory Now: \"%s\"\n", default_block_dir);	
}

const char *getTexBlockDirectory()
{
	return default_block_dir;
}

int genCanvas(float x, float y, int width, int height, float dpix, float dpiy)
{	
    int id = create_canvas( x, y, (float) width, (float) height, dpix, dpiy);
    if( id >= 0)
	{
        update_center_point();
	}
	
    return id;	
}

void destroyCanvases()
{
    for(int i = 0; i < num_canvases(); ++i)
	{
        free_canvas(i);
	}
	
	//free_all_canvas();
	update_center_point();	
}

int numCanvases()
{
	return num_canvases();
}

void render(int canvasId)
{
    if (!markers_initialized)
    {
        printf("Initializing markers\n");
		
        init_section_annotation_markers();
        markers_initialized = true;
    }
	
    render_canvas(canvasId);	
}

void setCanvasRowsAndColumns(int numRow, int numCol)
{
    set_canvas_rows_and_columns(numRow, numCol);	
}

float getCanvasPositionX(int canvasId)
{
    float x, y, z;
    int camera;
	
    if(!is_canvas(canvasId)) {
		return 0.0;
	}
	
    camera = get_canvas_camera(canvasId);	
    if(!is_camera(camera)) {
		return 0.0;
	}
	
    get_camera_position(camera,&x,&y,&z);
	
    return x;	
}

float getCanvasPositionY(int canvasId)
{
    float x, y, z;
    int camera;
	
    if(!is_canvas(canvasId)) {
		return 0.0;
	}
	
    camera = get_canvas_camera(canvasId);
    if(!is_camera(camera)) {
		return 0.0;
	}
	
    get_camera_position(camera,&x,&y,&z);
	
    return y;	
}

float getCanvasWidth(int canvasId)
{
    float w, h;
	
    if(!is_canvas(canvasId)) {
		return 0.0;
	}
    
    get_canvas_dimensions(canvasId, &w, &h);
	
    return w;	
}

float getCanvasHeight(int canvasId)
{
    float w, h;
	
    if(!is_canvas(canvasId)) {
		return 0.0;
	}
    
    get_canvas_dimensions(canvasId, &w, &h);
	
    return h;		
}

float getSceneCenterX()
{
    return get_scene_center_x();	
}

float getSceneCenterY()
{
    return get_scene_center_y();	
}

void panScene(float dx, float dy)
{
    translate_scene_center(dx,dy);	
}

void scaleScene(float scale)
{
	// limit min/max zoom level
    if(scale == 0.0f)
    {
        scale = 1.0f;
    }
    
    float _allScale = allScale * scale;
	
    if(_allScale < MIN_SCALE || _allScale > MAX_SCALE)
    {
        return;
    }
    else
    {
        allScale = _allScale;
    }
	
    float cx, cy;
    float lx, ly, lz;
    float w, h;
    int id;
	
    cx = get_scene_center_x();
    cy = get_scene_center_y();
	
		// for each canvas, scale it's distance from the center point
		// and scale the dimensions of the canvas
    for( id = 0; id < num_canvases(); ++id)
    {
        int camera;
        if( !is_canvas(id) )
        {
            continue;
        }
		
        camera = get_canvas_camera(id);
		
        if( !is_camera(camera) )
        {
            continue;
        }
		
        get_camera_position(camera,&lx,&ly,&lz);
        get_canvas_dimensions(id,&w,&h);
		
        float dx, dy;
        dx = lx - cx;
        dy = ly - cy;
        dy *= scale;
        dx *= scale;
        
#ifdef DEBUG
        printf("Moving canvas %d, from %.2f, %.2f to %.2f, %.2f\n",
               id, lx, ly, dx + cx, dy + cy);
#endif
		
        position_camera(camera, dx + cx, dy + cy, lz);
        
        lx = (lx + w) - cx;
        ly = (ly + h) - cy;
        lx = lx * scale;
        ly = ly * scale;
		
#ifdef DEBUG
        printf("Resizing canvas %d, from %.2f x %.2f to %.2f x %.2f\n",
               id, w, h, lx - dx, ly - dy);
#endif
		
        set_canvas_dimensions(id, lx - dx, ly - dy);
    }
	
		// update marker scale
		// setMarkerScale(ds);
		// update graph scale
		// setGraphScale(ds);	
}

void enableCanvasGrid(bool enabled)
{
	for(int i = 0; i < num_canvases(); ++i) {
		if(enabled) {
			set_canvas_draw_grid(i, true);
		} else {
			set_canvas_draw_grid(i, false);
		}
	}	
}

void setCanvasGridColor(float r, float g, float b)
{
	for(int i = 0; i < num_canvases(); ++i) {
		set_canvas_grid_color(i, r, g, b);
	}		
}

void setCrossHair(bool hasCrossHair)
{
	set_crosshair(hasCrossHair);
}

bool hasCrossHair()
{
	return has_crosshair();
}

void setCanvasBottomRow(int canvasId, bool isBottom)
{
	set_canvas_bottom_row(canvasId, isBottom);
}

void setCanvasFirstColumn(int canvasId, bool isFirstColumn)
{
	set_canvas_first_column(canvasId, isFirstColumn);
}

int addTrack(const char* trackName)
{
		// FIXME: For this version, it has only one session
	return append_track(default_track_scene, "only_one_session", trackName);
}

int genTextureBlocks(const char* nbuf)
{
	printf("---> [INFO] GenTextureBlocks() %s\n", nbuf);

		// fixme 1. Unicode path,
		// 2. non-exist path
    FILE *f = fopen(nbuf, "r");
    if(!f)
    {
        return -1;
    }
    fclose(f);
	
	TextureSet* ts = (TextureSet*) malloc(sizeof(TextureSet));
		// TextureSet* ts = new TextureSet();
	
    if( strstr(nbuf,".JPEG") || strstr(nbuf,".jpeg") ||
	   strstr(nbuf,".JPG")  || strstr(nbuf,".jpg") )
    {
        ts->texset = create_texset_from_jpeg(nbuf, LEVELS);
		
        if(ts->texset == NULL)
        {
            return -1;
        }
    }
    else if( strstr(nbuf,".PNG") || strstr(nbuf,".png"))
    {
        ts->texset = create_texset_from_png(nbuf, LEVELS);
		
        if(ts->texset == NULL)
        {
            return -1;
        }
    }
    else if( strstr(nbuf,".BMP") || strstr(nbuf,".bmp"))
    {
        ts->texset = create_texset_from_bmp(nbuf, LEVELS);
		
        if(ts->texset == NULL)
        {
            return -1;
        }
    }
    else if( strstr(nbuf,".TIFF") || strstr(nbuf,".tiff") ||
			strstr(nbuf,".TIF")  || strstr(nbuf,".tif") )
    {
        ts->texset = create_texset_from_tiff(nbuf, LEVELS);
		
        if(ts->texset == NULL)
        {
            return -1;
        }
    }
    else if(strstr(nbuf, ".jp2"))
    {
        printf("---> [TODO] Loading JPEG2000: %s\n", nbuf);
        ts->texset = create_texset_from_jp2k(nbuf, LEVELS);
		
        if(ts->texset == NULL)
        {
            return -1;
        }
    }
    else
    {
        printf("Could not load image %s, unsupported format\n", nbuf);
        return -1;
    }
	
	int i = strlen(nbuf);
    ts->imageFilename = (char*) malloc(i * sizeof(char));
    strcpy(ts->imageFilename, nbuf);    
	
		// find a place to insert
    for(int i=0; i<tVec.size(); i++)
    {
        if(tVec[i] == NULL)
        {
            tVec[i] = ts;
            return 1;
        }
    }
    tVec.push_back(ts);
	
    return 1;	
}

int loadImage(const char* imagePath)
{	
		// Search the tVec for common filename
    for(int j=0; j<tVec.size(); j++)
    {
        TextureSet* ts = tVec[j];
        if(ts == NULL) continue;
		
        if(strcmp(ts->imageFilename, imagePath) == 0)
        {
#ifdef DEBUG
            printf("---> [INFO] Hit  %d. file: %s\n", j, imagePath);
#endif
            int k = insert_texset(ts->texset);
			
				// remove ts from tVec;
			free(ts->imageFilename);
            free(ts);
            tVec[j] = NULL;            
			
            return k;
        } else {
#ifdef DEBUG
            printf("---> [INFO] Miss %d, file: %s\n", j, nbuf);
#endif
        }
    }
	
    printf("---> [INFO] No texset available for image: %s\n", imagePath);
	
    return -1;	
}

int addSectionToTrack(int trackId, int sectionId)
{	
	printf("\n--- Add Section To Track called ---\n");
	
    CoreSection* section;
	
    if(!is_track(default_track_scene, trackId))
    {
        printf("ERROR: %d is not a track\n", trackId);
        return -1;
    }
	
    create_section_model(trackId, sectionId, section);
	
    if(!section)
    {
        printf("ERROR: Could not create a section model\n");
        return -1;
    }
	
		// update x position
	TrackSceneNode* t = get_scene_track(trackId);
	section->px = t->nextPos;
	
    return append_model(get_scene_track(default_track_scene, trackId), section);
}

void setSectionName(int trackId, int sectionId, const char* sectionName)
{
	TrackSceneNode* t = get_scene_track(default_track_scene, trackId);
    CoreSection* cs = get_track_section(t, sectionId);
    
    if(!cs) {
		return;
	}
		
	set_section_name(cs, sectionName);
}

void setImageURL(int imageId, const char* urlString)
{
    if(!is_texset(imageId)) {
		return;
	}
	
    set_texset_url(imageId, urlString);
}

int addSectionImageToTrack(int trackId, int sectionId, int imageId)
{
    CoreSection* section;
	
		// fixme printf("\n--- Add Section Image To Track called ---\n");
	
    if(!is_track(default_track_scene, trackId))
    {
        printf("ERROR: %d is not a track\n", trackId);
        return -1;
    }
	
		// fixme printf("\n-- after is_track? --\n");
	
    if(!is_texset(imageId))
    {
        printf("ERROR: %d is not an image\n", imageId);
        return -1;
    }
	
		// fixme printf("\n-- after is_texset --\n");
	
    add_section_image(trackId, sectionId, imageId, 0, section);
	
		// fixme printf("\n-- after add_section_image --\n");
	
    if(!section)
    {
        printf("ERROR: Could not make a section model from image %d\n", imageId);
        return -1;
    }
	
		// fixme printf("\n-- before return: %d --\n", sec);
	
		//return append_model( get_scene_track(default_track_scene,track), section);
	
		// we know that coresection model already added before this point
		// so, just return that coresection id here
	return sectionId;	
}

void setSectionOrientation(int trackId, int sectionId, bool isPortrait)
{
    TrackSceneNode* tsn;
    CoreSection* cs;
	
    tsn = get_scene_track(default_track_scene, trackId);
	
    if(!tsn) return;
    cs = get_track_section(tsn, sectionId);
	
    if(!cs) return;
	
    cs->orientation = isPortrait;
	
    updateCoreSectionMeta(cs);
}

bool getSectionOrientation(int trackId, int sectionId)
{
    TrackSceneNode* tsn;
    CoreSection* cs;
	
    tsn = get_scene_track(default_track_scene,trackId);
    if(!tsn) return 0.0f;
	
    cs  = get_track_section( tsn, sectionId);
    if(!cs) return 0.0f;
	
    return cs->orientation;	
}

void deleteTrack(int trackId)
{
	// printf("\n--- Delete Track called ---\n");
    free_track(default_track_scene, trackId);
	
#ifdef DEBUG
    printf("Freed Track %d in Track Scene %d\n", track, default_track_scene);
#endif
	
}
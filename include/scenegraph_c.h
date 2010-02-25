/*
 *  scenegraph_c.h
 *  JustNative
 *
 *  Created by Julian on 9/1/09.
 *  Copyright 2009 CoreWall.org. All rights reserved.
 *
 */

#ifndef __SCENEGRAPH_C_H__
#define __SCENEGRAPH_C_H__

#pragma mark Done
void startUp();
void closeDown();
void setTexBlockDirectory(const char* abspath);
const char* getTexBlockDirectory();

int genCanvas(float x, float y, int width, int height, float dpix, float dpiy);
void destroyCanvases();
int numCanvases();

void render(int canvasId);
void setCanvasRowsAndColumns(int numRow, int numCol);

float getCanvasPositionX(int canvasId);
float getCanvasPositionY(int canvasId);
float getCanvasWidth(int canvasId);
float getCanvasHeight(int canvasId);

void panScene(float dx, float dy);
void scaleScene(float scale);

float getSceneCenterX();
float getSceneCenterY();

void enableCanvasGrid(bool enabled);
void setCanvasGridColor(float r, float g, float b);

void setCrossHair(bool hasCrossHair);
bool hasCrossHair();

void setCanvasBottomRow(int canvasId, bool isBottom);
void setCanvasFirstColumn(int canvasId, bool isFirstColumn);

int addTrack(const char* trackName);

int genTextureBlocks(const char* imageFilePath);
int loadImage(const char* imagePath);
int addSectionToTrack(int trackId, int sectionIdx); // sectionIdx?

void setSectionName(int trackId, int sectionId, const char* sectionName);
void setImageURL(int imageId, const char* urlString);
int addSectionImageToTrack(int trackId, int sectionId, int imageId);

void setSectionOrientation(int trackId, int sectionId, bool isPortrait);
bool getSectionOrientation(int trackId, int sectionId);

void deleteTrack(int trackId);

#pragma mark To be implemented


#endif

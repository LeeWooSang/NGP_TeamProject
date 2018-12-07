#pragma once
#include "Scene.h"
#include "MyInclude.h"
#include "MyDefine.h"
class SceneManager {
private:
	Scene * pSceneList[10];
	int PreIndex;
public:
	SceneManager();
	~SceneManager();
public:
	void Entry(int index, Scene* scene);
	void Warp(int index);
public:
	Scene* getScene()
	{
		return pSceneList[PreIndex];
	}

	
};
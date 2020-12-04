#pragma once

class Texture
{
public:
	Texture() {}
	~Texture() {}
};

class Texture2D : public Texture
{
public:
	Texture2D();
	~Texture2D();

private:

};

class Image2D : public Texture
{
public:
	Image2D();
	~Image2D();

private:

};
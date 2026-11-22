#pragma once
#include <map>
#include <vector>

#include "raylib.h"
#include "worldgen.h"

class TextureGenerator // Class to build and modify the environment' textures. 
{
private:
    // Sorted map to hold threshold-Color pairs.
    // This will be required for generating a proper texture in reasonable time.
    std::map<float, Color> pattern;
    // Also store desired dimensions of the texture.
    const int width;
    const int height;

    // Private constructor with variables>
    TextureGenerator(std::map<float, Color> pattern, int width, int height)
        : pattern(pattern), width(width), height(height) {}

    // private utility function for generateTexture.
    // It finds the correct color for a noise value via linear interpolation on the pattern.
    Color getColorForNoise(double noise) const;
public:
    // Public factories.
    static TextureGenerator makeGenerator(std::map<float, Color> pattern, Vector2 dimensions);
    static TextureGenerator makeGenerator(std::vector<float> thresholds, std::vector<Color> colors, Vector2 dimensions);
    static TextureGenerator makeGenerator(std::map<float, Color> pattern, int width, int height);
    static TextureGenerator makeGenerator(std::vector<float> thresholds, std::vector<Color> colors, int width, int height);

    // GenerateTexture with a perlin noise as a baseline.
    Texture2D generateTexture(const Perlin& perlin); 
};
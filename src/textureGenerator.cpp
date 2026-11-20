#include "textureGenerator.h"

// TextureGenerator Factory with pre-made pattern.
TextureGenerator TextureGenerator::makeGenerator(std::map<float, Color> pattern, Vector2 dimensions) 
{
    // Direct call to private constructor.
    return TextureGenerator(pattern, static_cast<int>(dimensions.x), static_cast<int>(dimensions.y));
}

// TextureGenerator Factory requiring threshold to color mapping.
TextureGenerator TextureGenerator::makeGenerator(std::vector<float> thresholds, std::vector<Color> colors, Vector2 dimensions) 
{
    // Mapping stage.
    std::map<float, Color> pattern;

    // Check if vectors of equal lenght.
    if (thresholds.size() == colors.size()) 
    {
        for (size_t i = 0; i < thresholds.size(); i++) 
        {
            // Map thresholds to colors.
            // Erases old pair if duplicate found !
            pattern.insert({thresholds[i], colors[i]});
        }
    }

    // Direct call to private constructor with newly made pattern.
    return makeGenerator(pattern, dimensions);
}

// TextureGenerator Factory with pre-made pattern.
TextureGenerator TextureGenerator::makeGenerator(std::map<float, Color> pattern, int width, int height) 
{
    // Direct call to private constructor.
    return TextureGenerator(pattern, width, height);
}

// TextureGenerator Factory requiring threshold to color mapping.
TextureGenerator TextureGenerator::makeGenerator(std::vector<float> thresholds, std::vector<Color> colors, int width, int height) 
{
    // Mapping stage.
    std::map<float, Color> pattern;

    // Check if vectors of equal lenght.
    if (thresholds.size() == colors.size()) 
    {
        for (size_t i = 0; i < thresholds.size(); i++) 
        {
            // Map thresholds to colors.
            // Erases old pair if duplicate found !
            pattern.insert({thresholds[i], colors[i]});
        }
    }

    // Direct call to private constructor with newly made pattern.
    return makeGenerator(pattern, width, height);
}

// Texture generator using perlin noise as a baseline.
Texture2D TextureGenerator::generateTexture(const Perlin& perlin) 
/*
Generates a texture of a given width and height.
Samples the perlin noise at all coordinates and applies color based on chosen pattern.
Creates an Image, converts it to a texture and unloads it.
*/
{
    // Color array to hold the values of every pixel in the texture.
    Color *pixels = (Color *)MemAlloc(width * height * sizeof(Color));

    // Cycle through every pixel in the texture of given dimensions.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Sample noise at given coordinates.
            double noise = perlin.SampleNoise(x, y);
            // Apply color depending on pattern. Append to pixels.
            pixels[y * width + x] = getColorForNoise(noise);
        }
    }

    // Initialise Image object with our dimensions and pixel array.
    // Include alpha channel format for transluscent overlays.
    Image noiseImage = { 0 };
    noiseImage.data = pixels;
    noiseImage.width = width;
    noiseImage.height = height;
    noiseImage.mipmaps = 1;
    noiseImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // Create result texture from image.
    Texture2D Texture = LoadTextureFromImage(noiseImage);
    // Unload Image to avoid memory leaks.
    UnloadImage(noiseImage);

    return Texture;
}

// Picks the appropriate color value for a given pixel, based on pattern.
Color TextureGenerator::getColorForNoise(double noise) const
{
    float val; // Store fraction value of noise in interval.
    Color c1; // Lower bound color.
    Color c2; // Upper bound color.

    float t1; // Lower bound threshold.
    float t2; // Upper bound threshold.

    // Local lambda function for color interpolation.
    // Uses two Colors and a threshold value.
    auto lerpColor = [](Color c1, Color c2, float val) -> Color {
        if (val == 0.0f) { return c1; }
        return (Color){
            static_cast<unsigned char>(c1.r + val * (c2.r - c1.r)),
            static_cast<unsigned char>(c1.g + val * (c2.g - c1.g)),
            static_cast<unsigned char>(c1.b + val * (c2.b - c1.b)),
            static_cast<unsigned char>(c1.a + val * (c2.a - c1.a))
        };
    };

    float t = static_cast<float>(noise);
    // Clamp the value of noise to match threshold.
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // Fallback in case no pattern provided.
    if (pattern.empty()) { return BLACK; }
    
    // If value outside interval, return closest color directly.
    if (noise <= pattern.begin()->first) { return pattern.begin()->second; }
    if (noise >= pattern.rbegin()->first) { return pattern.rbegin()->second; }
    
    // Find the upper bound index first.
    auto upper = pattern.upper_bound(noise);
    // Then get the threshold and color exact values.
    t2 = upper->first;
    c2 = upper->second;
    // Get the previous bound directly through an iterator.
    auto lower = std::prev(upper);
    t1 = lower->first;
    c1 = lower->second;

    // Avoid deviding by zero...again.
    if (t1 == t2) { val = 0.0f; }
    // Calculate the fractional distance of noise between the two.
    else { val = (noise - t1) / (t2 - t1); } 
    
    // Find which colours that references.
    return lerpColor(c1, c2, val);
}
#include "perlin.h"
#include <raylib.h>
#include <cmath>
#include <random>

// Constructor with variables.
// Variables set by default to allow for default constructor usage.
Perlin::Perlin(uint32_t seed, double scale, double persistence) 
    : m_perlin(seed), seed(seed), scale(scale), persistence(persistence) {}

// Simple Getters and Setters.
uint32_t Perlin::GetSeed() const { return seed; }
void Perlin::SetSeed(uint32_t newSeed)
{
    seed = newSeed;
    m_perlin = siv::PerlinNoise(seed);
} // Reseeds the noise to fully reset it.

double Perlin::GetScale() const { return scale; }
void Perlin::SetScale(double newScale) { scale = newScale; }

double Perlin::GetPersistence() const { return persistence; }
void Perlin::SetPersistence(double newPersistence) { persistence = newPersistence; }

ColorTheme Perlin::GetTheme() const { return m_theme;}
void Perlin::SetTheme(ColorTheme theme) { m_theme = theme; }

// ThemeName Getter.
const char* Perlin::GetThemeName() const 
{
    // Returns proper name of theme in case it requires displaying.
    switch (m_theme) {
        case ColorTheme::Space:      return "Space Nebula";
        case ColorTheme::Forest:     return "Terrain / Nature";
        case ColorTheme::Fire:       return "Fire / Magma";
        case ColorTheme::Monochrome: return "Monochrome";
        case ColorTheme::Blood:      return "Bloody";
        case ColorTheme::Ocean:      return "Ocean Blue";
        case ColorTheme::Desert:     return "Terrain / Desert";
        case ColorTheme::Cave:       return "Caves";
        case ColorTheme::Count:      // Defaults to default case, included for completeness.
        default:                     return "Unknown";
    }
}


// Simple 2D sampling function.
double Perlin::SampleNoise(int x, int y) const { return m_perlin.normalizedOctave2D_01(x * scale, y * scale, 4, persistence); }

// Generates and returns a Raylib texture from perlin noise.
Texture2D Perlin::GenerateTexture(int texWidth, int texHeight, double threshold) 
/*
Generates a texture of a given texWidth and texHeight.
Samples the perlin noise at all coordinates and applies color based on ColorTheme and threshold.
Creates an Image, converts it to a texture and unloads it.
*/
{
    // Color array to hold the values of every pixel in the texture.
    Color *pixels = (Color *)MemAlloc(texWidth * texHeight * sizeof(Color));

    // Cycle through every pixel in the texture of given dimensions.
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            // Sample noise at given coordinates.
            double noise = m_perlin.normalizedOctave2D_01(x * scale, y * scale, 4, persistence);
            // Apply color depending on theme and threshold. Append to pixels.
            pixels[y * texWidth + x] = GetColorForNoise(noise, threshold);
        }
    }

    // Initialise Image object with our dimensions and pixel array.
    // Include alpha channel format for transluscent overlays.
    Image noiseImage = { 0 };
    noiseImage.data = pixels;
    noiseImage.width = texWidth;
    noiseImage.height = texHeight;
    noiseImage.mipmaps = 1;
    noiseImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // Create result texture from image.
    Texture2D Texture = LoadTextureFromImage(noiseImage);
    // Unload Image to avoid memory leaks.
    UnloadImage(noiseImage);

    return Texture;
}

// Simple Theme Cycler for ColorTheme variable of perlin class.
void Perlin::CycleTheme() 
{
    // Ensures the current theme in theme enum and not count. Count theme used as modulo.
    m_theme = static_cast<ColorTheme>((static_cast<int>(m_theme) + 1) % static_cast<int>(ColorTheme::Count)); 
}

// Simple private and local function to simplify the GetColorForNoise function.
Color LerpColor(Color c1, Color c2, float val)
// Chooses a color between two given ones, based on a given noise value.
// This results in smoother colour gradients for perlin noise visualisation.
{
    // Color raylib object is {int r, int g, int b,int a}, RGBA channels. 
    return (Color){
        // Picks an in-between color based on given value.
        static_cast<unsigned char>(c1.r + val * (c2.r - c1.r)),
        static_cast<unsigned char>(c1.g + val * (c2.g - c1.g)),
        static_cast<unsigned char>(c1.b + val * (c2.b - c1.b)),
        static_cast<unsigned char>(c1.a + val * (c2.a - c1.a))
    };
}

// Picks the appropariate color value for a given pixel, based on a given noise value, threshold, and ColorTheme.
Color Perlin::GetColorForNoise(double noise, double threshold) const
{
    float t = static_cast<float>(noise);
    // Clamp the value of noise to match threshold.
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // Decide on color value based on chosen ColorTheme m_theme.
    switch (m_theme) {
        // Currently only theme actively based on a given and not default threshold.
        case ColorTheme::Cave: {
            if (t < threshold - 0.01f) {
                return (Color){ 30, 30, 40, 255 };      // Deep Cave Floor
            } 
            else if (t < threshold) {
                return (Color){ 80, 80, 100, 255 };     // Cave Wall Border
            } 
            else {
                return (Color){ 15, 15, 20, 255 };      // Solid Rock
            }
        }

        // Other theme colours based on set values.
        case ColorTheme::Space: {
            if (t < 0.5f) return LerpColor((Color){10, 5, 25, 255}, (Color){30, 20, 80, 255}, t * 2.0f);
            else return LerpColor((Color){30, 20, 80, 255}, (Color){150, 220, 255, 255}, (t - 0.5f) * 2.0f);
        }
        case ColorTheme::Forest: {
            if (t < 0.35f)   return LerpColor((Color){20, 40, 100, 255}, (Color){50, 120, 200, 255}, t / 0.35f);
            else if (t < 0.45f) return LerpColor((Color){50, 120, 200, 255}, (Color){210, 200, 140, 255}, (t - 0.35f) / 0.10f);
            else if (t < 0.75f) return LerpColor((Color){210, 200, 140, 255}, (Color){34, 139, 34, 255}, (t - 0.45f) / 0.30f);
            else                return LerpColor((Color){34, 139, 34, 255}, (Color){240, 240, 240, 255}, (t - 0.75f) / 0.25f);
        }
        case ColorTheme::Fire: {
            if (t < 0.4f)   return LerpColor((Color){10, 0, 0, 255}, (Color){180, 20, 0, 255}, t / 0.4f);
            else if (t < 0.7f) return LerpColor((Color){180, 20, 0, 255}, (Color){255, 140, 0, 255}, (t - 0.4f) / 0.3f);
            else               return LerpColor((Color){255, 140, 0, 255}, (Color){255, 255, 150, 255}, (t - 0.7f) / 0.3f);
        }
        // Monochrome forces all values flatten to greyscale.
        case ColorTheme::Monochrome: {
            unsigned char val = static_cast<unsigned char>(t * 255.0f);
            return (Color){ val, val, val, 255 };
        }
        case ColorTheme::Blood: {
            if (t < 0.5f) return LerpColor((Color){40, 0, 0, 255}, (Color){180, 10, 10, 255}, t * 2.0f);
            else          return LerpColor((Color){180, 10, 10, 255}, (Color){90, 0, 0, 255}, (t - 0.5f) * 2.0f);
        }
        case ColorTheme::Ocean: {
            if (t < 0.4f)   return LerpColor((Color){2, 10, 30, 255}, (Color){10, 50, 120, 255}, t / 0.4f);
            else if (t < 0.8f) return LerpColor((Color){10, 50, 120, 255}, (Color){0, 150, 200, 255}, (t - 0.4f) / 0.4f);
            else               return LerpColor((Color){0, 150, 200, 255}, (Color){200, 240, 255, 255}, (t - 0.8f) / 0.2f);
        }
        case ColorTheme::Desert: {
            if (t < 0.5f) return LerpColor((Color){120, 80, 30, 255}, (Color){220, 180, 100, 255}, t * 2.0f);
            else          return LerpColor((Color){220, 180, 100, 255}, (Color){255, 240, 200, 255}, (t - 0.5f) * 2.0f);
        }
        // Include Count for completeness. Use a single colour to indicate erroneous function if visualised.
        case ColorTheme::Count:
        default:
            return (Color){255, 0, 0, 255};
    }
}
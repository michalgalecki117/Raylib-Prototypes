#pragma once

#include <raylib.h>
#include "PerlinNoise.hpp"

enum class ColorTheme // Simple enum class to simplify switching themes.
{
    Space = 0, // Ensure default theme.
    Forest,
    Fire,
    Blood,
    Monochrome,
    Ocean,
    Desert,
    Cave,
    Count // Not a theme, used as a count of themes to cycle back to the first.
};

class Perlin // Perlin noise class for generating a perlin noise texture, or noise sampling.
{
    public:
    // Constructor with variables.
    // Variables set by default to allow for default constructor to function.
    Perlin(uint32_t seed = 1337, double scale = 0.015, double persistence = 0.5);
    ~Perlin() = default; // Default destructor.
    
    // Simple Getters and Setters.
    uint32_t GetSeed() const;
    void SetSeed(uint32_t newSeed); // Reseeds the perlin noise.

    double GetScale() const;
    void SetScale(double newScale);

    double GetPersistence() const;
    void SetPersistence(double newPersistence);

    ColorTheme GetTheme() const;
    void SetTheme(ColorTheme theme);

    // ThemeName Getter.
    const char* GetThemeName() const;
    

    // Simple 2D sampling function.
    double SampleNoise(int x, int y) const;

    // Generates and returns a Raylib texture from perlin noise.
    // Texture not included in object variables as perlin object not necessarily visual in usage.
    Texture2D GenerateTexture(int texWidth, int texHeight, double threshold = 0.45);
    
    // Simple Theme cycle for visualisation and color-coding.
    void CycleTheme();

private:
    // Private function only used internally.
    // Sets correct color for a noise value, based on theme and threshold.
    Color GetColorForNoise(double noise, double threshold = 0.45) const;

    // Generated perlin noise set itself.
    siv::PerlinNoise m_perlin;
    // Parameters for generation.
    uint32_t seed;
    double scale;
    double persistence;
    // Colorthemme to allow for easy visualisation.
    ColorTheme m_theme = ColorTheme::Space;
};
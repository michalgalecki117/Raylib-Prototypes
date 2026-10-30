#include "gameobject.h"
#include <raylib.h>


GameObject::GameObject() : position{0,0}, color(WHITE) {} // Default constructor.

GameObject::GameObject(Vector2 startPos, Color startColor)     
    : position(startPos)
    , color(startColor)
{} // Constructor with variables.

void GameObject::Draw() const {} // Virtual draw function.

// Simple Getters and Setters.
Vector2 GameObject::GetPosition() const { return position; }
void GameObject::SetPosition(Vector2 newPos) { position = newPos; }

Color GameObject::GetColor() const { return color; }
void GameObject::SetColor(Color newColor) { color = newColor; }

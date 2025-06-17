#ifndef GLOBALS_HPP
#define GLOBALS_HPP

HWND hMain = NULL; // Handle for the main window

const int tickSpacing = 25; // Spacing between grid lines in pixels
const int tickSize = 5;   // Size of the ticks in pixels

const int GraphWidth = 800;  // Width of the graph area in pixels
const int GraphHeight = 600; // Height of the graph area in pixels

const int DGBIndexesLine = 1000; // Base index for dialog group box controls
const int DGBIndexesCircle = 1100; // Base index for circle controls

// Group Box for Drawing
int DGBwidth = 250;
int DGBHeight = 150;

int DGroupBoxLeft = 800 + 30;
int DGroupBoxTop = 40;

// Handles for Group Boxes
HWND hDGBCircle = NULL; // Handle for the Circle group box
HWND hDGBLine = NULL;   // Handle for the Line group box
HWND hDGBEllipse = NULL; // Handle for the Ellipse group box
HWND hDropDown = NULL; // Handle for the drop-down box

COLORREF currColor = RGB(0, 0, 0); // Current color for drawing shapes

#endif // GLOBALS_HPP
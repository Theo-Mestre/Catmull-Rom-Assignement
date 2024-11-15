# 2D Catmull-Rom Curve Assignment

This repository contains code for generating a 2D shape using Catmull-Rom parametric curve segments. This project was completed as part of a math assignment for my Bachelor in Game Programming.

## Assignment Requirements

The tasks involved:
- Drawing a shape using Catmull-Rom curves with SFML as the rendering library
- Writing clean, readable code within a single code file
- Delivering a functional executable within a 2-hour limit

Given the project’s time constraints, I chose a functional approach to quickly start on essential elements without extensive architectural planning. Here’s an outline of my approach:

1. **Initial Setup**: Generated outer and inner circles as anchors for control points.
2. **Control Point Placement**: Calculated angles for each control point and randomly assigned them to one of the circles.
3. **Curve Generation**: Developed Catmull-Rom polynomial calculations for smooth curve generation.
4. **Rendering**: Drew the entire shape based on the generated curves.

With additional time, I added a few features like:
- Shape variation every 5 seconds
- Optional forced alternation of control points between circles

## Try It Yourself

To check out the project:
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/maths-assignment.git
   ```
2. Run `Scripts/SetupSolution.bat` to generate a Visual Studio solution with Premake.

## Acknowledgements

This project uses **[SFML](https://www.sfml-dev.org/)**.

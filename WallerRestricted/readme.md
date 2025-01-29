# WallerRestricted - Discrete Wall-Following Robot Evolution

## Overview
WallerRestricted implements genetic algorithm-based evolution of robot wall-following behavior in a restricted discrete environment. The robot must learn to follow walls efficiently using only 90-degree turns and discrete movements.

## Technical Specifications

### Environment
- 20x20 grid world
- Discrete movement and positioning
- Static obstacles in fixed positions
- Binary wall detection

### Robot Capabilities
- Discrete grid-based movement
- 90-degree turns only
- Simple wall detection
- Position tracking using grid coordinates

### Genetic Algorithm Parameters
- Population size: 500 individuals
- Generations: 51
- Selection ratio: 70% crossover, 30% reproduction
- Maximum individual complexity: 120 nodes
- 5 evaluation runs per individual
- 200 execution steps per run

### Available Functions
- `PROGN3`: Executes three branches sequentially
- `PROGN2`: Executes two branches sequentially
- `IFWALL`: Conditional wall detection branching

### Terminal Operations
- `WALKFRONT`: Move forward one cell
- `WALKBACK`: Move backward one cell
- `RIGHT`: Turn right 90 degrees
- `LEFT`: Turn left 90 degrees

### Fitness Function
Based on:
- Successfully following the predefined path
- Movement efficiency
- Maximum fitness: 60 points per execution

## Project Comparison
This project is part of a series of three robot evolution simulations:

1. WallerGlobal: Uses continuous movement and global perception
2. WallerLocal: Focuses on ball-following with local perception
3. WallerRestricted (this project): Implements discrete movement in a smaller environment

Each variation explores different aspects of evolutionary robotics and navigation strategies.
# Connect4

Author: xavimel
Difficulty: easy

## Description

A game of Connect4! Winning is not enough to get the flag though...

## Analysis

We can observe that:

- the flag is stored in a local variable
- the size of the board (rows, columns) is not constant and is saved in a local variable
- negative numbers are allowed when choosing a column to place the piece in

## Solution

- choose a negative column coinciding with the number-of-columns address (13 should work)
- make it overflow by placing a piece in that column so that the board print prints after the end of the board
- the flag should be visibile in the resulting print

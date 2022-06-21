# Simplified Checkers Game

A simple variant of the traditional checkers game, where the program will generate a move after every user move.

## Setup
An 8x8 chessboard with 12 black and 12 white pieces initially positioned as shown.

<img width="416" alt="Screen Shot 2022-06-21 at 4 53 18 pm" src="https://user-images.githubusercontent.com/107828138/174735381-3a3143a5-be18-44db-a50d-84d153d9611e.png">

The user is side black and the program (computer) generate moves for side white. Black makes a move first.

## Gameplay
In a single turn, the player either makes a **move** or a **capture**

Move (only diagonally forward for one step towards the opponent):

<img width="822" alt="Screen Shot 2022-06-21 at 4 55 55 pm" src="https://user-images.githubusercontent.com/107828138/174735855-ebcdfc7e-9aab-4908-b4ce-519d0fa70370.png">


Capture (jumps forward to an open cell diagonally as shown, landing cell must be empty):

<img width="308" alt="Screen Shot 2022-06-21 at 4 57 05 pm" src="https://user-images.githubusercontent.com/107828138/174736065-275a910e-23a8-449c-aa1e-d4750a0a7d88.png">

Towers: whenever a normal piece reach the furthest row, it becomes a tower and is able to make a move or capture both forward or backward (at maximum 4 possible moves).
Note the cell B1:

<img width="406" alt="Screen Shot 2022-06-21 at 5 03 48 pm" src="https://user-images.githubusercontent.com/107828138/174737115-1d57571c-7b8c-409c-8f41-b4870b6d745a.png">


A player wins the game if it is the opponent's turn and they cannot take action, move or capture, either because no pieces / towers are left or 
no legal moves or capture are possible.

<img width="795" alt="Screen Shot 2022-06-21 at 5 06 22 pm" src="https://user-images.githubusercontent.com/107828138/174737470-9d35b9de-8dea-44dd-b0a6-9577b468aff4.png">

## Implementation

This program implements the minimax algorithm to make decision on which move to take.
If white is making a move, it constructs a tree of all reacheable board configurations, with a depth of 5. So the root node will be the current board configuration\n
and the children nodes will be the boards of all possible moves made by white. Children nodes of these nodes will be all possible moves made by black, we keep doing\n
this until reach a tree height of 5.

We calculate the 'board cost' at the leaf nodes, which is:
Board Cost = b + 3B - w - 3W 

Where b and w are the number of black and white pieces; B and W are the number of black and white towers.
Therefore black moves would want to maximise cost while white moves would want to minimise cost. Board costs of other nodes are imputed by looking at the children nodes with max / min costs if black / white is making a move. This bottom-up imputation will eventually lead to the cost at the root node, where 
a decision is made (choose the board configuration that has this optimal cost).

<img width="831" alt="Screen Shot 2022-06-21 at 5 46 11 pm" src="https://user-images.githubusercontent.com/107828138/174744961-63536b85-9c06-4860-88b8-e785eea1670b.png">

<img width="1134" alt="Screen Shot 2022-06-21 at 5 47 14 pm" src="https://user-images.githubusercontent.com/107828138/174745163-4cfa824e-1875-4756-86fb-243709cc58a4.png">





# forsennait-game-in-c
text-based turn-based game based on probability written in C

RULES:
1)the game starts by first creating the game map, which is a chessboard, this chessboard contains in each cell an object and a danger
-object are medikit, potions material and rocket launcher ammunition.
-danger are traps and alien 
-the probability of encountering each of these things is inserted during the creation of the board

2)once created you can start playing, a player starts, he can decide whether to move on the board or use an object, if he finds himself in front of a trap, he will die, otherwise he could find himself in front of an alien, who will have to decide whether to face and take the object of the cell or escape, if he faces it, he will have a chance of dying, or coming out injured or unharmed

3)each player has 3 energy levels, shield level, life level or vulnerable level, every time he is hit he loses a part of energy, from "shield" he goes to "life", from life he goes to "vulnerable", then he dies, but he can recover energy with medikit (life) potion (shield).

4)What happens if both players are on the same cell? the final fight starts, the accumulated material is counted, which is used to build the floors for the tower, and the number of rocket launchers to shoot them down, whoever remains with the most floors in the tower wins

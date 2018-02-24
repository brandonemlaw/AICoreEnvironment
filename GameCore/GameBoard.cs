using System;

//Allows use of unmanaged code (Board dll)
using System.Runtime.InteropServices;

namespace GameCore
{
    
    public class GameBoard
    {
        //Low Level Board class is used to track the physical board; will increase AI speed
        public Board board;
                
        public const short ROW = 8;
        public const short COL = 8;

        public identity firstPlayer;


        //Returns a copy of the Low Level Board
        public Board getBoard()
        {
            return board;
        }

        //Initializing a New Gameboard
        public void newGameBoard(identity firstPlayer)
        {
            board = new Board();
            this.firstPlayer = firstPlayer;
            printGameBoard();
        }

        public string gameBoardString()
        {
            String result = "";

            //Set the second player attribute to have
            //the opposite identity of the first player
            identity secondPlayer = identity.O;
            if (firstPlayer == identity.O)
            {
                secondPlayer = identity.X;
            }

            //Display each row (loops backwards for right way up)
            for (int i = ROW - 1; i >= 0; i--)
            {
                //Write the row number
                result += " " + (i + 1);
                result += " ";

                //Display each piece
                for (int j = 0; j < COL; j++)
                {
                    char peice = board.getPieceAt(i, j);
                    if (peice == 'W')
                    {
                        result += firstPlayer;
                    }
                    else if (peice == 'B')
                    {
                        result += secondPlayer;
                    }
                    else
                    {
                        result += " ";
                    }


                    result += " ";// Temporary to see what is going on
                }
                result += "\n"; // Temporary to see what is going on

            }

            //Display the column ids
            result += "  A B C D E F G H\n";

            return result;
        }

        public void printGameBoard()
        {
            //Set the second player attribute to have
            //the opposite identity of the first player
            identity secondPlayer = identity.O;
            if (firstPlayer == identity.O)
            {
                secondPlayer = identity.X;
            }

            //Display each row (loops backwards for right way up)
            for (int i = ROW - 1; i >= 0; i--)
            {
                //Write the row number
                Console.Write(i + 1);
                Console.Write(" ");

                //Display each piece
                for (int j = 0; j < COL; j++)
                {
                    char peice = board.getPieceAt(i, j);
                    if (peice == 'W')
                    {
                        Console.Write(firstPlayer);
                    }
                    else if (peice == 'B')
                    {
                        Console.Write(secondPlayer);
                    }
                    else
                    {
                        Console.Write(" ");
                    }


                    Console.Write(" ");// Temporary to see what is going on
                }
                Console.Write("\n"); // Temporary to see what is going on

            }

            //Display the column ids
            Console.Write("  A B C D E F G H\n");
        }




        //Returns true if the game is over and false if the game is not
        public bool gameOver()
        {
            //get the value from the Low Level Board
            return board.isGameOver();
        }

        //Returns the token as a Square object based on the board itself
        public Square getSquareToken(COORD coord)
        {
                        
            //get the piece value from the board, using its 0-7 reference system
            char temp = board.getPieceAt(coord.X, coord.Y);

            //Return the square type represented by the value
            if (temp == 'W')
            {
                return Square.fromIdentity(firstPlayer);
            }
            else if (temp == 'B')
            {
                return Square.fromOppositeIdentity(firstPlayer);
            }
            else
            {
                return Square.S;
            }
        }

        public bool checkMove(identity currentPlayer, Move move)
        {
            bool isFirstsTurn = false;
            if (firstPlayer == currentPlayer)
            {
                isFirstsTurn = true;
            }

            //Try the move check on the Low Abstraction Board
            return board.makeMove(isFirstsTurn, move.Begin.X, move.Begin.Y, move.End.X, move.End.Y, true);
        }


        public bool movePiece(identity currentPlayer, Move move)
        {
            //If the current player is the first player, then white is moving
            bool isFirstsTurn = false;
            if (firstPlayer == currentPlayer)
            {
                isFirstsTurn = true;
            }

            //Execute the move on the Low Abstraction Board
            return board.makeMove(isFirstsTurn, move.Begin.X, move.Begin.Y, move.End.X, move.End.Y, false);
        }

    }


}

    

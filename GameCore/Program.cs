using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AI;

namespace GameCore
{
    class Program
    {
        private static Board savedBoard;
        private static Player savedPlayer;
        private static GameBoard game;

        static void Main(string[] args)
        {
            //Define the players
            Player PlayerX = new Player();
            PlayerX.setPlayer(identity.X);
            Player PlayerO = new AIPlayer();
            PlayerO.setPlayer(identity.O);
            Player currentPlayer = null;

            //Find out the first player, and set currentPlayer
            FirstPlayer(PlayerX, PlayerO, ref currentPlayer);

            //Setup the game board
            game = new GameBoard();
            game.newGameBoard(currentPlayer.getIdentity());

            COORD coord = new COORD();
            Move move = new Move();

            beginGame(currentPlayer.getIdentity(), ref PlayerX, ref PlayerO, ref game, ref move);

        }

        //Find out who the first player is going to be
        private static void FirstPlayer(Player PlayerX, Player PlayerO, ref Player currentPlayer)
        {
            //Find out who goes first
            do
            {
                Console.Write("Who goes first?");
                Console.Write("\n");
                char firstPlayer = Console.ReadKey().KeyChar;

                if (firstPlayer == 'X' || firstPlayer == 'x')
                {
                    PlayerX.turn = true;
                    currentPlayer = PlayerX;
                }
                else
                {
                    PlayerO.turn = true;
                    currentPlayer = PlayerO;
                }
            }
            while (PlayerX.turn == false && PlayerO.turn == false);

            Console.Write("\n");
        }
        //Begin the game
        private static void beginGame(identity firstID, ref Player xPlayer, ref Player oPlayer, ref GameBoard Game,
                                      ref Move move)
        {
            Player currentPlayer;
            if (firstID == identity.X)
            {
                currentPlayer = xPlayer;
            }
            else
            {
                currentPlayer = oPlayer;
            }

            bool AIAutoMove = false;
            bool playerAlwaysMove = false;

            while (!Game.gameOver())
            {
                // Console.Write("AI Favors at " + global::AI.AICore.evaluate1(true, game.getBoard()) + "\n");
                //Console.Write(currentPlayer.getIdentity() + " to move...\n");
                char keyCode = ' ';
                if (AIAutoMove && currentPlayer.getIdentity() == identity.O)
                {
                    Console.Write("\nRequesting Move From AI");
                    move = AIPlayer.getAMove((currentPlayer.getIdentity() == firstID));
                }
                else
                {
                    if (playerAlwaysMove && currentPlayer.getIdentity() == identity.X)
                    {
                        keyCode = 'm';
                    }
                    while (keyCode != 'm' && keyCode != 'a' && keyCode != 's' && keyCode != 'l' && keyCode != 'q' &&  keyCode != 'f' && keyCode != 'i' && keyCode != 'n')
                    {
                        Console.Write((char)currentPlayer.getIdentity() + ":");
                        keyCode = Console.ReadKey().KeyChar;
                        Console.Write("\n");
                    }

                    if (keyCode == 'm')
                    {
                        move = new Move();
                        String line = "        ";
                        while (line.Length != 4 || !(Char.IsDigit(line[1]) && Char.IsDigit(line[3]) && (line[0] - 97) >= 0 && (line[0] - 97) <= 7 && (line[2] - 97) >= 0 && (line[2] - 97) <= 7))
                        {
                            line = Console.ReadLine();
                        }

                        move.Begin.Y = line[0] - 97;
                        move.Begin.X = line[1] - 48 - 1;
                        move.End.Y = line[2] - 97;
                        move.End.X = line[3] - 48 - 1;

                    }
                    else if (keyCode == 'f')
                    {

                    }
                    else if (keyCode == 'n')
                    {
                        if (playerAlwaysMove)
                        {
                            playerAlwaysMove = false;
                        }
                        else
                        {
                            playerAlwaysMove = true;
                        }
                        move = null;
                    }
                    else if (keyCode == 'q')
                    {
                        if (AIAutoMove)
                        {
                            AIAutoMove = false;
                        }
                        else
                        {
                            AIAutoMove = true;
                        }
                        move = null;
                    }
                    else if (keyCode == 'a')
                    {
                        Console.Write("\nRequesting Move From AI");
                        move = AIPlayer.getAMove((currentPlayer.getIdentity() == firstID));
                    }
                    else if (keyCode == 's')
                    {
                        savedBoard = new Board(game.board);
                        savedPlayer = currentPlayer;
                        move = null;
                    }
                    else if (keyCode == 'l')
                    {
                        Game.board = new Board(savedBoard);
                        currentPlayer = savedPlayer;
                        move = null;
                    }
                    else if (keyCode == 'i')
                    {
                        move = null;
                        if (currentPlayer.getIdentity() == identity.X)
                        {
                            currentPlayer = oPlayer;
                        }
                        else
                        {
                            currentPlayer = xPlayer;
                        }
                    }
                }

                bool moved = false;
                if (move != null)
                {
                    moved = Game.movePiece(currentPlayer.getIdentity(), move);
                }
                Console.Clear();
                Game.printGameBoard();

                if (moved)
                {
                    if (currentPlayer.getIdentity() == identity.X)
                    {
                        currentPlayer = oPlayer;
                    }
                    else
                    {
                        currentPlayer = xPlayer;
                    }
                }
            }


        }



        public static Board getBoard()
        {
            return game.getBoard();
        }

        public static bool checkCOORD(Player currentPlayer, Move move)
        {
            if (currentPlayer.getIdentity() == identity.X)
            {
                if (game.getSquareToken(move.Begin) != Square.X)
                {

                    Console.Write("Invalid COORD... Try Again");
                    return false;
                    //Greater than thye loaction your on
                }
                return true;
            }

            if (currentPlayer.getIdentity() == identity.O)
            {
                if (game.getSquareToken(move.Begin) != Square.O)
                {

                    Console.Write("Invalid COORD... Try Again");
                    return false;
                    //Greater than thye loaction your on
                }
                return true;
            }
            else
            {
                Console.Write("Invalid COORD... Try Again");
                return false;
            }
        }

        public static bool checkMove(Player currentPlayer, Move move)
        {
            return game.checkMove(currentPlayer.getIdentity(), move);
        }

        public static identity getFirstPlayer()
        {
            return game.firstPlayer;
        }
    }



}

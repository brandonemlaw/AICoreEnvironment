using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AI;
using System.Collections;

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
            //FirstPlayer(PlayerX, PlayerO, ref currentPlayer);
            currentPlayer = PlayerX;

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
            Player currentPlayer = xPlayer;
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
            identity AIIdentity = identity.O;

            Stack<Board> previousMoves = new Stack<Board>();

            while (!Game.gameOver())
            {
                // Console.Write("AI Favors at " + global::AI.AICore.evaluate1(true, game.getBoard()) + "\n");
                //Console.Write(currentPlayer.getIdentity() + " to move...\n");
                char keyCode = ' ';
                if (AIAutoMove && currentPlayer.getIdentity() == AIIdentity)
                {
                    Console.Write("\nRequesting Move From AI");
                    move = AIPlayer.getAMove((currentPlayer.getIdentity() == firstID));
                }
                else
                {
                    if (playerAlwaysMove && currentPlayer.getIdentity() != AIIdentity)
                    {
                        keyCode = 'm';
                    }
                    Console.Write("m = make move\t a = call AI\t n = always make move for human\t q = toggle auto call AI\nt = change AI (from " + AIIdentity + 
                        ")\ti = swap turns\t l = load board\t s = save board\t u = undo\t f = quit\t \n");
                    while (keyCode != 'm' && keyCode != 'a' && keyCode != 's' && keyCode != 'l' && keyCode != 'q' &&  keyCode != 'f' && keyCode != 'i' && keyCode != 'n' && keyCode != 't' && keyCode != 'u')
                    {
                        Console.Write((char)currentPlayer.getIdentity() + ":");
                        keyCode = Console.ReadKey().KeyChar;
                        Console.Write("\n");
                    }

                    if (keyCode == 'm')
                    {
                        Console.Write("Move: ");
                        String line = "        ";
                        while (line.Length != 0 && (line.Length != 4 || !(Char.IsDigit(line[1]) && Char.IsDigit(line[3]) && (line[0] - 97) >= 0 && (line[0] - 97) <= 7 && (line[2] - 97) >= 0 && (line[2] - 97) <= 7)))
                        {
                            line = Console.ReadLine();
                        }
                        if (line.Length != 0)
                        {
                            move = new Move();
                            move.Begin.Y = line[0] - 97;
                            move.Begin.X = line[1] - 48 - 1;
                            move.End.Y = line[2] - 97;
                            move.End.X = line[3] - 48 - 1;
                        }
                        else
                        {
                            move = null;
                        }


                    }
                    else if (keyCode == 'f')
                    {

                    }
                    else if (keyCode == 't')
                    {
                        if (AIIdentity == identity.X)
                        {
                            AIIdentity = identity.O;
                        }
                        else
                        {
                            AIIdentity = identity.X;
                        }
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
                            Console.Write("Player always move enabled. Enter an empty move to disable.\nPress enter to aknowledge and continue...");
                            Console.ReadLine();
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
                    else if (keyCode == 'u')
                    {
                        if (previousMoves.Count != 0)
                        {
                            Game.board = new Board(previousMoves.Pop());
                            if (currentPlayer.getIdentity() == identity.X)
                            {
                                currentPlayer = oPlayer;
                            }
                            else
                            {
                                currentPlayer = xPlayer;
                            }
                            move = null;
                        }
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
                    previousMoves.Push(new Board(game.board));

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

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

            while (!Game.gameOver())
            {
               // Console.Write("AI Favors at " + global::AI.AICore.evaluate1(true, game.getBoard()) + "\n");
                //Console.Write(currentPlayer.getIdentity() + " to move...\n");
                move = currentPlayer.getMove();

                Game.movePiece(currentPlayer.getIdentity(), move);
                Console.Clear();
                Game.printGameBoard();

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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;


namespace GameCore
{
    class AIPlayer : Player
    {
        public struct AIMove
        {
            public int value;
            public ushort row;
            public ushort col;
            public ushort target;
        };

        //Imports C++ AI DLL
        [DllImport(@"AICoreD.dll")]
        private static extern AIMove AIGetMove(int blackCount, int whiteCount, uint[] blackRows, uint[] whiteRows, bool isWhitesTurn);


        public override Move getMove()
        {
            //Create the resulting move
            Move result = new Move();

            //Get first player identity and board
            identity first = Program.getFirstPlayer();
            Board board = Program.getBoard();

            //Decide if this is whites turn or not
            bool isWhitesTurn = (first == this.getIdentity());


            //Get the move from the AI DLL
            AIMove nextMove = AIGetMove(board.blackCount, board.whiteCount, board.blackRows, board.whiteRows, isWhitesTurn);

            //Convert the AIMove to a Move class
            result.Begin.X = nextMove.row;
            result.Begin.Y = nextMove.col;
            if (isWhitesTurn)
            {
                result.End.X = nextMove.row + 1;
            }
            else
            {
                result.End.X = nextMove.row - 1;
            }
            result.End.Y = nextMove.col + nextMove.target - 1;

            //Return the result
            return result;
        }
    }
}

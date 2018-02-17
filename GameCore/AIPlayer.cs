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
            public uint row;
            public uint col;
            public uint target;
        };

        //Imports C++ AI DLL
        [DllImport(@"AICoreD.dll")]
        private static extern AIMove AIGetMove(int blackCount, int whiteCount, uint[] blackRows, uint[] whiteRows, bool isWhitesTurn);


        public static  Move getAMove(bool isWhitesTurn)
        {
            //Create the resulting move
            Move result = new Move();

            //Get first player identity and board
            identity first = Program.getFirstPlayer();
            Board board = Program.getBoard();


            //Get the move from the AI DLL
            AIMove nextMove = AIGetMove(board.blackCount, board.whiteCount, board.blackRows, board.whiteRows, isWhitesTurn);

            //Convert the AIMove to a Move class
            result.Begin.X = checked((int)nextMove.row);
            result.Begin.Y = checked((int)nextMove.col);
            if (isWhitesTurn)
            {
                result.End.X = checked((int)nextMove.row + 1);
            }
            else
            {
                result.End.X = checked((int)nextMove.row - 1);
            }
            result.End.Y = checked((int)(nextMove.col + nextMove.target - 1));

            //Return the result
            return result;
        }
    }
}

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
        private static extern AIMove AIGetMove(int blackCount, int whiteCount, uint[] blackRows, uint[] whiteRows, bool isWhitesTurn, uint mode);


        public static  Move getAMove(bool isWhitesTurn)
        {
            //Create the resulting move
            Move result = new Move();

            //Get first player identity and board
            identity first = Program.getFirstPlayer();
            Board board = Program.getBoard();

            //Convert to AISpace
            uint[] AICOLUMNS = new uint[] { 1, 2, 4, 8, 16, 32, 64, 128 }; //A-H

            uint[] blackRows = new uint[8];
            uint[] whiteRows = new uint[8];


            for (int i = 0; i < 8; i++)
            {
                blackRows[i] = 0;
                whiteRows[i] = 0;
            }

            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (board.blackRows[i] % board.COLUMNS[j] == 0)
                    {
                        blackRows[i] += AICOLUMNS[j];
                    }
                    if (board.whiteRows[i] % board.COLUMNS[j] == 0)
                    {
                        whiteRows[i] += AICOLUMNS[j];
                    }
                }
            }


            //Get the move from the AI DLL
            AIMove nextMove = AIGetMove(board.blackCount, board.whiteCount, blackRows, whiteRows, isWhitesTurn, 0);

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

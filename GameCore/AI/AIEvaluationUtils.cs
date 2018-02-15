using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AI
{
    class EvaluationUtils
    {
        public static int evalBDepth(ref GameCore.Board board)
        {
            int result = 0;
            int i = 1;
            bool end = false;
            while (i < 6 && !end)
            {
                if (board.blackRows[i] != 1)
                {
                    /*for (int j = 0; j < 8; j++)
                    {
                        if (board.blackRows[i] % board.COLUMNS[j] == 0)
                        {
                            result += (6 - i);
                        }
                    }

                    end = true;*/
                    return 6 - i;
                }
                i++;
            }
            return result;
        }
        public static int evalWDepth(ref GameCore.Board board)
        {
            int result = 0;
            int i = 6;
            bool end = false;

            while (i > 1 && !end)
            {
                if (board.whiteRows[i] != 1)
                {
                    /* for (int j = 0; j < 8; j++)
                     {
                         if (board.whiteRows[i] % board.COLUMNS[j] == 0)
                         {
                             result += i - 1;
                         }
                     }
                     end = true;*/
                    return i - 1;
 
                 }
                    i--;
            }
            return result;
        }

    }

}
